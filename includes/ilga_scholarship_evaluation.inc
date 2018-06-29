<?php
/**
 * Calculcates the total score of an evaluation.
 *
 * @author Klaas Eikelboom (CiviCooP)
 * @date 25 juni 2018
 * @license AGPL-3.0
 */

class ilga_scholarship_evaluation {

  private $activityId;

  private $caseId;

  private $sum;

  /**
   * ilga_scholarship_evaluation constructor.
   *
   * @param $activityId
   */
  public function __construct($activityId) {
    $this->activityId = $activityId;
  }

  private function findCaseId() {

    $this->caseId = CRM_Core_DAO::singleValueQuery("
       select case_id from civicrm_case_activity
       where activity_id = %1", [
      1 => [$this->activityId, 'Integer'],
    ]);
  }

  private function calculateSum() {
    $this->sum = CRM_Core_DAO::singleValueQuery("
    select sum(cv.score_63) from civicrm_case_activity ca
    join civicrm_value_evaluation_8 cv on (ca.activity_id=cv.entity_id)
    where ca.case_id=%1", [
      1 => [$this->caseId, 'Integer'],
    ]);
  }

  private function store(){

    civicrm_api3('Case','create',[
      'id' => $this->caseId,
      variable_get(ILGA_SCHOLARSHIP_EVALUATION_TOTAL_CUSTOM_FIELD) => $this->sum,
    ]);
  }

  public function process() {
    $this->findCaseId();
    $this->calculateSum();
    $this->store();
  }


}