<?php

/**
 * @file
 * Hooks provided by the bootstrap_modal_forms module.
 */

/**
 * Registers forms with Bootstrap to be displayed as modals.
 *
 * This example registers the sitewide "Contact Us" form.
 *
 * @return array
 */
function hook_bootstrap_modal_forms() {
  module_load_include('inc', 'contact', 'contact.pages');
  $items = array();
  $items['contact_modal_form'] = array(
    'form' => drupal_get_form('contact_site_form'),
    'title' => t('Contact Us'),
    'action' => 'contact'
  );
  return $items;
}