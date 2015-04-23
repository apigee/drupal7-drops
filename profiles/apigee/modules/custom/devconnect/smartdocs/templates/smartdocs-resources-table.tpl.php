<?php print render($form['revision_tools']); ?>
<?php print render($form['options']); ?>
<?php if (array_key_exists('verbose', $form)) print render($form['verbose']); ?>
<?php if (array_key_exists('mid', $form)) print render($form['mid']); ?>

<?php foreach ($form['resources'] as $i => &$resource) : ?>
  <?php if (!is_numeric($i)) continue; ?>
  <fieldset class="collapsible form-wrapper fieldset titled" id="edit-<?php print $i; ?>">
      <div class="fieldset-description" style="position:relative;float:right;width:150px;top:4px;"><?php print render($resource['ops']); ?></div>
    <legend><span class="fieldset-title fieldset-legend"><span class="icon"></span><?php print check_plain($resource['#title']); ?></span></legend>
    <div class="fieldset-content fieldset-wrapper clearfix">
    <?php if (array_key_exists('#headers', $resource)) : ?>
      <table class="api-model-table sticky-enabled">
        <thead>
        <tr>
          <th class="select-all"></th>
          <?php foreach ($resource['#headers'] as $resource_header) print '<th>' . $resource_header . '</th>'; ?>
        </tr>
        </thead>
        <tbody>
      <?php $odd = TRUE; ?>
      <?php foreach ($resource['methods'] as $j => &$method) :?>
        <?php if (substr($j, 0, 1) == '#') continue; ?>
          <tr class="<?php print $odd ? 'odd' : 'even'; $odd = !$odd; ?>">
            <td><?php print render($method); ?></td>
            <?php foreach ($method['#data'] as $val): ?>
            <td class="word-wrap"><?php print $val; ?></td>
            <?php endforeach; ?>
          </tr>
      <?php endforeach; // $resource['methods'] ?>
        </tbody>
      </table>
    <?php endif; // array_key_exists('#headers', $resource) ?>
    </div>
  </fieldset>
<?php endforeach; // $form['resources'] ?>
<?php print drupal_render_children($form); ?>
