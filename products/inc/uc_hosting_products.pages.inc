<?php
/**
 * Callback for purchase/site
 */
function uc_hosting_products_site_info_form ($form_state) {

  // Access check - shunt the user back to cart if they don't have access
  // At the same time, try to retrieve the site product info
  $site_product = uc_hosting_products_get_site_product();
  if (empty($site_product)) {
    drupal_goto('cart');
  }

  // Retrieve the site form
  $site_form = drupal_retrieve_form('hosting_site_form', $form_state);
  $state = array();
  foreach (module_implements('form_alter') as $module) {
    $function = $module .'_form_alter';
    $function($form['site'], $state, 'site_node_form');
  }

  // We'll determine the client programatically later based on checkout choice
  unset($site_form['client']);

  $site_form['db_server']['#type'] = 'value';
  $site_form['db_server']['#value'] = $site_form['db_server']['#default_value'];

  $site_form['#redirect'] = 'cart';
  $site_form['#submit'][] = 'uc_hosting_products_site_info_process';

  // If there are default values set for profiles or platforms then apply them
  if ($site_product['data']['profile']) {
    $site_form['profile'] = array(
      '#type' => 'value',
      '#value' => $site_product['data']['profile'],
    );
  }
  if ($site_product['data']['platform']) {
    $site_form['platform'] = array(
      '#type' => 'value',
      '#value' => $site_product['data']['platform'],
    );
  }

  // Add a submit button to this form
  $site_form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  // Conditionally add the skip this site button
  if (!$site_product['value']) {
    // Allow people to skip this step
    $skip_button = array(
      '#name' => 'skip-this-step',
      '#id' => 'skip-this-step',
      '#value' => t('Skip this step'),
      '#attributes' => array('class' => 'form-submit')
    );

    $site_form['skip'] = array(
      '#type' => 'markup',
      '#value' => l(theme('button', $skip_button), $site_form['#redirect'], array('html' => TRUE)),
    );
  }

  return $site_form;
}

/**
 * Submit function for the create_site_form
 */
function uc_hosting_products_site_info_process ($form, &$form_state) {

  $site_model = db_result(db_query('SELECT model FROM {uc_hosting_products} WHERE type = "sites"'));

  // Get the current cart contents and add the submitted info to site items
  $cart = uc_cart_get_contents();
  foreach ($cart as $key => $item) { 
    if ($item->model == $site_model) {
      $item->data['site_data'] = $form_state['values'];
      uc_cart_update_item($item);
    }
  }
}

/**
 * Try to retrieve the relevant site product
 */
function uc_hosting_products_get_site_product () {
  // Retrieve the site models
  $site_model_result = db_query('SELECT * FROM {uc_hosting_products} WHERE type = "sites"');
  $site_models = array();
  while($row = db_fetch_array($site_model_result)) {
    $site_models[$row['model']] = $row;
    $site_models[$row['model']]['data'] = unserialize($row['data']);
  }

  $cart = uc_cart_get_contents();
  foreach ($cart as $key => $item) {
    if (in_array($item->model, array_keys($site_models))) {
      return $site_models[$item->model];
    }
  }

  return FALSE;
}
