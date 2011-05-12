<?php
/**
 * @file
 * Admin settings for the uc_hosting_products module
 */

/**
 * Settings form for the admin section
 */
function uc_hosting_products_admin () {
  $output = '';

  $result = pager_query('SELECT n.title, upf.nid, uhp.model, uhp.pfid FROM {uc_product_features} upf
    INNER JOIN {uc_hosting_products} uhp ON uhp.pfid = upf.pfid
    INNER JOIN {node} n ON n.nid = upf.nid
    ORDER BY n.title ASC', 10);

  $output .= _uc_hosting_product_admin_table($result);

  return $output;
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
    $link = l(t('edit features'), "node/{$row->nid}/edit/features", array('query' => drupal_get_destination()));
    $rows[] = array(
      $row->title,
      $row->model,
      $link
    );
  }

  return theme('table', $header, $rows);
}
