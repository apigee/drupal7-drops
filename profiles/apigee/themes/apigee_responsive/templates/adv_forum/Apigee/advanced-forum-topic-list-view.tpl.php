<?php
/**
 * @file views-view-table.tpl.php
 * Template to display a view as a table.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $header: An array of header labels keyed by field id.
 * - $fields: An array of CSS IDs to use for each field id.
 * - $class: A class or classes to apply to the table, based on settings.
 * - $row_classes: An array of classes to apply to each row, indexed by row
 *   number. This matches the index in $rows.
 * - $rows: An array of row items. Each row is an array of content.
 *   $rows are keyed by row number, fields within rows are keyed by field ID.
 * @ingroup views_templates
 */
?>
<div class="panel panel-default">
  <div class="panel-heading">
    <h3><?php print t('Topics List'); ?></h3>
  </div>
  <?php if (!empty($title)) : ?>
    <div class="panel-body muted">
      <?php print $title; ?>
    </div>
  <?php endif; ?>
  <div class="table-responsive">
    <table class="table">
      <thead>
      <tr>
        <?php foreach ($header as $field => $label): ?>
          <th class="views-field views-field-<?php print $fields[$field]; ?>">
            <?php print $label; ?>
          </th>
        <?php endforeach; ?>
      </tr>
      </thead>
      <tbody>
      <?php foreach ($rows as $count => $row): ?>
        <tr class="<?php print implode(' ', $row_classes[$count]); ?>">
          <?php if (empty($shadow[$count])): ?>
            <?php foreach ($row as $field => $content): ?>
              <td class="views-field views-field-<?php print $fields[$field]; ?>">
                <?php if ($field == 'title' && !empty($sticky[$count])): ?>
                  <span class="sticky-label"><?php print t('Sticky:'); ?></span>
                <?php endif; ?>
                <?php print $content; ?>
              </td>
            <?php endforeach; ?>
          <?php else: ?>
            <td class="views-field views-field-<?php print $fields['topic_icon']; ?>">
              <?php print $row['topic_icon']; ?>
            </td>
            <td class="views-field views-field-<?php print $fields['title']; ?>" colspan="<?php print count($header)-1; ?>">
              <?php print $shadow[$count]; ?>
            </td>
          <?php endif; ?>
        </tr>
      <?php endforeach; ?>
      </tbody>
    </table>
  </div>
  <div class="panel-footer"></div>
</div>