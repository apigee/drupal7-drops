<?php
/**
 * @file render utility for table data in forms
 *
 * Theme helper function.
 *
 * In its own inc file as it's a re-usable element that could be shared among
 * modules. I use it verbatim in a few places.
 */

if (! function_exists('theme_fieldsets_as_table')) {

  /**
   * Render the fields in an all-in-one edit form - as a table,
   *
   * The form was built as a nested fieldset, turn it into a table.
   *
   * This takes a 'fieldset' and re-renders all the elements into rows and
   * columns.
   *
   * @param A FAPI subform - a fieldset containing fieldsets containing form
   * fields - which become table, rows and cells respectively.
   */
  function theme_fieldsets_as_table(&$fields_container) {
    $output = "table";
    $table = array();
    // Fields_container is a fieldset, that will become a table
    $field_ids = element_children($fields_container);

    // Auto-gen the table header
    $header = array();
    $footer = array();
    $sample_row = array();

    // Aggregate ALL rows before creating the header,
    // in case they have unequal number of elements (cols)
    foreach ($field_ids as $fid ) {
      $sample_row += $fields_container[$fid];
    }
    $col_ids = element_children($sample_row);
    foreach ($col_ids as $col_id) {
      $sample_col = $sample_row[$col_id];
      if ($sample_col['#type'] == 'value' || $sample_col['#type'] == 'hidden') {
        continue;
      }
      $header[$col_id] = @$sample_col['#title'];
      $footer[$col_id] = @$sample_col['#description'];
    }

    foreach ($field_ids as $fid ) {
      $field_container = $fields_container[$fid];
      // $field_container is a fieldset that will become a row
      $table[$fid] = array();
      // Copy a few of the form attributes into this row style
      $table[$fid] += (array) $field_container['#attributes'];

      foreach (element_children($field_container) as $col_id) {
        $form_field = &$fields_container[$fid][$col_id]; // keep a handle so the element remembers when it's been rendered
        // $form_field is the actual HTML field that will become a cell
        unset($form_field['#title']);
        unset($form_field['#description']);

        $table[$fid]['data'][$col_id] = array(
          'data' => drupal_render($form_field),
          'class' => $col_id, // name our columns for js and css
        );
      }
    }
    // Add the descriptions in a bottom row.
    // This is an extra ROW, not a html table 'footer' thing as used by the
    // theme func.
    foreach ($footer as $col_id => $footer_description) {
      $table_footer['data'][$col_id] = '<small class="description">' . $footer_description . '</small>';
    }
    $table[] = $table_footer;
    return theme('table', array('header' => $header, 'rows' => $table));
  }

}
