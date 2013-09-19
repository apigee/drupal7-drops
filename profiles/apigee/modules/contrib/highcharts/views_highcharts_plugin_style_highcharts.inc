<?php
// $Id$
/**
 * @file
 * Contains the highcharts style plugin.
 */

/**
 * Style plugin to render each item in a slideshow of an ordered or unordered list.
 *
 * @ingroup views_style_plugins
 */
class views_highcharts_plugin_style_highcharts extends views_plugin_style {

	function init(&$view, &$display, $options = NULL) {
		parent::init($view, $display, $options);
		
	}

  // Set default options
  function option_definition() {
    $options = parent::option_definition();

    // Load all include files from views slideshow addons.
    module_load_all_includes('views_highcharts.inc');
    
	$options['format'] = array(
		"contains" => array(
			"chart_type" => array("default" => "column"),
			"skin" => array("default" => "gray"),
			"title" => array("default" => "highchart"),
			"subtitle" => array("default" => ""),
			"legend_enabled" => array("default" => "no"),
			"legend_settings" => array("default" => array()),
			"tooltip_formatter" => array("default" => "'<b>'+this.series.name+'</b><br/>'+this.x+': '+this.y"),
			"swap_axes" => array("default" => false)
		)	
	);
	
	$options['y_axis'] = array(
		"contains" => array(
			"title" => array("default" => ""),
			"title_align" => array("default" => "middle"),
			"reversed" => array("default" => false)
		)
	);

	$options['x_axis'] = array(
		"contains" => array(
			"dataset_data" => array("default" => array()),
			"dataset_label" => array("default" => array()),
			"reversed" => array("default" => false)
		)
	);


	$options['pie'] = array(
		"contains" => array(
			"dataset_data" => array("default" => array()),
			"selected_slice" => array("default" => array()),
			"donut_hole" => array("default" => 0),
			"data_labels" => array("default" => true)
		)
	);


    // call every module using hook_views_slideshow_option_definition and merge
    // it's return value with the other default options.
    return array_merge($options, module_invoke_all('views_highcharts_option_definition'));
  }
 

  function options_form(&$form, &$form_state) {
	
    module_load_all_includes('views_highcharts.inc');

	$form['format']['#title'] = t('Format');
	$form['format']['#type'] = "fieldset";
	
    $form['format']['chart_type'] = array(
      '#title' => t('Chart Type'),
      '#type' => 'select',
      '#options' => array( 'area' => 'Area', 'areaspline' => 'Area Spline', 'bar' => 'Bar', 'column' => 'Column', 'line' => 'Line',  'pie' => 'Pie', 'spline' => 'Spline' ),
      '#multiple' => FALSE,
      '#required' => TRUE,
      '#default_value' => $this->options['format']['chart_type'],
      '#id' => 'format_chart_type'
    );
    $form['format']['skin'] = array(
      '#type' => 'select',
      '#title' => t('Skin'),
      '#default_value' => $this->options['format']['skin'],
      '#options' => array(
        'gray' => t('Gray'),
        'grid' => t('Grid'),
      ),
    );
    $form['format']['title'] = array(
      '#title' => t('Chart Title'),
      '#type' => 'textfield',
      '#default_value' => $this->options['format']['title'],
    );

    $form['format']['subtitle'] = array(
      '#title' => t('Chart SubTitle'),
      '#type' => 'textfield',
      '#default_value' => $this->options['format']['subtitle'],
    );

    $form['format']['legend_enabled'] = array(
      '#title' => t('Show Legend'),
      '#type' => 'select',
      '#options' => array( 'yes' => 'Yes', 'no' => 'No' ),
      '#multiple' => FALSE,
      '#required' => TRUE,
      '#default_value' => $this->options['format']['legend_enabled'],
    );
/*    
	//Coming soon! 
	$form['format']['legend_settings'] = array(
      '#title' => t('Legend Settings'),
      '#type' => 'fieldset', 
	  '#states' => array(
  		"invisible" => array(
			"#legend_enabled" => array("value" => "no")
		)
	  ) 
	);

*/


   $form['format']['tooltip_formatter'] = array(
      '#title' => t('Tool Tip Formatter'),
      '#type' => 'textarea',
      '#required' => TRUE,
      '#default_value' => $this->options['format']['tooltip_formatter'],
    );

    $form['format']['swap_axes'] = array(
      '#title' => t('Swap X and Y Axis'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['format']['swap_axes'],
    );


	$form['y_axis'] = array(
		"#title" => t('Y Axis'),
		"#type" => "fieldset",
		"#states" => array(
			"invisible" => array(
				"#format_chart_type" => array("value" => "pie")
			)
		)
		
	);
	$form['y_axis']['title'] = array(
      '#title' => t('Y-Axis Title'),
      '#type' => 'textfield',
      '#default_value' => $this->options['y_axis']['title'],
    );

    $form['y_axis']['title_align'] = array(
      '#title' => t('Y-Axis Title Align'),
      '#type' => 'select',
      '#options' => array( 'low' => 'Low', 'middle' => 'Middle', 'high'=> 'High' ),
      '#multiple' => FALSE,
      '#required' => TRUE,
      '#default_value' => $this->options['y_axis']['title_align'],
    );

    $form['y_axis']['reversed'] = array(
      '#title' => t('Reverse This Axis?'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['y_axis']['reversed'],
    );

    $highcharts_handlers = $this->display->handler->get_handlers('field');
    $highcharts_fields = array();
    foreach ($highcharts_handlers as $key => $value) {
      $highcharts_fields[$key] = $value->options['ui_name'] ? $value->options['ui_name'] : $value->options['field'];
    }
 
	$form['x_axis'] = array(
		"#title" => t('X Axis'),
		"#type" => "fieldset",
		"#states" => array(
			"invisible" => array(
				"#format_chart_type" => array("value" => "pie")
			)
		)
		
	);
	
	$form['x_axis']['dataset_data'] = array(
      '#title' => t('Data Column(s)'),
      '#type' => 'checkboxes',
      '#options' => $highcharts_fields,
      '#required' => TRUE,
      '#default_value' => $this->options['x_axis']['dataset_data'],
	  '#description' => "All returned fields may be selected to be plotted on a highchart. But keep in mind, selecting text fields here may break your chart, cause death, disease and famine and quite possibly end the world as we know it."
    );

    $form['x_axis']['dataset_label'] = array(
      '#title' => t('Data Labels Column'),
      '#type' => 'radios',
      '#options' => $highcharts_fields,
      '#required' => TRUE,
      '#default_value' => $this->options['x_axis']['dataset_label'],
    );

    $form['x_axis']['reversed'] = array(
      '#title' => t('Reverse This Axis?'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['x_axis']['reversed'],
    );

	$form['pie'] = array(
		"#title" => t('Pie'),
		"#type" => "fieldset",
		"#states" => array(
			"visible" => array(
				"#format_chart_type" => array("value" => "pie")
			)
		)
		
	);
	
	$form['pie']['dataset_data'] = array(
      '#title' => t('Pie Pieces'),
      '#type' => 'checkboxes',
      '#options' => $highcharts_fields,
      '#required' => TRUE,
      '#default_value' => $this->options['pie']['dataset_data'],
	  '#description' => "All returned fields may be selected to be plotted on a highchart. But keep in mind, selecting text fields here may break your chart, cause death, disease and famine and quite possibly end the world as we know it. If a label exists, the field's label will be used in the legend/label."
    );

	$form['pie']['selected_slice'] = array(
      '#title' => t('Selected Slice'),
      '#type' => 'radios',
      '#options' => $highcharts_fields,
      '#required' => TRUE,
      '#default_value' => $this->options['pie']['selected_slice'],
    );

	$form['pie']['donut_hole'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Donut Hole'),
	  '#description' => t('Donut Hole Size size as expressed by a percentage of the chart. (max 95%)'),
	  '#default_value' => $this->options['pie']['donut_hole']
	);
    $form['pie']['data_labels'] = array(
      '#title' => t('Data Labels Enabled'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['pie']['data_labels'],
    );

  }

  function options_validate(&$form, &$form_state) {
    module_load_all_includes('views_highcharts.inc');
  }

  function options_submit(&$form, &$form_state) {
    module_load_all_includes('views_highcharts.inc');
    $arguments = array(
      $form,
      &$form_state,
    );

    // Call all modules that use hook_views_slideshow_options_form_submit
    foreach (module_implements('views_highcharts_options_form_submit') as $module) {
      $function = $module . '_views_highcharts_options_form_submit';
      call_user_func_array($function, $arguments);
    }
  }

  function views_highcharts_get_skins() {}
}

