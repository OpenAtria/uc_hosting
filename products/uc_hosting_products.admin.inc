<?php
/**
 * @file
 * Admin settings for the uc_hosting_products module
 */

/**
 * Settings form for the admin section
 */
function uc_hosting_products_admin () {
  $form = array();

  $form['site_feature_settings'] = array(
    '#type' => 'fieldset',
    '#tree' => FALSE,
    '#title' => t('Site feature settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['site_feature_settings']['uc_hosting_products_force_site_creation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force clients to create their site on purchase'),
    '#default_value' => variable_get('uc_hosting_products_force_site_creation', 0),
    '#description' => t('This will remove the "Skip this step" button on the site creation form shown to clients before checkout.'),
    '#required' => FALSE,
  );

  return system_settings_form($form);
}
