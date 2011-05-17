<?php
/**
 * @file
 * Included by the uc_hosting_products module to provide callbacks for the platform access feature
 */

/**
 * Callback to add the site form on the product feature page
 */
function uc_hosting_products_site_form ($form_state, $node, $feature) {
  $form = array();

  // Get default values and shared form elements for all uc_hosting features
  $hosting_product = _uc_hosting_products_feature_fetch_product($feature);
  if (empty($feature)) {
     $feature = array('id' => 'hosting_site');
  }
  _uc_hosting_products_feature_shared_elements(&$form, $node, $feature, $hosting_product);

  $form['force_site_creation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force clients to create their site on purchase'),
    '#default_value' => $hosting_product->value,
    '#description' => t('This will remove the "Skip this step" button on the site creation form shown to clients before checkout.'),
    '#required' => FALSE,
  );

  // @see hosting_quota_get
  if (user_access('edit all quotas')) {
    $quotas = module_invoke_all('hosting_quota_resource');
  }
  else {
    $quotas = array();
    drupal_set_message('You must be able to edit quotas to add this feature.', 'warning');
  }

  $form['site_defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site Defaults (Optional)'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );

  // @see _hosting_get_platforms()
  if (user_access('view locked platforms')) {
    $platforms = _hosting_get_platforms();
  }
  else if (user_access('view platform')) {
    $platforms = _hosting_get_enabled_platforms();
  }

  // Install profiles 
  $profiles = hosting_get_profiles();
  foreach($profiles as $id => $name) {
    // Don't allow a site to be provisioned with hostslave or hostmaster profile
    if (in_array($name, array('Hostslave', 'Hostmaster'))) {
      unset($profiles[$id]);
    }
  }

  $platforms[0] = t("Don't associate a platform");
  $profiles[0] = t("Don't associate a profile");

  $form['site_defaults']['profile'] = array(
    '#type' => 'select',
    '#title' => t('Profile'),
    '#description' => t('Install this site automatically with this profile.'),
    '#default_value' => $hosting_product->data['profile'] ? $hosting_product->data['profile'] : 0,
    '#options' => $profiles,
    '#required' => FALSE,
  );

  $form['site_defaults']['platform'] = array(
    '#type' => 'select',
    '#title' => t('Platform'),
    '#description' => t('Install this site automatically onto this platform.'),
    '#default_value' => $hosting_product->data['platform'] ? $hosting_product->data['platform'] : 0,
    '#options' => $platforms,
    '#required' => FALSE,
  );

  $form['quota'] = array(
    '#type' => 'value',
    '#value' => 'sites',
  );

  $form['#validate'][] = 'uc_hosting_products_single_feature_validate';
  $form['#validate'][] = 'uc_hosting_products_site_form_validate';

  return uc_product_feature_form ($form);
}

/**
 * Validate site feature settings
 */
function uc_hosting_products_site_form_validate($form, &$form_state) {
  if ($form_state['values']['site_defaults']['profile'] && $form_state['values']['site_defaults']['platform']) {
    $allowed_plats = hosting_get_profile_platforms($form_state['values']['site_defaults']['profile'], FALSE);
    if (!array_key_exists($form_state['values']['site_defaults']['platform'], $allowed_plats)) {
      form_set_error('platform', t('That profile is not available on this platform.'));
    }
  }
}

/**
 * Save the product site feature settings.
 */
function uc_hosting_products_site_form_submit($form, &$form_state) {
  $hosting_product = array(
    'pfid' => $form_state['values']['pfid'],
    'model' => $form_state['values']['model'],
    'type' => 'sites',
    'value' => $form_state['values']['force_site_creation'],
    'data' => $form_state['values']['site_defaults'],
  );

  $description = t('<strong>SKU:</strong> !sku<br />', array('!sku' => empty($hosting_product['model']) ? 'Any' : $hosting_product['model']));
  $description .= t('Allows the creation of a site on product purchase.');

  // data to be saved to the uc_product_feature table
  $data = array(
    'pfid' => $form_state['values']['pfid'],
    'nid' => $form_state['values']['nid'],
    'fid' => 'hosting_site',
    'description' => $description,
  );

  $form_state['redirect'] = uc_product_feature_save($data);

  // Insert or update uc_hosting_products table
  if (empty($hosting_product['pfid'])) {
    $hosting_product['pfid'] = db_last_insert_id('uc_product_features', 'pfid');
  }

  $existing_prod = db_fetch_object(db_query("SELECT hpid, data FROM {uc_hosting_products} WHERE pfid = %d LIMIT 1", $hosting_product['pfid']));

  $key = NULL;
  if ($existing_prod->hpid != 0) {
    $key = 'hpid';
    $hosting_product['hpid'] = $existing_prod->hpid;
  }

  $hosting_product['data'] = serialize($hosting_product['data']);

  drupal_write_record('uc_hosting_products', $hosting_product, $key);
}

/**
 * Settings form for the sites
 */
function uc_hosting_products_site_settings() {
  $form = array();
  return $form;
}

