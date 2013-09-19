//
// Add multi-select toggling for nested treeviews
//
/*
  DIV.tree-branch
    LABEL.tree-branch-label
      checkbox.tree-branch-toggle
    DIV.tree-content
      LABEL
        checkbox.tree-leaf-toggle
      LABEL
        checkbox.tree-item
      
*/

/** 
 * Attach an event to each tree-branch-toggle - the representative toggle for a branch.
 * Onset, toggle all child elements of that branch 
 */

(function ($) {

  Drupal.behaviors.toggle_treeview = {
    attach: function (context, settings) {
      
      $('.tree-branch-toggle').change(
          function(e){
            var checked = $(this).attr("checked");
            // find current container
            parentElem = $(this).parent();
            while(! $(parentElem).is(".tree-branch") && (parentElem = $(parentElem).parent())) { /* loop */ } 
            if($(parentElem).is(".tree-branch")){
              // now set every child
              $('input:checkbox', parentElem).attr("checked", checked ? true : false);
            }
          }
        )  // each label

        // If making a change to a child, the parent is no longer 'select all'
        // Leaving the parent to recurse would override the manual settings otherwise.
        $('.tree-leaf-toggle, .tree-branch-toggle').change(
          function(e){
            // find current container
            parentElem = $(this).parent();
            if ($(this).hasClass('tree-branch-toggle')) parentElem = $(this).parent().parent().parent().parent();
            while(!$(parentElem).is("form") && ! $(parentElem).is(".tree-branch") && (parentElem = $(parentElem).parent())) { /* loop */ } 
            if ($(parentElem).is(".tree-branch")){
              // unset the 'select all' property as we are now individials
              $('> legend > label > input', parentElem).attr("checked", false);
            }
          }
        )  // each label

      
    } // end behaviour.toggle_treeview.attach func
  }
  
})(jQuery);
