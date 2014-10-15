<?php

?>
<table class="checkout-review table table-striped table-condensed">
  <tbody>
    <tr class="pane-data even even">
      <td class="pane-data-full">
      <div class="field field-name-commerce-customer-address field-type-addressfield field-label-hidden">
        <div class="field-items">
          <div class="field-item even">
            <div class="addressfield-container-inline name-block">
              <div class="name-block"><?php print t('Order Number: @order_number', array('@order_number' => $order->order_number)); ?></div>
            </div>
            <div class="addressfield-container-inline name-block">
              <div class="name-block"><?php print t('API Provider: @api_number', array('@api_number' => $api_provider)); ?></div>
            </div>
            <div class="addressfield-container-inline name-block">
              <div class="name-block"><?php print t('Amount:');?> <?php print commerce_currency_format($order->commerce_order_total[LANGUAGE_NONE][0]['amount'], $order->commerce_order_total[LANGUAGE_NONE][0]['currency_code']); ?></div>
            </div>
          </div>
        </div>
      </div>
      </td>
    </tr>
  </tbody>
</table>
