<?php
// $Id$

/**
 * Function to create an attribute for a designated quota
 *
 * @sa hosting_quota_get
 *
 * @param $rname
 *   The machine name of the resource
 * @param $resource
 *   An array representing a quota resource
 * @param $ordering
 *   A numerical value affecting the display order of attributes
 */
function uc_hosting_quota_create($rname, $resource = NULL, $ordering = 0) {
  if (!is_array($resource)) {
    $resource = hosting_quota_get($rname);
  }

  // This is a one table operation so lets just insert the data directly into the table
  if (!$attr_id = db_result(db_query('SELECT a.aid FROM {uc_attributes} a INNER JOIN {uc_hosting_quotas} u ON a.aid = u.aid WHERE u.quota = "%s"', $rname))) {
    db_query("INSERT INTO {uc_attributes} (name, label, ordering, required, display, description)
      VALUES ('%s', '%s', %d, %d, %d, '%s')",
      $rname,
      $resource['title'],
      $ordering,
      1,
      1,
      $resource['description']);

    // Determine the attribute id for the resource
    $attr_id = db_result(db_query('SELECT a.aid FROM {uc_attributes} a WHERE a.name = "%s"', $rname));

    if (db_query('INSERT INTO {uc_hosting_quotas} (aid, quota) VALUES (%d, "%s")', $attr_id, $rname)) {
      drupal_set_message (t('Attribute created for @quota', array('@quota' => $resource['title'])));
    } else {
      drupal_set_message (t('Error creating attribute for @quota', array('@quota' => $resource['title'])), 'error');
      watchdog ('uc_hosting', 'Error creating attribute for @quota: @error', array('@quota' => $resource['title'], '@error' => db_error()), WATCHDOG_ERROR);
    }
  }
  else {
    watchdog('uc_hosting', 'Attribute creation duplicate call.');
  }
  drupal_goto();
}

/**
 * Function to recurse through available quota resources
 * and create associated attributes
 */
function uc_hosting_quotas_create_attributes () {

  $all_resources = module_invoke_all('hosting_quota_resource');

  $ordering = -10;
  foreach ($all_resources as $rname => $resource) {
    uc_hosting_quota_create($rname, $resource, $ordering);
    $ordering++;
  }

  return '';
}

/**
 * Alter the option form to include a field for setting the numerical quota
 */
function uc_hosting_form_uc_attribute_option_form_alter (&$form) {

  $platform_aid = variable_get('uc_hosting_platforms_attribute', db_result(db_query('SELECT a.aid FROM {uc_attributes} a WHERE a.name = "%s"', 'platforms')));

  switch ($form['aid']['#value']) {

  // For options belonging to the platform attribute
  case $platform_aid:
    $platform_options = _hosting_get_enabled_platforms();
 
    $form['uc_hosting_platform'] = array(
      '#type' => 'select',
      '#title' => t('Aegir Platform'), 
      '#options' => $platform_options, 
      '#description' => t('Choose a platform to be included in this option.')
    );

    $form['#submit'][] = 'uc_hosting_save_platform_option';
    break;

  default:
    $attribute = uc_attribute_load ($form['aid']['#value']);

    if ($resource = hosting_quota_get($attribute->name)) {
      $form['hosting_quota_value'] = array(
        '#type' => 'textfield',
        '#title' => 'Value to assign to the quota',
        '#required' => TRUE,
        '#size' => 60,
        '#description' => 'Integer: ' . $attribute->description,
      );

      if ($limit = db_result(db_query('SELECT value FROM {uc_hosting_quota_options} WHERE oid = %d', $form['oid']['#value']))) {
        $form['hosting_quota_value']['#default_value'] = $limit;
      }
      else {
        $form['hosting_quota_value']['#default_value'] = variable_get("hosting_quota_default_{$attribute->name}", 0);
      }

      $form['#validate'][] = 'uc_hosting_quota_check_limit';
      $form['#submit'][] = 'uc_hosting_quota_save_limit';
    }
    break;
  }
}

/**
 * Alter the option deletion form to call our clean-up function
 */
function uc_hosting_form_uc_attribute_option_delete_confirm_alter (&$form) {
  $form['#submit'][] = 'uc_hosting_quota_delete_limit';
  // drupal_set_message('<pre>' . var_export($form, true) . '</pre>');
}

/**
 * Alter the attribute deletion to call a clean-up function
 */
function uc_hosting_form_uc_attribute_delete_confirm_alter (&$form) {
  $form['#submit'][] = 'uc_hosting_quota_delete_attribute';
}

/**
 * Validate that the submitted value is in fact an integer
 */
function uc_hosting_quota_check_limit ($form, &$form_state) {
  $form_state['values']['hosting_quota_value'] = (int) $form_state['values']['hosting_quota_value'];
  if (!is_int($form_state['values']['hosting_quota_value'])) {
    form_set_error('hosting_quota_value', 'You must input an integer as quota value.');
  }
}

/**
 * Save a platform option
 */
function uc_hosting_save_platform_option ($form, &$form_state) {

  // There is already a quota value for this option
  if ($oid = db_result(db_query('SELECT oid FROM {uc_hosting_platform_options} WHERE oid = %d', $form['oid']['#value']))) {
    db_query('UPDATE {uc_hosting_platform_options} SET platform = %d WHERE oid = %d', $oid, $form_state['values']['uc_hosting_platform']);
  }

  // We are modifying an existing option without a quota value
  else if ($form_state['values']['oid']) {
    db_query('INSERT INTO {uc_hosting_platform_options} (oid, platform) VALUES ("%d", "%d")', $form_state['values']['oid'], $form_state['values']['uc_hosting_platform']);
  }

  // We are creating a new option
  else {
    $oid = db_result(db_query('SELECT MAX(oid) FROM {uc_attribute_options}'));
    db_query('INSERT INTO {uc_hosting_platform_options} (oid, platform) VALUES ("%d", "%d")', $oid, $form_state['values']['uc_hosting_platform']);
 }
}

/**
 * Save the new limit
 */
function uc_hosting_quota_save_limit ($form, &$form_state) {
  // There is already a quota value for this option
  if ($oid = db_result(db_query('SELECT oid FROM {uc_hosting_quota_options} WHERE oid = %d', $form['oid']['#value']))) {
    db_query('UPDATE {uc_hosting_quota_options} SET value = %d WHERE oid = %d', $oid, $form_state['values']['hosting_quota_value']);
  }

  // We are modifying an existing option without a quota value
  else if ($form_state['values']['oid']) {
    db_query('INSERT INTO {uc_hosting_quota_options} (oid, value) VALUES ("%d", "%d")', $form_state['values']['oid'], $form_state['values']['hosting_quota_value']);
  }

  // We are creating a new option
  else {
    $oid = db_result(db_query('SELECT MAX(oid) FROM {uc_attribute_options}'));
    db_query('INSERT INTO {uc_hosting_quota_options} (oid, value) VALUES ("%d", "%d")', $oid, $form_state['values']['hosting_quota_value']);
 }
}

/**
 * Delete a limit on option delete
 */
function uc_hosting_quota_delete_limit ($form, &$form_state) {
  db_query('DELETE FROM {uc_hosting_quota_options} WHERE oid = %d', $form_state['values']['oid']);
}

/**
 * Delete an attribute and its associated options from the uc_hosting tables
 */
function uc_hosting_quota_delete_attribute ($form, &$form_state) {
  db_query('DELETE FROM {uc_hosting_quota_options} WHERE aid = %d', $form_state['values']['aid']);
  db_query('DELETE FROM {uc_hosting_quotas} WHERE aid = %d',  $form_state['values']['aid']);
}
