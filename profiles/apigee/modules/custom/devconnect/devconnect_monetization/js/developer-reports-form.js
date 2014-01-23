/**
 * Created by isaias on 12/27/13.
 */
(function ($) {
    Drupal.behaviors.devconnect_monetization_developer_reports = {
        attach: function (context) {

            $("#devconnect-monetization-developer-report-form .date[name='start_date']").datepicker({
                changeMonth : true,
                changeYear : true,
                showWeek : true,
                numberOfMonths : 1,
                showAnim : "fold",
                onClose : function(selectedDate) {
                    $("#devconnect-monetization-developer-report-form .date[name='end_date']").datepicker("option", "minDate", selectedDate);
                }
            });

            $("#devconnect-monetization-developer-report-form .date[name='end_date']").datepicker({
                changeMonth : true,
                changeYear : true,
                numberOfMonths : 3,
                showAnim : "fold",
                onClose : function(selectedDate) {
                    $("#devconnect-monetization-developer-report-form .date[name='start_date']").datepicker("option", "maxDate", selectedDate);
                }
            });
        }
    };
})(jQuery);
