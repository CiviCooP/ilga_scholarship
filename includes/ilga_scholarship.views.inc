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
      'evaluate_session' => [
        'title' => 'Evaluate Session',
        'help' => 'Generates link to the Session evaluation form',
        'field' => [
          'handler' => 'ilga_scholarship_evaluate_session_link',
          'click sortable' => FALSE,
        ],
      ],
      'evaluate_summary' => [
        'title' => 'Evaluation Summary',
        'help' => 'Shows a summary of all evaluations',
        'field' => [
          'handler' => 'ilga_scholarship_evaluate_summary',
          'click sortable' => FALSE,
        ],
      ],
      'evaluation_stream' => [
        'title' => t('Evaluation Stream'),
        'help' => t('Filter only the streams the evaluator is subscribed on'),
        'filter' => [
          'handler' => 'ilga_scholarship_handler_filter_streams',
        ]
      ],
      'evaluation_session_verdicts' => [
        'title' => t('Session Verdictssh '),
        'help' => t('Filter on the session verdicts'),
        'filter' => [
          'handler' => 'ilga_scholarship_handler_filter_sessionverdicts',
        ]
      ],
    ],
  ];
  return $data;
}