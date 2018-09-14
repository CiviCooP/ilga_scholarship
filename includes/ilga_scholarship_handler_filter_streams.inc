<?php
/**
 *  Drupal view handler for making to filter on world region
 *
 * @author Klaas Eikelboom (CiviCooP)
 * @date august 2018
 * @license AGPL-3.0
 */

class ilga_scholarship_handler_filter_streams extends views_handler_filter_in_operator{
    function construct() {
        parent::construct();
        if (!civicrm_initialize()) {
            return;
        }
    }

    function operators(){
        $operators = parent::operators();
        unset($operators['in']);
        unset($operators['not in']);
        return $operators;
    }

    function op_empty()
    {
        if ($this->operator == 'empty') {
            return;
        }
        $this->ensure_my_table();

       $streamId = civicrm_api3('Contact','getvalue',[
         'id' => ilga_scholarship_helper::currentUser(),
         'return' => 'custom_'.variable_get(ILGA_SCHOLARSHIP_STREAM_CUSTOM_FIELD,0),
       ]);

       if($streamId) {

         $streamList = implode(',',$streamId);

         $clause = "webform_submissions.sid in 
                  (select sid from {webform_submitted_data} wsd 
                   where wsd.cid=150 and wsd.nid=12 and wsd.data in ($streamList)
                  )";

         $this->query->add_where_expression($this->options['group'], $clause);
       }

    }

}