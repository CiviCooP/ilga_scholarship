<?php
/**
 *  Drupal view handler for making to filter on world region
 *
 * @author Klaas Eikelboom (CiviCooP)
 * @date august 2018
 * @license AGPL-3.0
 */

class ilga_scholarship_handler_filter_sessionverdicts extends views_handler_filter_in_operator {

  function construct() {
    parent::construct();
    if (!civicrm_initialize()) {
      return;
    }
  }

  public function get_value_options() {
    if (isset($this->value_options)) {
      return $this->value_options;
    }
    else {
      $this->value_options = ilga_scholarship_helper::customFieldOptions(variable_get(ILGA_SCHOLARSHIP_SESSION_VERDICT_CUSTOM_FIELD));
      $this->value_options['todo'] = 'To Do';
      $this->value_options['unvoted'] = 'Unvoted';
      return $this->value_options;
    }
  }

  function operators() {
    $operators = parent::operators();
    return $operators;
  }

  function op_simple() {
    if (empty($this->value)) {
      return;
    }

    $submissionIdColumn = ilga_scholarship_helper::custom_column_name(variable_get(ILGA_SCHOLARSHIP_SUBMISSION_CUSTOM_FIELD));
    $verdictColumn = ilga_scholarship_helper::custom_column_name(variable_get(ILGA_SCHOLARSHIP_SESSION_VERDICT_CUSTOM_FIELD));
    $sessionTable = '{'.ilga_scholarship_helper::custom_table_name('Sessions_proposal_evaluation').'}';

    $this->ensure_my_table();
    $activityTypeId = variable_get(ILGA_SCHOLARSHIP_SESSION_ACTIVITY_TYPE);
    $contactId = ilga_scholarship_helper::currentUser();
    $value=$this->value[0];
    if($value=='todo'){
       $clause = "not exists (select 1 from $sessionTable cv
                              join {civicrm_activity} a on (cv.entity_id=a.id and a.activity_type_id = $activityTypeId)                             
                              join {civicrm_activity_contact} ac on (ac.activity_id = a.id and record_type_id=1 and ac.contact_id=$contactId)
                              where cv.$submissionIdColumn = webform_submissions.sid
       )";
    } else if ($value=='unvoted')
      $clause = "not exists (select 1 from $sessionTable cv
                              join {civicrm_activity} a on (cv.entity_id=a.id and a.activity_type_id = $activityTypeId)                                                         
                              where cv.$submissionIdColumn = webform_submissions.sid
       )";
    else {
      $clause = "exists (select 1 from $sessionTable cv
                              join {civicrm_activity} a on (cv.entity_id=a.id and a.activity_type_id = $activityTypeId)                             
                              join {civicrm_activity_contact} ac on (ac.activity_id = a.id and record_type_id=1 and ac.contact_id=$contactId)
                              where cv.$submissionIdColumn = webform_submissions.sid
                              and   cv.$verdictColumn=$value
       )";
    }

    $this->query->add_where_expression($this->options['group'], $clause);
  }

}