This document assumes you want to implement a product feature using uc_hosting's database tables and exiting infrastructure. For a more complete example of an Ubercart 2.x product feature implementation, check out uc_file in the Ubercart core.

The Basics
----------

Any module implementing a uc_hosting feature should include both uc_hosting and uc_hosting_products as dependencies. Include these lines in your .info file:

<code>
dependencies[] = uc_hosting
dependencies[] = uc_hosting_products
</code>

Declaring product features to Ubercart 2.x
------------------------------------------

The first step is to implement Ubercart's hook_product_feature. Here is an example implementation for the platform access feature in uc_hosting_products:

<code>
/**
 * Implementation of hook_product_feature().
 */
function uc_hosting_products_product_feature () {
  $features = array();

  // Set the feature for the platforms
  $features[] = array(
    'id' => 'hosting_platform',
    'title' => t('Access to a platform'),
    'callback' => 'uc_hosting_products_platform_form',
    'delete' => 'uc_hosting_products_feature_delete',
    'settings' => 'uc_hosting_products_platform_settings',
  );

  return $features;
}
</code>

An implementation of hook_product_feature returns a numerical array of associative arrays. Feature arrays must include five keys:

* __id__ will be used to identify your feature at the database level, in the uc_product_features table, and also in urls. Think of it as the type of your feature.
* __title__ is the name of your feature as it will be displayed to site admins.
* __callback__ will be used to create and modify instances of your feature attached to specific products. This callback should return a drupal form passed through the uc_product_feature_form function.
* __delete__ is a simple function called on the removal of a feature instance from a product.
* __settings__ is a callback that provides additional settings for the feature, and is not used presently in any uc_hosting product feature.

Of note is that, unlike Drupal's core hook_menu and hook_theme, hook_product_feature does not include a 'file' key. uc_hosting works around this by using include_once at the beginning of the hook_product_feature implementation.

If you wish to use uc_hosting's shared feature functions, make sure to include the file "products/inc/uc_hosting_products.feature_shared.inc" (path relative to the uc_hosting root) at the beginning of your implementation of hook_product_feature.

The callback form
-----------------

This form is called everytime someone adds a product feature to a product. It allows you to set any options for your feature that should be delivered on a per-product basis. Lets take a look at this function for the platform access feature:

<code>
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
</code>

___uc_hosting_products_feature_fetch_product__ attempts to retrieve existing values for this instance of a product feature from the database.

<code>
  _uc_hosting_products_feature_shared_elements(&$form, $node, $feature, $hosting_product);
</code>

___uc_hosting_products_feature_shared_elements__ declares some form elements that either uc_hosting or Ubercart itself depend on to provide the product feature functionality.

<code>
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
</code>

This code block generates a list of platforms to allow the admin to select which platform to bind to his product. This code is specific to the platform access implementation - this is where you should include your own form elements relevant to your feature.

<code>
  $form['#validate'][] = 'uc_hosting_products_single_feature_validate';
</code>

For many implementations, __uc_hosting_products_single_feature_validate__ ensures that a given uc_hosting product feature can only be enabled once on any given product. All of the exising uc_hosting_products features make use of this function.

<code>
  return uc_product_feature_form ($form);
}
</code>

Finally, __uc_product_feature_form__ adds some required form elements from Ubercart.

You also need to make sure to code a submit function for your callback, of the form callback_submit:

<code>
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
</code>

We manually save the product_feature to ubercart's database tables here. This is so that we can then retrieve the index of the entry in the uc_product_features table for later use in our own data.

<code>
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
</code>

Note that the uc_hosting_products table has both an integer column __value__, and a longtext column __data__ for storing serialized information about the feature.

Taking action in Aegir based on product features
------------------------------------------------

uc_hosting assumes that most purchases made via Ubercart are intended to be expressed as changes to an Aegir client node. So it provides some code to help you create new clients or modify exiting ones when an order containing your feature is made. Any other actions needed to make your feature work you will need to implement yourself. This will involve implementing the Ubercart hook, [hook_order](http://api.ubercart.org/api/function/hook_order/2) and your own callback function.

Lets start by taking a look at uc_hosting_products own implementation of hook_order:

<code>
/**
 * Implementation of hook_order
 *
 * Actions t$form_state['values']['create_later']o take on order changes involving an aegir product
 *
 * @param $op string
 *   Provided by ubercart on invocation
 * @param &$arg1
 *   Different data depending on the op
 * @param $arg2
 *   Different data depending on the op
 */
function uc_hosting_products_order ($op, &$arg1, $arg2) {
  switch ($op) {
    case 'update':
      if ($arg2 == 'completed') {
        foreach ($arg1->products as $product) {
          if (_uc_hosting_products_has_feature ($product)) {
            // Make the changes necessary to the hosting client
            $client = uc_hosting_update_client($arg1, $product, 'uc_hosting_products_client_update');
          }
        }
      }
      break;
    default:
      break;
  }
}
</code>

___uc_hosting_products_has_feature__ is defined in uc_hosting_products.module, and provides some simple logic to detect uc_hosting product features. Rather than use this function, you will most likely want to define your own conditions.

__uc_hosting_update_client__ does the work of matching up an incoming ubercart order to an existing Aegir client, if possible. If not possible, it creates a new client node. It then calls the function provided as a third argument, in this case uc_hosting_products_client_update, and passes to that function the affected client node, as well as the product and order objects from Ubercart.

This callback function is the place to take actions in Aegir or pass commands. Take a look at uc_hosting_products for a fully fleshed out example.
