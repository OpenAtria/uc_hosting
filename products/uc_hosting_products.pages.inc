<?php
/**
 * Callback for purchase/site
 */
function uc_hosting_products_site_info_form ($form_state) {

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
  $site_form['#validate'][] = 'uc_hosting_products_site_info_skip';
  $site_form['#submit'][] = 'uc_hosting_products_site_info_process';

  // Add a submit button to this form
  $site_form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  // Allow people to skip this step
  $site_form['skip'] = array(
    '#type' => 'button',
    '#value' => t('Skip this step'),
  );

  return $site_form;
}

/**
 * Allow us to skip the site_info form
 */
function uc_hosting_products_site_info_skip ($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Skip this step')) {
    drupal_goto($form['#redirect']);
  }
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
