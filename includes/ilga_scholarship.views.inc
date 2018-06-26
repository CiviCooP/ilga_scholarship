<?php
/**
 * Configuration of an view element
 *
 * @author Klaas Eikelboom (CiviCooP)
 * @date 25 juni 2018
 * @license AGPL-3.0
 */

function ilga_scholarship_views_data() {
  $data = [
    'ilga_scholarship' => [
      'table' => [
        'group' => 'ILGA Scholarship',
        'join' => ['#global' => ''],
      ],
      'evaluate_application' => [
        'title' => 'Evaluate Application',
        'help' => 'Generates link to the Scholarship evaluation form',
        'field' => [
          'handler' => 'ilga_scholarship_evaluate_application_link',
          'click sortable' => FALSE,
        ],
      ],
    ],
  ];
  return $data;
}