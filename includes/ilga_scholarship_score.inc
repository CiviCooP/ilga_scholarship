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
  private $identity;
  private $beenBefore;
  private $workShop;
  private $birthDate;
  private $country;
  private $stateProvince;
  private $stateProvinceName;
  private $referenceDateYounger30;
  private $referenceDateOlder60;

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
    $this->identity = $values[variable_get(ILGA_SCHOLARSHIP_SEXUAL_IDENTITY_CUSTOM_FIELD)];
    $this->beenBefore = $values[variable_get(ILGA_SCHOLARSHIP_BEEN_BEFORE_CUSTOM_FIELD)];
    $this->workShop = $values[variable_get(ILGA_SCHOLARSHIP_WORKSHOP_CUSTOM_FIELD)];

    $client_id = $values['client_id'];
    $contactId = $client_id[1];

    $contact = civicrm_api3('Contact','getsingle',['id' => $contactId]);
    $this->birthDate = $contact['birth_date'];
    $this->country   = $contact['country'];
    $this->stateProvince   = $contact['state_province'];
    $this->stateProvinceName   = $contact['state_province_name'];

    $date = variable_get(ILGA_SCHOLARSHIP_REFERENCE_DATE);
    $this->referenceDateYounger30 = ($date['year'] - 30) .'-' . str_pad($date['month'],2,'0',STR_PAD_LEFT) . '-' .str_pad($date['day'],2,'0',STR_PAD_LEFT);
    $this->referenceDateOlder60 = ($date['year'] - 60) .'-' . str_pad($date['month'],2,'0',STR_PAD_LEFT) . '-' .str_pad($date['day'],2,'0',STR_PAD_LEFT);

  }

  /**
   * Workhorse of the procedure. Does the calculation
   *
   * @throws \CiviCRM_API3_Exception
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

    if($this->beenBefore) {
      $this->score -= 10;
      $this->explanation[] = '-10 points because is not the first visit';
    }

    if($this->beenBefore) {
      $this->score +=10;
      $this->explanation[] = '10 points because plans to apply for workshop';
    }

    if ($this->birthDate > $this->referenceDateYounger30) {
      $this->score += 10;
      $this->explanation[] = '10 for being younger than 30 on conference date';
    }
    if ($this->birthDate < $this->referenceDateOlder60) {
      $this->score += 10;
      $this->explanation[] = '10 for being older than 60 on conference date';
    }
    if (in_array($this->country, ['Uganda', 'Iran', 'Russia'])) {
      $this->score += 10;
      $this->explanation[] = "10 points for being resident of {$this->country}";
    }

    if(is_array($this->identity)){
      foreach($this->identity as $identityId){
          $label = $result = civicrm_api3('OptionValue', 'getvalue', array(
            'return' => "label",
            'value' => $identityId,
            'option_group_id' => "how_do_you_identify__20180621160343",
          ));
          if(in_array($label,['Trans','Intersex','Non-binary','Bisexual'])){
            $this->score += 10;
            $this->explanation[] = "10 points for identity {$label}";
          }
      }
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
