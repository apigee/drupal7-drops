<?php
$form = $variables['form'];
?>
  <div id="create-report">
    <div class="intro">
      <p><?php print t('Choose the following parameters below to generate a revenue report.'); ?></p>
    </div>
    <div class="report-section">
      <h3><?php print t('Date Range'); ?></h3>
      <hr>
      <div class="row">
        <div class="col-md-4">
          <p><?php print t('Select a date range option.'); ?></p>
        </div>
        <div class="col-md-8">
          <?php print drupal_render($form['start_date']); ?>
          <?php print drupal_render($form['end_date']); ?>
        </div>
      </div>
    </div>
    <div class="report-section">
      <h3><?php print t('Reporting Level'); ?></h3>
      <hr>
      <div class="row">
        <div class="col-md-4">
          <p><?php print t('Choose a Summary or Detailed report.'); ?></p>
        </div>
        <div class="col-md-8">
          <?php print drupal_render($form['reporting_level']); ?>
        </div>
      </div>
    </div>
    <?php if ($form['currency']['#type'] == 'radios'): ?>
      <div class="report-section">
        <h3><?php print t('Currency'); ?></h3>
        <hr>
        <div class="row">
          <div class="col-md-4">
            <p><?php print t('Select the currency in which to display<br />transactions in this report.'); ?></p>
          </div>
          <div class="col-md-8">
            <?php print drupal_render($form['currency']); ?>
          </div>
        </div>
      </div>
    <?php endif; ?>
    <div class="row">
      <div class="col-md-12">
        <?php print drupal_render($form['download_report']); ?>
      </div>
    </div>
  </div>
<?php print drupal_render_children($form); ?>
