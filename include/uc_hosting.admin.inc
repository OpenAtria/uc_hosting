<?php

/**
 * @file
 * Admin interfaace for uc_hosting
 */

/**
 * Default callback for administration page
 */
function uc_hosting_admin () {
  return uc_hosting_admin_list_products();
}

/**
 * Display a list of products with & without the uc_hosting feature
 */
function uc_hosting_admin_list_products () {

  $output = '';

  $result = pager_query('SELECT n.title, upf.nid, uhp.model, uhp.pfid FROM {uc_product_features} upf
    INNER JOIN {uc_hosting_products} uhp ON uhp.pfid = upf.pfid
    INNER JOIN {node} n ON n.nid = upf.nid
    ORDER BY n.title ASC', 10);

  $output .= _uc_hosting_admin_product_table($result, 'remove');

  $result = pager_query('SELECT n.title, p.nid, p.model FROM {uc_products} p
    INNER JOIN {node} n ON n.nid = p.nid
    LEFT OUTER JOIN {uc_product_features} upf ON upf.nid = n.nid
    LEFT OUTER JOIN {uc_hosting_products} uhp ON uhp.pfid = upf.pfid
    WHERE uhp.pfid IS NULL ORDER BY n.title ASC', 10);

  $output .= _uc_hosting_admin_product_table($result, 'add');

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
function _uc_hosting_admin_product_table($result, $op = 'remove') {
  // Initialise variables to be passed to theme_table
  $rows = array();
  switch ($op) {
    case 'add':
      $header = array(
        t('Other Products'),
        t('Model'),
        t('Links'),
      );
      break;
    case 'remove':
      $header = array(
        t('Hosting Products'),
        t('Model'),
        t('Links'),
      );
      break;
    default:
      break;
  }

  while ($row = db_fetch_object($result)) {
    $link_items = array();
    switch ($op) {
      case 'remove':
        $link_items[] = l(t('remove'), "node/{$row->nid}/edit/features/hosting/{$row->pfid}/delete", array('query' => drupal_get_destination()));
        break;
      case 'add':
        $link_items[] = l(t('add'), "node/{$row->nid}/edit/features/hosting/add", array('query' => drupal_get_destination()));
        break;
      default:
        return t('There was an error generating product information.');
    }
    $link_items[] = l(t('edit'), "node/{$row->nid}/edit", array('query' => drupal_get_destination()));
    $rows[] = array(
      $row->title,
      $row->model,
      theme('item_list', $link_items),
    );
  }

  return theme('table', $header, $rows);
}
