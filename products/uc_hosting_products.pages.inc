<?php
// $Id$

/**
 * Access callback for the site details form
 */
function uc_hosting_products_site_info_access () {
  global $user;

  // Only return true if the user has a cart item of type site
  $site_model = db_result(db_query('SELECT model FROM {uc_hosting_products} WHERE type = "sites"'));
  $cart = uc_cart_get_contents();
  foreach ($cart as $key => $item) {
    if ($item->model == $site_model) {
      return TRUE;
    }
  }

  // Or if they're user 1
  if ($user->uid == 1) {
    return TRUE;
  }
}

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

  // Change some of this form's elements
  unset($site_form['client']);
  $site_form['db_server']['#type'] = 'value';
  $site_form['db_server']['#value'] = $site_form['db_server']['#default_value'];

  $site_form['#redirect'] = 'cart';
  $site_form['#submit'][] = 'uc_hosting_products_site_info_process';

  // Add a submit button to this form
  $site_form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $site_form;
}

/**
 * Submit function for the create_site_form
 */
function uc_hosting_products_site_info_process ($form, $form_state) {

  $site_model = db_result(db_query('SELECT model FROM {uc_hosting_products} WHERE type = "sites"'));

  // Get the current cart contents and add the submitted info to site items
  $cart = uc_cart_get_contents();
  foreach ($cart as $key => $item) { 
    if ($item->model == $site_model) {
      $item->data['site_data'] = $form_state['values'];
      $item->module = 'uc_hosting_products';
      $item->data['module'] = 'uc_hosting_products';
      uc_cart_update_item($item);
    }
  }
}
