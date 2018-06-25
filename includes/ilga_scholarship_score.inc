<?php
/**
 * Created by PhpStorm.
 * User: klaas
 * Date: 25-6-18
 * Time: 10:00
 */

class ilga_scholarship_score{


  private $caseId;
  private $disability;
  private $oppressed;
  private $score= 0;
  private $explanation = [];
  private $birthDate;
  private $country;

  /**
   * _Score constructor.
   *
   * @param $caseId
   */
  public function __construct($caseId) {
    $this->caseId = $caseId;
  }

  public function getInfo(){
    $values = civicrm_api3('Case','getsingle',['id'=>$this->caseId]);
    $this->disability = $values[variable_get(ILGA_SCHOLARSHIP_DISABILITY_CUSTOM_FIELD)];
    $this->oppressed=$values[variable_get(ILGA_SCHOLARSHIP_OPPRESSED_CUSTOM_FIELD)];

    $client_id = $values['client_id'];
    $contactId = $client_id[1];

    $contact = civicrm_api3('Contact','getsingle',['id' => $contactId]);
    $this->birthDate = $contact['birth_date'];
    $this->country   = $contact['country'];


  }

  public function calculate(){
    $this->score=0;
    if($this->disability){
      $this->score+=10;
      $this->explanation[]= '10 points for having a disability';
    }
    if($this->oppressed){
      $this->score+=10;
      $this->explanation[]= '10 points for being part of an oppressed group';
    }
    if($this->birthDate > '1989-07-01'){
      $this->score+=10;
      $this->explanation[]= '10 for being younger than 30 on conference data';
    }
    if($this->birthDate < '1959-07-01'){
      $this->score+=10;
      $this->explanation[]= '10 for being older than 60 on conference data';
    }
    if(in_array($this->country,['Uganda','Iran','Russia'])){
      $this->score+=10;
      $this->explanation[]= "10 points for being resident of {$this->country}";
    }
  }

  public function set(){

    civicrm_api3('Case','create',array(
        'id' =>$this->caseId,
        variable_get(ILGA_SCHOLARSHIP_SCORE_CUSTOM_FIELD) => $this->score,
        variable_get(ILGA_SCHOLARSHIP_SCORE_EXPLANATION_CUSTOM_FIELD) => implode("\n",$this->explanation)
      )
    );
  }

  public function process(){
    $this->getInfo();
    $this->calculate();
    $this->set();
  }
}
