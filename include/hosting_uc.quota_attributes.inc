<?php
// $Id$

/**
 * Function to recurse through available quota resources
 * and create associated attributes
 */
function hosting_uc_quotas_create_attributes () {

  $all_resources = module_invoke_all('hosting_quota_resource');

  $ordering = -10;
  foreach ($all_resources as $rname => $resource) {

    // Determine the attribute id for the resource
    $attr_id = db_result(db_query('SELECT a.aid FROM {uc_attributes} a WHERE a.name = "%s"', $rname));

    // This is a one table operation so lets just insert the data directly into the table
    if (!$attr_id) {
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

      db_query('INSERT INTO {hosting_uc_quotas} (aid, quota) VALUES (%d, "%s")', $attr_id, $rname);
    }

    // Increment the ordering
    $ordering++;
  }

  return 'hiya';
}

/**
 * Alter the option form to include a field for setting the numerical quota
 */
function hosting_uc_form_uc_attribute_option_form_alter (&$form) {

  $attribute = uc_attribute_load ($form['aid']['#value']);

  if ($resource = hosting_quota_get($attribute->name)) {
    $form['hosting_quota_value'] = array(
      '#type' => 'textfield',
      '#title' => 'Value to assign to the quota',
      '#required' => TRUE,
      '#size' => 60,
      '#description' => 'Integer: ' . $attribute->description,
    );

    if ($limit = db_result(db_query('SELECT value FROM {hosting_uc_quota_options} WHERE oid = %d', $form['oid']['#value']))) {
      $form['hosting_quota_value']['#default_value'] = $limit;
    }
    else {
      $form['hosting_quota_value']['#default_value'] = variable_get("hosting_quota_default_{$attribute->name}", 0);
    }

    $form['#validate'][] = 'hosting_uc_quota_check_limit';
    $form['#submit'][] = 'hosting_uc_quota_save_limit';
  }
}

/**
 * Validate that the submitted value is in fact an integer
 */
function hosting_uc_quota_check_limit ($form, &$form_state) {
  $form_state['values']['hosting_quota_value'] = (int) $form_state['values']['hosting_quota_value'];
  if (!is_int($form_state['values']['hosting_quota_value'])) {
    form_set_error('hosting_quota_value', 'You must input an integer as quota value.');
  }
}

/**
 * Save the new limit
 */
function hosting_uc_quota_save_limit ($form, &$form_state) {
  if ($oid = db_result(db_query('SELECT oid FROM {hosting_uc_quota_options} WHERE oid = %d', $form['oid']['#value']))) {
    db_query('UPDATE {hosting_uc_quota_options} SET value = %d WHERE oid = %d', $oid, $form_state['values']['hosting_quota_value']);
  }
  else {
    $oid = db_result(db_query('SELECT MAX(oid) FROM {uc_attribute_options}'));
    db_query('INSERT INTO {hosting_uc_quota_options} (oid, value) VALUES ("%d", "%d")', $oid , $form_state['values']['hosting_quota_value']);
  }
}

