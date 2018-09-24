<?php
/**
 * Shows the summary of the session voting process
 *
 * @author Klaas Eikelboom (CiviCooP)
 * @date 24 september 2018
 * @license AGPL-3.0
 */
class ilga_scholarship_session_summary extends views_handler_field {

  function construct() {
      parent::construct();
    }
    function query() {
      // do nothing -- to override the parent query.
    }

  function render($values) {
    return ilga_scholarship_helper::sessionSummary($values->sid);
  }

}


