(function($) {
	Drupal.behaviors.devconnect_org = {
		attach: function(context) {
    	$("form.form-stacked button.btn.primary.action:not(button-processed)").click(function(evt) {
				evt.stopPropagation();
				document.location.href=Drupal.settings.basePath+jQuery(this).attr('data-url');
				return false;
			}).addClass("button-processed");

			// jQuery Select
			$('select#api_product').attr('title', 'Select an API Product');

			var sl = $('select#api_product').selectList({
			  instance: true,
			  clickRemove: false,
			  onAdd: function (select, value, text) {
			    $('.selectlist-item').last().append('<span class="delete"></span>');
			  }
			});

			$('.selectlist-list').on('click', '.delete', function(event) {
			 sl.remove($(this).parent().data('value'));
			});

			$('.selectlist-item').append('<span class="delete"></span>');


		},
		detach: function(context) {
		}
	}
})(jQuery);
