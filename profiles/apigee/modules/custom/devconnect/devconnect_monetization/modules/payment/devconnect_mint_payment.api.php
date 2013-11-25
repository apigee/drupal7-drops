<?php
/**
 * Author Isaias Arellano
 * User: isaias
 * Date: 11/6/13
 * Time: 1:51 PM
 */

function hook_mint_continue_complete_message() {
}

/**
 * @return string Returns the provider id as registered in backend API
 */
function hook_mint_provider_id() {
}

/**
 *
 * Payment orders are forwarded to this hook once endpoint returns the redirect URL
 *
 * Implementing module tailors the redirect form
 *
 * @param array $form
 * @param array $form_state
 * @param object $order
 * @param array $payment_method
 * @param \Apigee\Mint\DataStructures\Payment $payment
 *
 * @return array redirect form to make the offsite payment
 */
function hook_mint_dispatch_redirect_form($form, $form_state, $order, $payment_method, \Apigee\Mint\DataStructures\Payment $payment) {
}
