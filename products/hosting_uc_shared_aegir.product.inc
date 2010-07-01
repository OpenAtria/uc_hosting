<?php
// $Id$

/**
 * STEP ONE - Set up the product class
 *************************************/

// Set up values for the shared hosting product
$form_state['values']  = array (
  'pcid' => 'hosting_uc_shared',
  'name' => t('Shared Aegir Hosting'),
  'description' => t('One year of shared hosting on this Aegir server. Aegir greatly simplifies managing multiple site versions and 
applying Drupal security updates.'),
  'op' => 'Submit',
);

// Create the shared hosting product
include_once (drupal_get_path('module', 'uc_product') . '/uc_product.admin.inc');
$product_class = uc_product_class_load($form_state['values']['pcid']);
if (!$product_class->pcid) {
  drupal_execute('uc_product_class_form', $form_state);
}

// Load the class object up for later
$product_class = uc_product_class_load($form_state['values']['pcid']);

$form_state = array();


/**
 * STEP TWO - Assign attributes to the shared hosting product class
 ******************************************************************/
include_once (drupal_get_path('module', 'uc_attribute') . '/uc_attribute.admin.inc');

// We will use this value to store attribute ids for later use in associating them to our new product class
$all_attr_ids = array();

// First the number of sites and its options 
$attr_values  = array (
  'name' => 'Number of sites',
  'label' => 'Number of sites',
  'description' => '',
  'required' => 1,
  'display' => '2',
  'ordering' => '-10',
);

// Determine the attribute id for number of sites
$attr_id = db_result(db_query('SELECT a.aid FROM {uc_attributes} a WHERE a.name = "%s"', $attr_values['name']));

// This is a one table operation so lets just insert the data directly into the table
if (!$attr_id) {
  db_query("INSERT INTO {uc_attributes} (name, label, ordering, required, display, description) VALUES ('%s', '%s', %d, %d, %d, '%s')", $attr_values['name'], $attr_values['label'], $attr_values['ordering'], $attr_values['required'], $attr_values['display'], $attr_values['description']);
  $attr_id = db_result(db_query('SELECT a.aid FROM {uc_attributes} a WHERE a.name = "%s"', $attr_values['name']));
}
$attr_values = array();
$all_attr_ids[$attr_id] = $attr_id;

// Grab the attribute for later
$attribute = uc_attribute_load($attr_id);

// Assign options to number of sites attribute - One site
$opt_values  = array (
  'aid' => $attr_id,
  'name' => 'One site',
  'ordering' => '-10',
  'cost' => '0.00',
  'price' => '150.00',
  'weight' => '',
);
if (!db_result(db_query('SELECT o.oid FROM {uc_attribute_options} o WHERE o.name = "%s" AND o.aid = %d', $opt_values['name'], $attr_id))) {
  db_query("INSERT INTO {uc_attribute_options} (aid, name, cost, price, weight, ordering) VALUES (%d, '%s', %f, %f, %f, %d)", $opt_values['aid'], $opt_values['name'], $opt_values['cost'], $opt_values['price'], $opt_values['weight'], $opt_values['ordering']);
}
$opt_values = array();

// Three sites
$opt_values  = array (
  'aid' => $attr_id,
  'name' => 'Three sites',
  'ordering' => '-8',
  'cost' => '0.00',
  'price' => '250.00',
);
if (!db_result(db_query('SELECT o.oid FROM {uc_attribute_options} o WHERE o.name = "%s" AND o.aid = %d', $opt_values['name'], $attr_id))) {
  db_query("INSERT INTO {uc_attribute_options} (aid, name, cost, price, weight, ordering) VALUES (%d, '%s', %f, %f, %f, %d)", $opt_values['aid'], $opt_values['name'], $opt_values['cost'], $opt_values['price'], $opt_values['weight'], $opt_values['ordering']);
}
$opt_values = array();

// Five sites
$opt_values = array (
  'aid' => $attr_id,
  'name' => 'Five sites',
  'ordering' => '-6',
  'cost' => '0.00',
  'price' => '375.00',
  'weight' => '',
);
if (!db_result(db_query('SELECT o.oid FROM {uc_attribute_options} o WHERE o.name = "%s" AND o.aid = %d', $opt_values['name'], $attr_id))) {
  db_query("INSERT INTO {uc_attribute_options} (aid, name, cost, price, weight, ordering) VALUES (%d, '%s', %f, %f, %f, %d)", $opt_values['aid'], $opt_values['name'], $opt_values['cost'], $opt_values['price'], $opt_values['weight'], $opt_values['ordering']);
}
$opt_values = array();

// Bandwidth Quotas
$attr_values = array(
  'name' => 'Bandwidth Quota',
  'label' => 'Bandwidth Quota',
  'description' => '',
  'required' => 0,
  'display' => '1',
  'ordering' => '-5',
);

// Determine the attribute id for bandwidth
$attr_id = db_result(db_query('SELECT a.aid FROM {uc_attributes} a WHERE a.name = "%s"', $attr_values['name']));

// This is a one table operation so lets just insert the data directly into the table
if (!$attr_id) {
  db_query("INSERT INTO {uc_attributes} (name, label, ordering, required, display, description) VALUES ('%s', '%s', %d, %d, %d, '%s')", $attr_values['name'], $attr_values['label'], $attr_values['ordering'], $attr_values['required'], $attr_values['display'], $attr_values['description']);
  $attr_id = db_result(db_query('SELECT a.aid FROM {uc_attributes} a WHERE a.name = "%s"', $attr_values['name']));
}
$attr_values = array();
$all_attr_ids[$attr_id] = $attr_id;

// Grab the attribute for later
$attribute = uc_attribute_load($attr_id);

// Bandwidth Options: 15GB
$opt_values = array (
  'aid' => $attr_id,
  'name' => '15 GB',
  'ordering' => '-10',
  'cost' => '0.00',
  'price' => '0.00',
  'weight' => '',
);
if (!db_result(db_query('SELECT o.oid FROM {uc_attribute_options} o WHERE o.name = "%s" AND o.aid = %d', $opt_values['name'], $attr_id))) {
  db_query("INSERT INTO {uc_attribute_options} (aid, name, cost, price, weight, ordering) VALUES (%d, '%s', %f, %f, %f, %d)", $opt_values['aid'], $opt_values['name'], $opt_values['cost'], $opt_values['price'], $opt_values['weight'], $opt_values['ordering']);
}
$opt_values = array();

// Hard Disk Quotas
$attr_values = array(
  'name' => 'Disk Quota',
  'label' => 'Disk Space Quota',
  'description' => '',
  'required' => 0,
  'display' => '1',
  'ordering' => '0',
);

// Determine the attribute id for hard disk
$attr_id = db_result(db_query('SELECT a.aid FROM {uc_attributes} a WHERE a.name = "%s"', $attr_values['name']));

// This is a one table operation so lets just insert the data directly into the table
if (!$attr_id) {
  db_query("INSERT INTO {uc_attributes} (name, label, ordering, required, display, description) VALUES ('%s', '%s', %d, %d, %d, '%s')", $attr_values['name'], $attr_values['label'], $attr_values['ordering'], $attr_values['required'], $attr_values['display'], $attr_values['description']);
  $attr_id = db_result(db_query('SELECT a.aid FROM {uc_attributes} a WHERE a.name = "%s"', $attr_values['name']));
}
$attr_values = array();
$all_attr_ids[$attr_id] = $attr_id;

// Grab the attribute for later
$attribute = uc_attribute_load($attr_id);

// Hard Disk Options: 5GB
$opt_values = array (
  'aid' => $attr_id,
  'name' => '5 GB',
  'ordering' => '-10',
  'cost' => '0.00',
  'price' => '0.00',
  'weight' => '',
);
if (!db_result(db_query('SELECT o.oid FROM {uc_attribute_options} o WHERE o.name = "%s" AND o.aid = %d', $opt_values['name'], $attr_id))) {
  db_query("INSERT INTO {uc_attribute_options} (aid, name, cost, price, weight, ordering) VALUES (%d, '%s', %f, %f, %f, %d)", $opt_values['aid'], $opt_values['name'], $opt_values['cost'], $opt_values['price'], $opt_values['weight'], $opt_values['ordering']);
}
$opt_values = array();

/**
 * STEP THREE - Associate these attributes to the product class
 **************************************************************/

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

?>
