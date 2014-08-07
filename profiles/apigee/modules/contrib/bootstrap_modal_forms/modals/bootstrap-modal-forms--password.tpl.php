<?php
/**
 * @file
 * Provides the user password request form HTML.
 */

if (user_is_logged_in()) {
  $identifier = 'user_password_form_modal';
  $modal_style = '';
  include dirname(__FILE__) . '/bootstrap-modal-forms.tpl.php';
}
