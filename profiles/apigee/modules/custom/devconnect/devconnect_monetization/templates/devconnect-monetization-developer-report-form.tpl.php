<?php
$form = $variables['form'];
?>
<div id="create-report">
  <div class="intro">
    <p>Choose the following parameters below to generate a revenue report.</p>
  </div>
 <div class="report-section">
    <h3>Date Range</h3>
    <div class="row">
      <div class="span8">
        <p>Select a date range option.</p>
      </div>
      <div class="span16">
        <?php print drupal_render($form['start_date']); ?>
        <?php print drupal_render($form['end_date']); ?>
      </div>
    </div>
  </div>
  <div class="report-section">
    <h3>Reporting Level</h3>
    <div class="row">
      <div class="span8">
        <p>Choose a Summary or Detailed report.</p>
      </div>
      <div class="span16">
        <?php print drupal_render($form['reporting_level']); ?>
      </div>
    </div>
  </div>
  <?php if ($form['currency']['#type'] == 'radios'): ?>
  <div class="report-section">
    <h3>Currency</h3>
    <div class="row">
      <div class="span8">
        <p>Select the currency in which to display<br>transactions in this report.</p>
      </div>
      <div class="span16">
        <?php print drupal_render($form['currency']); ?>
      </div>
    </div>
  </div>
  <?php endif; ?>
  <div class="row">
    <div class="span24">
      <?php print drupal_render($form['download_report']); ?>
    </div>
  </div>
</div>

<?php print drupal_render_children($form); ?>