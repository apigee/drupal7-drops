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
              <div class="name-block">Order Number: <?php echo $order->order_number; ?></div>
            </div>
            <div class="addressfield-container-inline name-block">
              <div class="name-block">API Provider: <?php echo $api_provider; ?></div>
            </div>
            <div class="addressfield-container-inline name-block">
              <div class="name-block">Amount: <?php echo sprintf('%s %.2f', $order->commerce_order_total[LANGUAGE_NONE][0]['currency_code'], $order->commerce_order_total[LANGUAGE_NONE][0]['amount']/100); ?></div>
            </div>
          </div>
        </div>
      </div>
      </td>
    </tr>
  </tbody>
</table>
