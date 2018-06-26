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

     return l('Approve', 'node/' . variable_get(ILGA_SCHOLARSHIP_EVALUATION_WEBFORM_NID), [
         "query" => $query,
       ]
     );
   }


}