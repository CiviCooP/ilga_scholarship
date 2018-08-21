<?php
/**
 * Adds a view element
 *
 * @author Klaas Eikelboom (CiviCooP)
 * @date 25 juni 2018
 * @license AGPL-3.0
 */
class ilga_scholarship_evaluate_session_link extends views_handler_field {

  function construct() {
      parent::construct();
      module_load_include('inc', 'webform', 'includes/webform.submissions');
    }
    function query() {
      // do nothing -- to override the parent query.
    }

  function render($values) {
    $submission = webform_get_submission(variable_get(ILGA_SCHOLARSHIP_SESSION_WEBFORM_NID),$values->sid);
    return ilga_scholarship_helper::session_approval_link($submission->civicrm['contact'][1]['id']);
  }

}


