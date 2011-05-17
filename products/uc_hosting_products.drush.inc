<?php
/**
 * @file
 * Ensure that certain tasks generate approrpriate actions by uc_hosting
 */

/**
 * On a lock task of a platform used in a product, notify the user
 */
function uc_hosting_products_post_hosting_lock_task($task, $data) {
  if ($task->ref->type == 'platform') {
    if ($message = _uc_hosting_products_platform_check($task->ref))
      watchdog('uc_hosting', $message);
      drupal_set_message($message, 'error');
  }
}

/**
 * On a migrate task of a platform used in a product, notify the user
 */
function uc_hosting_products_post_hosting_migrate_task($task, $data) {
  if ($task->ref->type == 'platform') {
    if ($message = _uc_hosting_products_platform_check($task->ref))
      watchdog('uc_hosting', $message);
      drupal_set_message($message, 'error');
  }
}


/**
 * Helper function to check if a platform is used in a product
 *
 * @param $platform
 *   The node representation of the platform
 */
function _uc_hosting_products_platform_check($platform) {
  $affected_products = db_result(db_query('SELECT hpid, pfid FROM uc_hosting_products WHERE data LIKE "%%platform%%%d%%" LIMIT 1', $platform->nid));
  if ($affected_products) {
    $error_state = variable_get('uc_hosting_products_error_state', FALSE);
    $error_message = '<p>' . t('The platform !platform has been locked or migrated, but is used by Ubercart products. Please review and correct your products. !link', array(
      '!platform' => l($platform->title, "node/{$platform->nid}"),
      '!link' => l(t('Aegir Ubercart integration admin section.'), 'admin/store/hosting'),
    )) . '</p>';
    $error_state .= $error_message;
    variable_set('uc_hosting_products_error_state', $error_state);
    return $error_message;
  }
  else {
    return FALSE;
  }
}
