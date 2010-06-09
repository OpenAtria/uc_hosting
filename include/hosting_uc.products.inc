<?php
// $Id$

/**
 * Import and run a product definition
 *
 * @param $product string
 *   A string identifying which product to import
 */

function hosting_uc_products_add_product ($product = 'hosting_uc_shared_aegir') {

  $product_file = $product . '.product.inc';
  $product_dir = drupal_get_path('module', 'hosting_uc') . '/products';
  if (file_check_location("/{$product_dir}/{$product_file}", "/{$product_dir}")) {
    include_once("{$product_dir}/{$product_file}");
    return "Imported $product";
  }

  return FALSE;
}

?>
