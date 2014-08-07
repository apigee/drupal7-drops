<?php
/**
 * @file
 * Provides the user register form HTML.
 */

if (user_is_logged_in()) {
  $identifier = 'user_register_form_modal';
  $modal_style = '';
  include dirname(__FILE__) . '/bootstrap-modal-forms.tpl.php';
}
