(function ($) {
///////////////

// Add multi-select toggling for file type checkboxes
//
/*
  DIV.tree-branch
    DIV.tree-content
      LABEL.file-item
        checkbox.tree-leaf
      LABEL.image-item
        checkbox.tree-item
      
*/

/** 
 * Add some checkboxes which, if clicked will toggle on or off all matching file type selectors
 */

Drupal.behaviors.filetype_selectors = {
  attach: function (context, settings) {
    // Add new UI boxes.
    // The form theme should have provided a placeholder indicating where to put it.
    var types = {'html-item':'Page', 'image-item':'Image', 'resource-item':'Resource', 'document-item':'Document'};
    for (type in types){
      $('#import-html-selectors').append("<label class='"+ type +"-wrapper'><input type='checkbox' value='"+ type +"' />"+ types[type] +"</label>");
    }
    $('#import-html-selectors input').change(
      function(e){
        var checked = $(this).attr("checked");
        $('#import-html-importprocess-form input.'+ $(this).val()).attr("checked", checked ? 1 : 0);
      }
    )
  } // behaviors.filetype_selectors.attach func
};   


///////////
})(jQuery);