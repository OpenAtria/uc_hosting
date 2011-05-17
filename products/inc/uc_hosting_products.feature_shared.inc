<?php
/**
 * Helper function to generate shared form elements in all features
 */
function _uc_hosting_products_feature_shared_elements(&$form, $node, $feature, $hosting_product) {

  $form['feature_id'] = array(
    '#type' => 'hidden',
    '#value' => $feature['id'],
  );

  // Grab all the models on this product, get rid of the top -Any- option 
  $models = uc_product_get_models($node); 
  array_shift($models); 
 
  $form['model'] = array( 
    '#type' => 'select', 
    '#title' => t('SKU'), 
    '#default_value' => $hosting_product->model, 
    '#description' => t('This is the SKU that will need to be purchased to associate the purchase with a site.'), 
    '#options' => $models, 
  );

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  $form['pfid'] = array(
    '#type' => 'value',
    '#value' => $feature['pfid'],
  );
}

/**
 * Helper function to get an associated hosting_product from a feature
 */
function _uc_hosting_products_feature_fetch_product($feature) {
  if (!empty($feature)) {
    $hosting_product = db_fetch_object(db_query("SELECT * FROM {uc_hosting_products} WHERE pfid = %d", $feature['pfid']));
    $hosting_product->data = unserialize($hosting_product->data);
  }
  else {
    $hosting_product = new stdClass();
  }
  return $hosting_product;
}

/**
 * Validation function to enforce single feature policy
 */
function uc_hosting_products_single_feature_validate($form, &$form_state) {
  // For this to work, we can have only one hosting feature of each type per product
  $check = db_result(db_query("SELECT pf.pfid FROM {uc_product_features} pf LEFT JOIN {uc_hosting_products} hp ON pf.pfid = hp.pfid WHERE pf.fid = '%s' AND pf.nid = %d", $form['feature_id']['#value'], $form_state['values']['nid']));
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
