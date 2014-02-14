"use strict";

CKEDITOR.plugins.add('featurette', {
  lang: 'en',
  requires: 'widget,dialog',
  icons: 'featurette',
  init: function(editor) {

    CKEDITOR.dialog.add('featurette', this.path + 'dialogs/featurette.js');

    // Register the featurette widget.
    editor.widgets.add('featurette', {
      // Allow all HTML elements, classes, and styles that this widget requires.
      // Read more about the Advanced Content Filter here:
      // * http://docs.ckeditor.com/#!/guide/dev_advanced_content_filter
      // * http://docs.ckeditor.com/#!/guide/plugin_sdk_integration_with_acf
      allowedContent:
              'div(!featurette,align-left,align-right,align-center){width};' +
              'div(!featurette-content); h2(!featurette-title)',
      // Minimum HTML which is required by this widget to work.
      requiredContent: 'div(featurette)',
      // Define two nested editable areas.
      editables: {
        title: {
          // Define CSS selector used for finding the element inside widget element.
          selector: '.featurette-heading',
          // Define content allowed in this nested editable. Its content will be
          // filtered accordingly and the toolbar will be adjusted when this editable
          // is focused.
          allowedContent: 'br strong em span(text-muted)'
        },
        content: {
          selector: '.lead',
          allowedContent: 'p br ul ol li strong em'
        }
      },
      parts: {
        colImage: 'div.image',
        colText: 'div.message',
        link: 'div.image a',
        image: 'div.image img',
        h2: 'div.message h2.featurette-heading',
        content: 'div.message p.lead'
      },
      // Define the template of a new Simple Box widget.
      // The template will be used when creating new instances of the Simple Box widget.
      template:
              '<div class="row featurette">' +
              '<div class="image col-md-5">' +
              '<a href="" target="">' +
              '<img class="featurette-image img-responsive" alt="" src="http://placehold.it/500x500">' +
              '</a>' +
              '</div>' +
              '<div class="message col-md-7">' +
              '<h2 class="featurette-heading">First featurette heading. <span class="text-muted">It\'ll blow your mind.</span></h2>' +
              '<p class="lead">Donec ullamcorper nulla non metus auctor fringilla. Vestibulum id ligula porta felis euismod semper. Praesent commodo cursus magna, vel scelerisque nisl consectetur. Fusce dapibus, tellus ac cursus commodo.</p>' +
              '</div>' +
              '</div>',
      button: 'Create a featurette',
      dialog: 'featurette',
      upcast: function(element) {
        return element.name == 'div' && element.hasClass('featurette');
      },
      init: function() {
        this.setData('src', this.parts.image.getAttribute('src') || '');
        this.setData('alt', this.parts.image.getAttribute('alt') || '');
        this.setData('align', this.parts.colImage.getAttribute('data-align') || 'left');
        if (this.parts.link) {
          this.setData('link', this.parts.link.getAttribute('href') || '');
          this.setData('target', this.parts.link.getAttribute('target') || '');
        }
      },
      data: function() {
        this.parts.image.setAttribute('src', this.data.src);
        // CKEditor is adding some stuff that causes the image not to be saved.
        this.parts.image.removeAttribute('data-cke-saved-src');
        this.parts.image.setAttribute('alt', this.data.alt);
        if (this.data.align != this.parts.colImage.getAttribute('data-align')) {
          this.parts.colImage.setAttribute('data-align', this.data.align);
          if (this.data.align == 'right') {
            this.parts.colImage.addClass('col-md-push-7');
            this.parts.colText.addClass('col-md-pull-5');
          }
          else {
            this.parts.colImage.removeClass('col-md-push-7');
            this.parts.colText.removeClass('col-md-pull-5');
          }
        }
        if (!this.data.link && this.parts.link) {
          // Link has been removed. Move the image and remove the link.
          this.parts.image.move(this.parts.colImage);
          this.parts.link.remove();
        }
        else if (this.data.link) {
          if (!this.parts.colImage.findOne('a')) {
            var newLink = new CKEDITOR.dom.element('a');
            newLink.appendTo(this.parts.colImage);
            this.parts.link = newLink;
            this.parts.image.move(this.parts.link);
          }
          this.parts.link.setAttribute('href', this.data.link);
          this.parts.link.setAttribute('target', this.data.target);
        }
      }
    });
  }
});
