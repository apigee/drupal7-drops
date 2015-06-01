<?php print render($form['revision_tools']); ?>
<?php print render($form['published_docs_link']); ?>
<?php print render($form['options']); ?>
<?php if (array_key_exists('verbose', $form)) print render($form['verbose']); ?>
<?php if (array_key_exists('mid', $form)) print render($form['mid']); ?>

<?php foreach ($form['resources'] as $i => &$resource) : ?>
  <?php if (!is_numeric($i)) continue; ?>
  <fieldset class="collapsible form-wrapper fieldset titled" id="edit-<?php print $i; ?>">
      <div class="fieldset-description" style="position:relative;float:right;width:150px;top:4px;right:70px;"><?php print render($resource['ops']); ?></div>
    <legend><span class="fieldset-title fieldset-legend"><span class="icon"></span><?php print check_plain($resource['#title']); ?></span></legend>
    <div class="fieldset-content fieldset-wrapper clearfix">
    <?php if (array_key_exists('#headers', $resource)) : ?>
      <table class="table table-striped">
        <thead>
        <tr>
          <th class="select-all"></th>
            <?php //foreach ($resource['#headers'] as $resource_header) print '<th class="col-sm-1">' . $resource_header . '</th>'; ?>
            <th class="col-sm-2">Name</th>
            <th class="col-sm-3">Description</th>
            <th class="col-sm-1">Method</th>
            <th class="col-sm-1">Authentication</th>
            <th class="col-sm-1">Node Association</th>
            <th class="col-sm-1">Published</th>
            <th class="col-sm-1">Synced</th>
            <th class="col-sm-2">Operations</th>
        </tr>

        </thead>
        <tbody>
      <?php $odd = TRUE; ?>
      <?php foreach ($resource['methods'] as $j => &$method) :?>
        <?php if (substr($j, 0, 1) == '#') continue; ?>
          <tr class="<?php print $odd ? 'odd' : 'even'; $odd = !$odd; ?>">
            <td><?php print render($method); ?></td>
            <td class="word-wrap"><?php print render($method['#data']['name']) ?></td>
            <td class="word-wrap"><?php print render($method['#data']['description']) ?></td>
            <td class="word-wrap"><?php print render($method['#data']['method']) ?></td>
            <td class="word-wrap"><?php print render($method['#data']['auth']) ?></td>
            <td class="word-wrap"><?php print render($method['#data']['node']) ?></td>
            <td class="word-wrap"><?php print render($method['#data']['status']) ?></td>
            <td class="word-wrap"><?php print render($method['#data']['synced']) ?></td>
            <td class="word-wrap"><?php print render($method['#data']['operations']) ?></td>
          </tr>
      <?php endforeach; // $resource['methods'] ?>
        </tbody>
      </table>
    <?php endif; // array_key_exists('#headers', $resource) ?>
    </div>
  </fieldset>
<?php endforeach; // $form['resources'] ?>
<?php print drupal_render_children($form); ?>
