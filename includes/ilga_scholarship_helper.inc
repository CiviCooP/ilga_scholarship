<?php
/**
 * Class with static helper functions
 *
 * @author Klaas Eikelboom (CiviCooP)
 * @date 25 juni 2018
 * @license AGPL-3.0
 */

class ilga_scholarship_helper{

  /**
   * @param $caseId
   * @param $contactId
   *
   * @return null|string
   */
  private static function activityId($caseId,$contactId){

    return CRM_CORE_DAO::singleValueQuery(
      "select act.id
       from civicrm_activity act
       join civicrm_case_activity ca on (act.id = ca.activity_id and ca.case_id = %1)
       join civicrm_activity_contact ac on (ac.activity_id = ca.activity_id)
       join   civicrm_option_value   ov on (ov.value = ac.record_type_id and ov.name='Activity Assignees')
       join   civicrm_option_group   og on (ov.option_group_id = og.id and og.name ='activity_contacts')
       where act.activity_type_id = %3 and ac.contact_id=%4 and act.is_deleted=0 and act.is_current_revision=1",[
         1 => [$caseId,'Integer'],
         2 => [2,'Integer'], // record type id
         3 => [variable_get(ILGA_SCHOLARSHIP_EVALUATE_ACTIVITY_TYPE),'Integer'],
         4 => [$contactId,'Integer']
      ]);
  }

  /**
   * @param $contactId
   * @param $evaluatorId
   *
   * @return null|string
   */
  private static function sessionActivityId($contactId,$evaluatorId){

    return CRM_CORE_DAO::singleValueQuery(
      "select act.id from civicrm_activity act
       join   civicrm_activity_contact ac1 on (ac1.activity_id = act.id and ac1.contact_id=%1)
       join   civicrm_option_value     ov1 on (ov1.value = ac1.record_type_id and ov1.name='Activity Source')
       join   civicrm_option_group     og1 on (ov1.option_group_id = og1.id and og1.name ='activity_contacts')
       join   civicrm_activity_contact ac2 on (ac2.activity_id = act.id and ac2.record_type_id = 1 and ac2.contact_id=%2)
       join   civicrm_option_value     ov2 on (ov2.value = ac2.record_type_id and ov2.name='Activity Assignees')
       join   civicrm_option_group     og2 on (ov2.option_group_id = og2.id and og2.name ='activity_contacts')
       where  act.activity_type_id = %3    and act.is_current_revision =1",[
      1 => [$contactId,'Integer'],
      2 => [$evaluatorId,'Integer'], // record type id
      3 => [variable_get(ILGA_SCHOLARSHIP_SESSION_ACTIVITY_TYPE),'Integer'],
    ]);
  }

  /**
   * @return int Contact Id of the current user. Needed to find the
   * details of the evaluator
   */
  public static function currentUser() {
     civicrm_initialize();
     $session = CRM_Core_Session::singleton();
     return $session->get('userID');
   }

  /**
   * @param $caseId to create the approval link for
   *
   * @return string
   * @throws \CiviCRM_API3_Exception
   */
  static public function approval_link($caseId){

    try{

     $result = civicrm_api3('Case', 'getsingle', array(
       'return' => array("contact_id"),
       'id' => $caseId,
     ));

     $clientId = array_shift($result['client_id']);

     $query = [
       'cid1' => ilga_scholarship_helper::currentUser(),
       'cid2' => $clientId,
       'case1' => $caseId,
     ];

     $activityId = self::activityId($caseId,self::currentUser());
     if($activityId){
       $query['activity1'] = $activityId;
     }

     return l('Evaluate', 'node/' . variable_get(ILGA_SCHOLARSHIP_EVALUATION_WEBFORM_NID), [
         "query" => $query,
       ]
     );
   } catch (Exception $ex){
      return 'Oops - something went wrong';
    }
  }

  /**
   * @param $contactId of the contact that proposed the session
   *
   * @return string
   * @throws \CiviCRM_API3_Exception
   */
  static public function session_approval_link($sid,$proposerId){

    $activityId = CRM_Scholarship_Utils::findActivityBySid($sid,ilga_scholarship_helper::currentUser());
    if($activityId){
      $query['activity1'] = $activityId;

      $value = civicrm_api3('Activity','getvalue',
        ['id' => $activityId,'return'=>variable_get(ILGA_SCHOLARSHIP_SESSION_VERDICT_CUSTOM_FIELD)]
      );

      $label = self::fieldLabel(variable_get(ILGA_SCHOLARSHIP_SESSION_VERDICT_CUSTOM_FIELD),$value);
    } else {
      $label = 'Approve Session';
    }

    return l($label, "scholarship/votesession/$sid/$proposerId");


  }

  /**
   * @param $name
   *
   * @return array
   * @throws \CiviCRM_API3_Exception
   */
  static public function custom_table_name($name){
     return civicrm_api3('CustomGroup','getvalue',[
       'name' => $name,
       'return' => 'table_name',
     ]);
   }

  /**
   * @param $customName
   *
   * @return array
   * @throws \CiviCRM_API3_Exception
   */
  static public function custom_column_name($customName){
    $id = substr($customName,7);
    return civicrm_api3('CustomField','getvalue',[
      'id' => $id,
      'return' => 'column_name',
    ]);
  }

  /**
   * @param $caseId
   *
   * @return string
   * @throws \CiviCRM_API3_Exception
   */
  static public function evaluationSummary($caseId){
     $activityTypeId = variable_get(ILGA_SCHOLARSHIP_EVALUATE_ACTIVITY_TYPE);
     $optionGroupName = variable_get(ILGA_SCHOLARSHIP_EVALUATION_OPTION_GROUP);
     $columnName = self::custom_column_name(variable_get(ILGA_SCHOLARSHIP_EVALUATION_CUSTOM_FIELD));
     $tableName = self::custom_table_name('Evaluation');
     $sql = "
     select {$columnName}, ov.label, ov.value, count(1) count
     from civicrm_case_activity ca
     join civicrm_activity act on (ca.activity_id = act.id and act.activity_type_id=%3)
     join {$tableName} cv on (cv.entity_id = act.id)
     join civicrm_option_value ov on (ov.value = $columnName)
     join civicrm_option_group og on (ov.option_group_id = og.id and og.name=%2)
     where case_id = %1
     group by {$columnName}, ov.label
     order by ov.value desc
     ";

     $output = "";

     $dao = CRM_Core_DAO::executeQuery($sql,
       [
         1 => [$caseId,'Integer'],
         2 => [$optionGroupName,'String'],
         3 => [$activityTypeId, 'Integer']
       ]);

     while($dao->fetch()){
       $output .= $dao->count. " ".$dao->label . "<br/>";
     }
     return $output;
   }

  /**
   * @param $fieldName
   * @param $value
   *
   * @return string
   */
  static function fieldLabel($fieldName,$value){
    try {
      $fieldId = substr($fieldName, 7);
      $optionGroupId = civicrm_api3('CustomField', 'getvalue',
        ['id' => $fieldId, 'return' => 'option_group_id']);
      $label = civicrm_api3('OptionValue', 'getvalue',
        [
          'value' => $value,
          'option_group_id' => $optionGroupId,
          'return' => 'label'
        ]);
      return $label;
    }
    catch (CiviCRM_API3_Exception $ex){
      Civi::log()->error("Scholarship fieldLabel $fieldName,$value");
      Civi::log()->error($ex->getMessage());
      return 'fieldLabel Error';
    }
   }

  /**
   * @param $fieldName
   *
   * @return array
   */
  static function customFieldOptions($fieldName) {
    try {
      $fieldId = substr($fieldName, 7);
      $optionGroupId = civicrm_api3('CustomField', 'getvalue',
        ['id' => $fieldId, 'return' => 'option_group_id']);
      $result = civicrm_api3('OptionValue', 'get',
        [
          'option_group_id' => $optionGroupId,
        ]);
      $options = [];
      foreach ($result['values'] as $value){
        $options[$value['value']] = $value['label'];
      }
      return $options;
    } catch (CiviCRM_API3_Exception $ex) {
      Civi::log()->error("Oops no custom values found for $fieldName");
      return ['Oops'];
    }

   }
}