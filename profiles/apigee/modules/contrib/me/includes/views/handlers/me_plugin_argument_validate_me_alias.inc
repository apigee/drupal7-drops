<?php

/**
 * @file
 *
 * Argument validator to help validate the me alias. If the argument is 'me', other validators
 * will most likely be useless at this point, so we provde the same functionality as the
 * user validator.
 */

/**
 * Validate whether an argument is an acceptable me alias, and user name/uid.
 */
class me_plugin_argument_validate_me_alias extends views_plugin_argument_validate_user {

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
   
    $form['roles']['#dependency'] = array(
      'edit-options-validate-options-me-restrict-roles' => array(1)
    );

    $form['me_redirect'] = array(
      '#type' => 'checkbox',
      '#title' => t("Redirect to the users uid when '%me' is entered as an argument.", array('%me' => _me_get_me_alias(TRUE))),
      '#description' => t("If selected, when a user enters '%me' for this argument, they will be redirected to the view with their user id inplace of '%me'.", array('%me' => _me_get_me_alias(TRUE))),
      '#default_value' => !empty($this->argument->options['me_redirect']),
    );
  }
  
  /**
   * Provides a form of options for our validator.
   */
  function options_validate(&$form, &$form_state) {
    // We are unable to rely on options having already been set, so let's make
    // sure defaults are here:
    parent::options_validate($form, $form_state);
    
    if (!isset($this->argument->options['me_redirect'])) {
      $this->argument->options['me_redirect'] = FALSE;
      $this->argument->options['me_validate_user_argument_type'] = 'uid';
      $this->argument->options['me_validate_user_roles'] = array();
    }

  }

  /**
   * Performs the actual validation.
   */
  function validate_argument($argument) {
    // Only modify the argument when the wildcard does not equal the 'me' alias.
    
    if (!isset($this->argument->options['wildcard']) || !_me_is_alias($this->argument->options['wildcard'])) {
      $uid_args = array();
      $seperator = ' ';
      if (empty($this->argument->options['break_phrase'])) {
        $uid_args[] = $argument;
      }
      else {
        // Modified from views_break_phrase() to include characters that a 'me' alias
        // may include.
        if (preg_match('/^([0-9a-zA-Z]+[+ ])+[0-9a-zA-Z]+$/', $argument)) {
          // The '+' character in a query string may be parsed as ' '.
          $uid_args = preg_split('/[+ ]/', $argument);
        }
        else if (preg_match('/^([0-9a-zA-Z]+,)*[0-9a-zA-Z]+$/', $argument)) {
          $seperator = ',';
          $uid_args = explode(',', $argument);
        }
      }

      // Check if we need to do a redirect, and make sure the option is disabled if we don't.
      // But be sure not to redirect in a live preview.
      if (empty($this->view->live_preview) && !empty($this->options['me_redirect'])) {
        $redirect_args = array_filter($uid_args, create_function('$n', 'return _me_is_alias($n);'));
        if (!empty($redirect_args)) {
          // Trigger a redirect.
          me_views_pre_execute(NULL, TRUE);
        }
      }

      // The alias could potentially show up more than once. Loop over each argument
      // and check to be sure.
      foreach ($uid_args as $key => $uid_arg) {
        $uid_args[$key] = _me_check_arg($uid_arg, $this->options['type'] == 'name', FALSE);
        //Make sure we only allow access to the current user
        if (is_numeric($uid_args[$key])) {
          if ($uid_args[$key] != $GLOBALS['user']->uid) {
            $this->view->build_info['denied'] = TRUE;
            return FALSE;
          }
        } else if ($uid_args[$key] != $GLOBALS['user']->name) {
          $this->view->build_info['denied'] = TRUE;
          return FALSE;
        }
      }

      $argument = implode($seperator, $uid_args);
    }

    // We always need to return the parent::set_argument() call.
    $this->argument->options['validate_user_argument_type'] = $this->options['type'];
    $this->argument->options['validate_user_restrict_roles'] = $this->options['restrict_roles'];
    $this->argument->options['validate_user_roles'] = $this->options['roles'];
    return parent::validate_argument($argument);
  }
}

