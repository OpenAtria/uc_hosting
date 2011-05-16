<?php
/**
 * @file
 * Admin settings for the uc_hosting_products module
 */

/**
 * Page Callback for the admin section
 */
function uc_hosting_products_admin () {
  $output = '';

  // Display any errors
  if ($errors = variable_get('uc_hosting_products_error_state', FALSE)) {
    drupal_set_message ($errors, 'error');
  }

  // provide a button to clear the uc_hosting error state messages
  $clear_element = array(
    '#name' => 'uc-hosting-products-clear-error-state',
    '#id' => 'uc-hosting-products-clear-error-state',
    '#value' => t('Clear all Aegir Ubercart integration error messages.'),
    '#attributes' => NULL
  );

  $output .= l(theme('button', $clear_element), 'admin/store/hosting/clear-errors', array('html' => TRUE, 'query' => drupal_get_destination())) . '<p>&nbsp;</p>';

  $result = pager_query('SELECT n.title, u.nid, u.model FROM {uc_products} u
    INNER JOIN {node} n ON n.nid = u.nid
    WHERE u.nid IN (SELECT nid FROM {uc_product_features} WHERE fid LIKE "hosting_%")
    ORDER BY n.title ASC', 10);

  $output .= _uc_hosting_product_admin_table($result);

  return $output;
}

/**
 * Callback to clear error messages
 */
function uc_hosting_products_clear_errors () {
  variable_set('uc_hosting_products_error_state', FALSE);
  drupal_set_message(t('Error messages cleared!'));
  drupal_goto('admin/store/hosting');
}

/**
 * Helper function to generate product lists for uc_hostings admin page
 *
 * @param $result
 *   A mysql result resource
 * @param $op
 *   Whether we want to render the table with add or remove links
 */
function _uc_hosting_product_admin_table($result) {
  // Initialise variables to be passed to theme_table
  $rows = array();
  $header = array(
    t('Hosting Products'),
    t('Model'),
    t('Links'),
  );

  while ($row = db_fetch_object($result)) {
    $link = l(t('edit features'), "node/{$row->nid}/edit/features", array('attributes' => array('target' => '_BLANK')));
    $rows[] = array(
      $row->title,
      $row->model,
      $link
    );
  }

  return theme('table', $header, $rows);
}
