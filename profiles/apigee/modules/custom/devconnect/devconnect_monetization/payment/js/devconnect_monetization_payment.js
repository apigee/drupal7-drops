jQuery(function($) {
	$("form div#edit-review-pane-1").hide();
	$('#commerce-checkout-form-checkout').ajaxStart(function(){
		$("#edit-continue").attr("disabled", true);
	});
	$('#commerce-checkout-form-checkout').ajaxSuccess(function(){
		$("#edit-continue").attr("disabled", false);
	});
});