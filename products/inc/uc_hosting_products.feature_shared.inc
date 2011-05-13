<?php
/**
 * Helper function to generate shared form elements in all features
 */
function _uc_hosting_products_feature_shared_elements($feature) {
  $elements = array();

  $elements['feature_id'] = array(
    '#type' => 'hidden',
    '#value' => $feature['id'],
  );

  return $elements;
}

/**
 * Validation function to enforce single feature policy
 */
function uc_hosting_products_single_feature_validate($form, &$form_state) {
  // For this to work, we can have only one hosting feature per product
  $check = db_result(db_query("SELECT pf.pfid FROM {uc_product_features} pf LEFT JOIN {uc_hosting_products} hp ON pf.pfid = hp.pfid WHERE pf.fid LIKE 'hosting_%' AND pf.nid = %d", $form_state['values']['nid']));
  if ($check && arg(5) == 'add') {
    form_set_error('model', 'Sorry, only one hosting feature per product.');
  }
}

/**
 * Delete function for all features in this module (platforms and quotas)
 */
function uc_hosting_products_feature_delete($feature) {
  db_query("DELETE FROM {uc_hosting_products} WHERE pfid = %d", $feature['pfid']);
} 
