<?php
// $Id$

/**
 * STEP ONE - Set up the product class
 *************************************/

// Set up values for the shared hosting product class
$form_state['values']  = array (
  'pcid' => 'uc_hosting_shared',
  'name' => t('Shared Aegir Hosting'),
  'description' => t('Shared hosting on this Aegir server. Options assume a one year subscription.'),
  'op' => 'Submit',
);

// Create the shared hosting product class
include_once (drupal_get_path('module', 'uc_product') . '/uc_product.admin.inc');
$product_class = uc_product_class_load($form_state['values']['pcid']);
if (!$product_class->pcid) {
  drupal_execute('uc_product_class_form', $form_state);
}

// Load the class object up for later
$product_class = uc_product_class_load($form_state['values']['pcid']);

$form_state = array();


/**
 * STEP TWO - Generate attributes and assign them options
 * This assumes we have already generated the quota attributes
 ******************************************************************/
include_once (drupal_get_path('module', 'uc_attribute') . '/uc_attribute.admin.inc');

// Fetch all resources so we can use the info to find the attributes we will associate to this class and create options.
$all_resources = module_invoke_all('hosting_quota_resource');

// We'll be reusing this later on for assigning attributes to options
$resource_names = array_keys($all_resources);


if (in_array('sites', $resource_names)) {

  // Determine the attribute id for number of sites
  $attr_id = db_result(db_query('SELECT q.aid FROM {uc_hosting_quotas} q WHERE q.quota = "%s"', 'sites'));

  $attr_values = array();

  // Grab the attribute for later
  $attribute = uc_attribute_load($attr_id);

  // Assign options to number of sites attribute - One site
  $opt_values  = array (
    'aid' => $attr_id,
    'name' => 'One site',
    'hosting_quota_value' => 1,
    'ordering' => '-10',
    'cost' => '0.00',
    'price' => '0.00',
    'weight' => '',
  );
  if (!db_result(db_query('SELECT o.oid FROM {uc_attribute_options} o WHERE o.name = "%s" AND o.aid = %d', $opt_values['name'], $attr_id))) {
    // Create the attribute
    db_query("INSERT INTO {uc_attribute_options} (aid, name, cost, price, weight, ordering) VALUES (%d, '%s', %f, %f, %f, %d)", $opt_values['aid'], $opt_values['name'], $opt_values['cost'], $opt_values['price'], $opt_values['weight'], $opt_values['ordering']);
    // Assign the appropriate quota_value
    db_query('INSERT INTO {uc_hosting_quota_options} (oid, value) VALUES ((SELECT MAX(oid) FROM {uc_attribute_options}), %d)', $opt_values['hosting_quota_value']);
  }
  $opt_values = array();

  // Three sites
  $opt_values  = array (
    'aid' => $attr_id,
    'name' => 'Three sites',
    'hosting_quota_value' => 3,
    'ordering' => '-8',
    'cost' => '111.00',
    'price' => '120.00',
  );
  if (!db_result(db_query('SELECT o.oid FROM {uc_attribute_options} o WHERE o.name = "%s" AND o.aid = %d', $opt_values['name'], $attr_id))) {
    db_query("INSERT INTO {uc_attribute_options} (aid, name, cost, price, weight, ordering) VALUES (%d, '%s', %f, %f, %f, %d)", $opt_values['aid'], $opt_values['name'], $opt_values['cost'], $opt_values['price'], $opt_values['weight'], $opt_values['ordering']);
    // Assign the appropriate quota_value
    db_query('INSERT INTO {uc_hosting_quota_options} (oid, value) VALUES ((SELECT MAX(oid) FROM {uc_attribute_options}), %d)', $opt_values['hosting_quota_value']);
  }
  $opt_values = array();

  // Five sites
  $opt_values = array (
    'aid' => $attr_id,
    'name' => 'Five sites',
    'hosting_quota_value' => 5,
    'ordering' => '-6',
    'cost' => '221.00',
    'price' => '270.00',
    'weight' => '',
  );
  if (!db_result(db_query('SELECT o.oid FROM {uc_attribute_options} o WHERE o.name = "%s" AND o.aid = %d', $opt_values['name'], $attr_id))) {
    db_query("INSERT INTO {uc_attribute_options} (aid, name, cost, price, weight, ordering) VALUES (%d, '%s', %f, %f, %f, %d)", $opt_values['aid'], $opt_values['name'], $opt_values['cost'], $opt_values['price'], $opt_values['weight'], $opt_values['ordering']);
    // Assign the appropriate quota_value
    db_query('INSERT INTO {uc_hosting_quota_options} (oid, value) VALUES ((SELECT MAX(oid) FROM {uc_attribute_options}), %d)', $opt_values['hosting_quota_value']);
  }
  $opt_values = array();

  // A quick adjustment because we would like our site options to appear as radio buttons
  db_query('UPDATE uc_attributes SET display = 2 WHERE aid = %d', $attr_id);
}

/**
 * STEP THREE - Assign all quota attributes to our new product class
 **************************************************************/

// This array will be passed on to the attribute form
$all_attr_ids = array();

foreach ($resource_names as $quota_resource) {
  $attr_id = db_result(db_query('SELECT q.aid FROM {uc_hosting_quotas} q WHERE q.quota = "%s"', $quota_resource));
  $all_attr_ids[$attr_id] = $attr_id;
}

$form_state['values']  = array (
  'add_attributes' => $all_attr_ids,
  'op' => 'Add attributes',
  'add' => 'Add attributes',
  'id' => $product_class->pcid,
  'type' => 'class',
  'view' => 'add',
);
drupal_execute('uc_object_attributes_form', $form_state, $product_class, 'class', 'add');
$form_state = array();


