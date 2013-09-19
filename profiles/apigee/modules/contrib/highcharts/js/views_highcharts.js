Drupal.behaviors.views_highcharts = {
	charts: [],
	attach: function(context) {
		jQuery.each(jQuery(".views-highcharts-chart", context).not(".views-highcharts-processed"), function(idx, value) {
			chart_id = jQuery(value).attr("id");
			if (Drupal.settings.views_highcharts[chart_id] != undefined) {
				Drupal.behaviors.views_highcharts.charts[chart_id] = 
					new Highcharts.Chart(Drupal.settings.views_highcharts[chart_id]);
				jQuery(value).addClass("views-highcharts-processed");
			}
		})
	},
	detach: function(context) {
		
	}
}