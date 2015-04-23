/**
 * Created by isaias on 12/27/13.
 */
(function ($) {
    Drupal.behaviors.devconnect_monetization_developer_reports = {
        attach: function (context) {

            var settings = Drupal.settings.devconnect_monetization_developer_reports;

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
            if(settings.date_format === 'd-m-Y') {
                $("#devconnect-monetization-developer-report-form .date[name='start_date']").datepicker("option", "dateFormat", "dd-mm-yy");
            } else {
                $("#devconnect-monetization-developer-report-form .date[name='start_date']").datepicker("option", "dateFormat", "mm/dd/yy");
            }

            $("#devconnect-monetization-developer-report-form .date[name='end_date']").datepicker({
                changeMonth : true,
                changeYear : true,
                numberOfMonths : 3,
                showAnim : "fold",
                onClose : function(selectedDate) {
                    $("#devconnect-monetization-developer-report-form .date[name='start_date']").datepicker("option", "maxDate", selectedDate);
                }
            });
            if(settings.date_format === 'd-m-Y') {
                $("#devconnect-monetization-developer-report-form .date[name='end_date']").datepicker("option", "dateFormat", "dd-mm-yy");
            } else {
            $("#devconnect-monetization-developer-report-form .date[name='end_date']").datepicker("option", "dateFormat", "mm/dd/yy");
}
        }
    };
})(jQuery);
