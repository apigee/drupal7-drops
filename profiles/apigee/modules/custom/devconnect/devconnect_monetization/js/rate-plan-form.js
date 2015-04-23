(function ($) {
    Drupal.behaviors.devconnect_monetization_rate_plan_form = {
        attach: function (context) {

            var settings = Drupal.settings.devconnect_monetization_rate_plan_form;

            // Initialize datepicket widget
            $("input.date").datepicker();
            if(settings.date_format === 'd-m-Y') {
                $("input.date").datepicker("option", "dateFormat", "dd-mm-yy");
            } else {
                $("input.date").datepicker("option", "dateFormat", "mm/dd/yy");
            }

            // Listen for the user to switch between tabs
            if ($(".tabbable .nav-tabs a.plan-tab[data-toggle='tab']").length == 0) {

                var selected_plan_id = $("input[name='plan_options']:first").val();
                var action = (settings.active_plan_id === null || settings.active_plan_id === undefined)
                    || (settings.active_plan_id !== null && settings.active_plan_ends_today) ? "purchase" : "cancel";
                var oppositeAction = action == "cancel" ? "purchase" : "cancel";
                $("input[name='action'][value='" + action + "']").prop("checked", true);
                $("input[name='plan_options']:first").prop("checked", true);

                // Set current action title
                $("span#action_title_wrapper").html(settings.action_title_wrapper[action]);

                // Toggle action's messages
                $("p." + action).show();
                $("p." + oppositeAction).hide();
                $("strong." + action).show();
                $("strong." + oppositeAction).hide();

                // Set title to action box
                $("span#plan_name_wrapper").html(settings.product_specific_plan_name);

                // Set initial value to start_date datepicker widget
                $("input#edit-start-date").val(settings.select_date_tip[action]);

                $("#edit-" + action + ".btn.form-submit").show();
                $("#edit-" + oppositeAction + ".btn.form-submit").hide();

                if (settings.date_limits == undefined) {
                    return;
                }
                // Set the minimum date for purchase/cancel to be performed
                var date_limit = settings.date_limits[selected_plan_id].min_date;
                $("input.date").datepicker("option", "minDate", new Date(date_limit.year, date_limit.month - 1, date_limit.day));

                // If maximum date is set, then set the maximum date for purchase/cancel to be performed
                date_limit = settings.date_limits[selected_plan_id].max_date;
                if (date_limit != undefined) {
                    $("input.date").datepicker("option", "maxDate", new Date(date_limit.year, date_limit.month - 1, date_limit.day));
                }
            }

            $(".tabbable .nav-tabs a.plan-tab[data-toggle='tab']").on("show.bs.tab", function (e){

                // Set fragment to the tab
                var href = $(this).attr("href");
                var formAction = $("form#devconnect-monetization-plan-form").attr("action");
                if (formAction.indexOf("#") > 0) {
                    formAction = formAction.substring(0, formAction.indexOf("#"));
                }
                $("form#devconnect-monetization-plan-form").attr("action", formAction + href);

                $("a.accept.plan.requirement").each(function(){
                    var requirementUrl = $(this).attr("href");
                    if (requirementUrl.indexOf("%23") > 0) {
                        requirementUrl = requirementUrl.substring(0, requirementUrl.indexOf("%23"));
                    }
                    $(this).attr("href", requirementUrl + "%23" + encodeURI(href.substring(1)));
                });

                var selected_plan_id = $(this).attr("plan-id");

                // Update the value of the selected tab plan
                $("input[name='plan_options'][value='" + selected_plan_id + "']").prop("checked", true);

                // Get current plan's action: purchase|cancel
                var action = (selected_plan_id == settings.active_plan_id && !settings.active_plan_ends_today) ? "cancel" : "purchase";

                // Get current plan's no action
                var oppositeAction = action == "cancel" ? "purchase" : "cancel";

                // Set action radio value according to plan's current action
                $("input[name='action'][value='" + action + "']").prop("checked", true);

                // Set current action title
                $("span#action_title_wrapper").html(settings.action_title_wrapper[action]);

                // Toggle action's messages
                $("p." + action).show();
                $("p." + oppositeAction).hide();
                $("strong." + action).show();
                $("strong." + oppositeAction).hide();

                // Set title to action box
                var planName = $(this).html();
                var futurePlanSpanPos = planName.indexOf("<span");
                planName = futurePlanSpanPos < 0 ? planName : planName.substring(0, futurePlanSpanPos - 1).trim();
                $("span#plan_name_wrapper").html(planName);

                // Set initial value to start_date datepicker widget
                $("input#edit-start-date").val(settings.select_date_tip[action]);

                // Toggle action buttons
                $("#edit-" + action + ".btn.form-submit").show();
                $("#edit-" + oppositeAction + ".btn.form-submit").hide();

                var tab_id_selector = $(this).attr("href");

                if (settings.date_limits == undefined) {
                    return;
                }
                // Set the minimum date for purchase/cancel to be performed
                var date_limit = settings.date_limits[selected_plan_id].min_date;
                $("input.date").datepicker("option", "minDate", new Date(date_limit.year, date_limit.month - 1, date_limit.day));

                // If maximum date is set, then set the maximum date for purchase/cancel to be performed
                date_limit = settings.date_limits[selected_plan_id].max_date;
                if (date_limit != undefined) {
                    $("input.date").datepicker("option", "maxDate", new Date(date_limit.year, date_limit.month - 1, date_limit.day));
                }

                // Hide purchase form when future plan tab is pre-selected and returning from other rate plan tab
                if ($("div.tab-content.plans-comparison div" + tab_id_selector).find("li.active a[plan-version]").attr("plan-version") == "future") {
                    $("div.purchase-plan.well").hide();
                }
                else {
                    $("div.purchase-plan.well").show();
                }
            });

            // Hide purchase form when toggling between current and future
            $("a[data-toggle][plan-version]").on("show.bs.tab", function (e){
                if ($(this).attr("plan-version") == "future") {
                    $("div.purchase-plan.well").hide();
                }
                else {
                    $("div.purchase-plan.well").show();
                }
            });

            // If not default tab, show first tab
            if ($("input[name='plan_options']:checked").length == 0) {
                $(".tabbable .nav-tabs a.plan-tab[data-toggle='tab']:eq(0)").tab("show");
            }
            else {
                var index = $("input[name='plan_options']").index($("input[name='plan_options']:checked"));
                $(".tabbable .nav-tabs a.plan-tab[data-toggle='tab']:eq(" + index + ")").trigger("show.bs.tab");
            }
        }
    };
})(jQuery);