// Register the plugin within the editor.
CKEDITOR.plugins.add( 'jumbotron', {
	// This plugin requires the Widgets System defined in the 'widget' plugin.
	requires: 'widget',

	// Register the icon used for the toolbar button. It must be the same
	// as the name of the widget.
	icons: 'jumbotron',

	// The plugin initialization logic goes inside this method.
	init: function( editor ) {

		// Register the jumbotron widget.
		editor.widgets.add( 'jumbotron', {
			// Allow all HTML elements, classes, and styles that this widget requires.
			// Read more about the Advanced Content Filter here:
			// * http://docs.ckeditor.com/#!/guide/dev_advanced_content_filter
			// * http://docs.ckeditor.com/#!/guide/plugin_sdk_integration_with_acf
			allowedContent: {
        div: {
          classes: '!jumbotron',
        },
        div: {
          classes: '!content',
        },
        h1: true
      },
//        'div(!jumbotron);' +
//				'div(!content); h1',

			// Minimum HTML which is required by this widget to work.
			requiredContent: 'div(jumbotron)',

			// Define two nested editable areas.
			editables: {
				title: {
					// Define CSS selector used for finding the element inside widget element.
					selector: 'h1',
					// Define content allowed in this nested editable. Its content will be
					// filtered accordingly and the toolbar will be adjusted when this editable
					// is focused.
//					allowedContent: 'br strong em span(text-muted)'
				},
				content: {
					selector: '.content',
//					allowedContent: 'p br ul ol li strong em a[href,alt, role](btn, btn-primary, btn-default, btn-success, btn-info, btn-danger, btn-link, btn-lg, btn-sm, btn-xs)'
				}
			},
      
      parts: {
        title: 'h1',
        content: '.content'
      },

			// Define the template of a new Simple Box widget.
			// The template will be used when creating new instances of the Simple Box widget.
			template:
      '<div class="jumbotron">' +
        '<h1>Hello, world!</h1>' +
        '<div class="content">' +
          '<p>This is a simple hero unit, a simple jumbotron-style component for calling extra attention to featured content or information.</p>' +
          '<p><a href="#" class="btn btn-primary btn-lg" role="button">Learn more</a></p>' +
        '</div>' +
      '</div>',

			// Define the label for a widget toolbar button which will be automatically
			// created by the Widgets System. This button will insert a new widget instance
			// created from the template defined above, or will edit selected widget
			// (see second part of this tutorial to learn about editing widgets).
			//
			// Note: In order to be able to translate your widget you should use the
			// editor.lang.jumbotron.* property. A string was used directly here to simplify this tutorial.
			button: 'Create a jumbotron',

			// Check the elements that need to be converted to widgets.
			//
			// Note: The "element" argument is an instance of http://docs.ckeditor.com/#!/api/CKEDITOR.htmlParser.element
			// so it is not a real DOM element yet. This is caused by the fact that upcasting is performed
			// during data processing which is done on DOM represented by JavaScript objects.
			upcast: function( element ) {
				// Return "true" (that element needs to converted to a Simple Box widget)
				// for all <div> elements with a "jumbotron" class.
				return element.name == 'div' && element.hasClass( 'jumbotron' );
			},

			// When a widget is being initialized, we need to read the data ("align" and "width")
			// from DOM and set it by using the widget.setData() method.
			// More code which needs to be executed when DOM is available may go here.
			init: function() {
			},

			// Listen on the widget#data event which is fired every time the widget data changes
			// and updates the widget's view.
			// Data may be changed by using the widget.setData() method, which we use in the
			// Simple Box dialog window.
			data: function() {
			}
		} );
	}
} );
