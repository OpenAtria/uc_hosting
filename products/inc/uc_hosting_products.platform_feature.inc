<?php
/**
 * @file
 * Included by the uc_hosting_products module to provide callbacks for the platform access feature
 */

/**
 * Callback to add the platform form on the product feature page
 */
function uc_hosting_products_platform_form ($form_state, $node, $feature) {
  $form = array();

  // Get default values and shared form elements for all uc_hosting features
  $hosting_product = _uc_hosting_products_feature_fetch_product($feature);
  if (empty($feature)) {
     $feature = array('id' => 'hosting_platform');
  }
  _uc_hosting_products_feature_shared_elements(&$form, $node, $feature, $hosting_product);

  // @see _hosting_get_platforms()
  if (user_access('view locked platforms')) {
    $platforms = _hosting_get_platforms();
  }
  else if (user_access('view platform')) {
    $platforms = _hosting_get_enabled_platforms();
  }
  else {
    $platforms = array();
    drupal_set_message('You must have permission to access platforms to enable this feature.', 'warning');
  }

  $form['platform'] = array(
    '#type' => 'select',
    '#title' => t('Platform'),
    '#description' => t('Select the platform to associate with this product.'),
    '#default_value' => $hosting_product->value,
    '#options' => $platforms,
    '#required' => TRUE,
  );

  $form['#validate'][] = 'uc_hosting_products_single_feature_validate';

  return uc_product_feature_form ($form);
}

/**
 * Save the platform feature settings.
 */
function uc_hosting_products_platform_form_submit($form, &$form_state) {
  $hosting_product = array(
    'pfid' => $form_state['values']['pfid'],
    'model' => $form_state['values']['model'],
    'type' => 'platform',
    'value' => $form_state['values']['platform'],
  );

  $platform_node = node_load($hosting_product['value']);
  $description = t('<strong>SKU:</strong> !sku<br />', array('!sku' => empty($hosting_product['model']) ? 'Any' : $hosting_product['model']));
  $description .= t('<strong>Platform:</strong> !platform', array('!platform' => $platform_node->title));

  $data = array(
    'pfid' => $form_state['values']['pfid'],
    'nid' => $form_state['values']['nid'],
    'fid' => 'hosting_platform',
    'description' => $description,
  );

  $form_state['redirect'] = uc_product_feature_save($data);

  // Insert or update uc_hosting_products table
  if (empty($hosting_product['pfid'])) {
    $hosting_product['pfid'] = db_last_insert_id('uc_product_features', 'pfid');
  }

  $existing_prod = db_fetch_object(db_query("SELECT hpid, data FROM {uc_hosting_products} WHERE pfid = %d LIMIT 1", $hosting_product['pfid']));

  $key = NULL;
  if ($existing_prod->hpid) {
    $key = 'hpid';
    $hosting_product['hpid'] = $existing_prod->hpid;
    $hosting_product['data'] = unserialize($existing_prod->data);
    $hosting_product['data']['platform'] = $hosting_product['value'];
  }
  // If necessary build a data array from scratch
  else {
    $hosting_product['data'] = array(
      'platform' => $hosting_product['value'],
    );
  }

  $hosting_product['data'] = serialize($hosting_product['data']);

  drupal_write_record('uc_hosting_products', $hosting_product, $key);
}

/**
 * Settings form for the platforms
 */
function uc_hosting_products_platform_settings() {
  $form = array();
  return $form;
}

