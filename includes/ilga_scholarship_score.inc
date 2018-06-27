<?php
/**
 * Contains all the logic to calculate all the scholarship score's
 *
 * @author Klaas Eikelboom (CiviCooP)
 * @date 25 juni 2018
 * @license AGPL-3.0
 */

class ilga_scholarship_score{


  private $caseId;
  private $disability;
  private $birthDate;
  private $country;
  private $stateProvince;
  private $stateProvinceName;

  /**
   * @var int Result of the score calculation
   */
  private $score= 0;

  /**
   * @var array Explanation how the result is calculated
   */
  private $explanation = [];

  /**
   * _Score constructor.
   *
   * @param $caseId for which the score must be calculated
   */
  public function __construct($caseId) {
    $this->caseId = $caseId;
  }

  /**
   * Reads score information form the database
   * @throws \CiviCRM_API3_Exception
   */
  public function getInfo(){
    $values = civicrm_api3('Case','getsingle',['id'=>$this->caseId]);
    $this->disability = $values[variable_get(ILGA_SCHOLARSHIP_DISABILITY_CUSTOM_FIELD)];

    $client_id = $values['client_id'];
    $contactId = $client_id[1];

    $contact = civicrm_api3('Contact','getsingle',['id' => $contactId]);
    $this->birthDate = $contact['birth_date'];
    $this->country   = $contact['country'];
    $this->stateProvince   = $contact['state_province'];
    $this->stateProvinceName   = $contact['state_province_name'];


  }

  /**
   * Workhorse of the procedure. Does the calculation
   */
  public function calculate() {
    /*
     * Each step has the same structure. Points are added to (or extracted from
     * the score). In the explanation the reason is described
     */
    $this->score = 0;
    if ($this->disability) {
      $this->score += 10;
      $this->explanation[] = '10 points for having a disability';
    }

    if ($this->birthDate > '1989-07-01') {
      $this->score += 10;
      $this->explanation[] = '10 for being younger than 30 on conference data';
    }
    if ($this->birthDate < '1959-07-01') {
      $this->score += 10;
      $this->explanation[] = '10 for being older than 60 on conference data';
    }
    if (in_array($this->country, ['Uganda', 'Iran', 'Russia'])) {
      $this->score += 10;
      $this->explanation[] = "10 points for being resident of {$this->country}";
    }

    if ($this->country == 'United States' && in_array($this->stateProvince, [
        'MS',
        'AL',
        'GA'
      ])) {
      $this->score += 10;
      $this->explanation[] = "10 points for being resident of {$this->stateProvinceName}";
    }
  }

  /**
   * Write the result back to the case custom fields
   * @throws \CiviCRM_API3_Exception
   */
  public function set(){

    civicrm_api3('Case','create',array(
        'id' =>$this->caseId,
        variable_get(ILGA_SCHOLARSHIP_SCORE_CUSTOM_FIELD) => $this->score,
        variable_get(ILGA_SCHOLARSHIP_SCORE_EXPLANATION_CUSTOM_FIELD) => implode("\n",$this->explanation)
      )
    );
  }

  /**
   * Umbrella procedure that executes all the code.
   * @throws \CiviCRM_API3_Exception
   */
  public function process(){
    $this->getInfo();
    $this->calculate();
    $this->set();
  }
}
