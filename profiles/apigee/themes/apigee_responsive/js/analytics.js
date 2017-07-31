(function ($) {
    Drupal.behaviors.apigee_responsive_analytics = {
        attach: function (context, settings) {
            var apps = Drupal.settings.apigee_responsive_analytics_data;
            var timeText = Drupal.t('Time (UTC)');

            $.each(apps, function(index, value) {
                var wrapper = Object.keys(value)[0];
                console.log(value[wrapper].analytics_data.series);
                $('#'+wrapper).highcharts({
                    title: {
                        text: value[wrapper].analytics_data.chart_name,
                        x: -20
                    },
                    xAxis: {
                        categories: value[wrapper].analytics_data.categories,
                        title: {
                            text: timeText,
                            margin: 0
                        }
                    },
                    yAxis: {
                        title: {
                            text: value[wrapper].analytics_data.chart_name_y
                        },
                        plotLines: [{
                            value: 0,
                            width: 1,
                            color: '#808080'
                        }],
                        min:  0
                    },
                    legend: {
                        layout: 'vertical',
                        align: 'right',
                        verticalAlign: 'top',
                        x: -10,
                        y: 100,
                        borderWidth: 0
                    },
                    series: value[wrapper].analytics_data.series,
                    credits: {
                        enabled: false
                    },
                    plotOptions: {
                        line: {
                            marker: {
                                enabled: false
                            }
                        }
                    }
                });
            });
        }
    };
})(jQuery);