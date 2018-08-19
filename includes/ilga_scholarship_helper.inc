<?php
/**
 * Class with static helper functions
 *
 * @author Klaas Eikelboom (CiviCooP)
 * @date 25 juni 2018
 * @license AGPL-3.0
 */

class ilga_scholarship_helper{

  private static function activityId($caseId,$contactId){

    return CRM_CORE_DAO::singleValueQuery(
      "select act.id
       from civicrm_activity act
       join civicrm_case_activity ca on (act.id = ca.activity_id and ca.case_id = %1)
       join civicrm_activity_contact ac on (ac.activity_id = ca.activity_id and ac.record_type_id = %2)
       where act.activity_type_id = %3 and ac.contact_id=%4 and act.is_deleted=0 and act.is_current_revision=1",[
         1 => [$caseId,'Integer'],
         2 => [2,'Integer'], // record type id
         3 => [variable_get(ILGA_SCHOLARSHIP_EVALUATE_ACTIVITY_TYPE),'Integer'],
         4 => [$contactId,'Integer']
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

     $result = civicrm_api3('Case', 'getsingle', array(
       'return' => array("contact_id"),
       'id' => $caseId,
     ));
     $clientId = array_shift($result['client_id']);

     $query = [
       'cid1' => $clientId,
       'cid2' => ilga_scholarship_helper::currentUser(),
       'case1' => $caseId,
     ];

     $activityId = self::activityId($caseId,self::currentUser());
     if($activityId){
       $query['activity1'] = $activityId;
     }

     return l('Approve', 'node/' . variable_get(ILGA_SCHOLARSHIP_EVALUATION_WEBFORM_NID), [
         "query" => $query,
       ]
     );
   }

   static public function custom_table_name($name){
     return civicrm_api3('CustomGroup','getvalue',[
       'name' => $name,
       'return' => 'table_name',
     ]);
   }

  static public function custom_column_name($customName){
    $id = substr($customName,7);
    return civicrm_api3('CustomField','getvalue',[
      'id' => $id,
      'return' => 'column_name',
    ]);
  }

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




}