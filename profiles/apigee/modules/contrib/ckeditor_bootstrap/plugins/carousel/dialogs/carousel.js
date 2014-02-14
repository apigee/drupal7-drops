"use strict";

(function($) {

  CKEDITOR.dialog.add('carousel', function(editor) {
    var maxSlides = 5;
    var lang = editor.lang.carousel,
        commonLang = editor.lang.common;

    var dialog = {
      title: 'Edit Carousel',
      minWidth: 600,
      minHeight: 300,
      contents: [
        {
          id: 'info',
          label: lang.infoTab,
          accessKey: 'I',
          elements: [
            {
              id: 'slides',
              type: 'select',
              label: lang.slideTitle,
              items: [],
              setup: function(widget) {
                this.setValue(widget.data.slides);
              },
              commit: function(widget) {
                widget.setData('slides', this.getValue());
              },
              onChange: function(api) {
                var dialog = this.getDialog();
                // Hide and show tabs.
                for (var i=1; i<=maxSlides; i++ ) {
                  if (i <= this.getValue()) {
                    dialog.showPage('slide' + i);
                  }
                  else {
                    dialog.hidePage('slide' + i);
                  }
                }
              }
            },
            {
              id: 'interval',
              type: 'text',
              label: lang.interval,
              setup: function(widget) {
                this.setValue(widget.data.interval);
              },
              commit: function(widget) {
                widget.setData('interval', this.getValue());
              }
            },
            {
              id: 'pause',
              type: 'checkbox',
              label: lang.pause,
              setup: function(widget) {
                this.setValue(widget.data.pause == 'hover');
              },
              commit: function(widget) {
                widget.setData('pause', (this.getValue()?'hover':''));
              }
            },
            {
              id: 'wrap',
              type: 'checkbox',
              label: lang.wrap,
              setup: function(widget) {
                this.setValue(widget.data.wrap);
              },
              commit: function(widget) {
                widget.setData('wrap', this.getValue());
              }
            },
          ]
        }
      ]
    };

    for (var ii = 1; ii <= maxSlides; ii++) {
     (function (i) { // Closure the i variable so that it is available in callbacks
        // Add slide count
        dialog.contents[0].elements[0].items.push([i + ' ' + (i == 1 ? lang.slideSingular : lang.slidePlural), i]);
        // Add tab for each slide
        dialog.contents[i] = {
          id: 'slide' + i,
          label: lang.slideSingular + ' ' + i,
          accessKey: i,
          elements: [
            {
              type: 'vbox',
              padding: 0,
              children: [
                {
                  type: 'hbox',
                  widths: ['380px', '220px'],
                  align: 'right',
                  children: [
                    {
                      id: 'src' + i,
                      type: 'text',
                      label: lang.imageTitle,
                      setup: function(widget) {
                        this.setValue(widget.data['src' + i]);
                      },
                      commit: function(widget) {
                        widget.setData('src' + i, this.getValue());
                      },
                      validate: CKEDITOR.dialog.validate.notEmpty(lang.urlMissing)
                    },
                    {
                      type: 'button',
                      id: 'browse',
                      style: 'display:inline-block;margin-top:16px;',
                      align: 'center',
                      label: commonLang.browseServer,
                      hidden: true,
                      filebrowser: 'slide' + i + ':src' + i
                    }
                  ]
                }
              ]
            },
            {
              id: 'alt' + i,
              type: 'text',
              label: lang.alt,
              setup: function(widget) {
                this.setValue(widget.data['alt' + i]);
              },
              commit: function(widget) {
                widget.setData('alt' + i, this.getValue());
              }
            },
          ],
        };
      }(ii));
    }

    return dialog;
  });
})(jQuery);