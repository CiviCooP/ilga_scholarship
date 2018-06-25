<?php

class ilga_scholarship_helper{

   public static function currentUser() {
     civicrm_initialize();
     $session = CRM_Core_Session::singleton();
     return $session->get('userID');
   }

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