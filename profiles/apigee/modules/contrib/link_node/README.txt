The link_node module allows users to link to another node from
the body of another node.  

The syntax of these links ("[node:NNNN]") is incompatible with the attached_node module.

INSTALLATION

1. Unpack the link_node distribution into the modules directory.

2. Enable the link_node module in the administer --> modules page 
of the admin section.

3. Once enabled, there will be new option within the "input formats" area of the
"administer" area of Drupal.  To get to it, go "input formats" (admin/filters), and
then click on the first "Configure" link, under "permissions and settings".  This is
the configure link for "filtered HTML". 

Within this page (admin/filters/1), be sure that the box for "Link Node Filter" 
is checked off.  Then click the "Save configuration" button.

Alternatively, you can pick whichever filter you want (not just "Filtered HTML") to
add the functionality to.


WHAT IT DOES

Once you have enabled the attached node filter as described above, you (or any user
who can post) can add an embedded reference to any page using the syntax:

[node:##]

To do this, the page/node must be using "Filtered HTML" as its input format.  That
can be set as the default, or it can be turned on a node by node basis.



ADVANCED CONFIGURATION/USAGE

If you want your users to be able to configure how the nodes are rendered, you
can click on the "configure filters" tab of the same "Filtered HTML" filter.
Here, in the "Attached Nodes codes" section, you can specify node properties 
that users are allowed to override. A common thing to override is the "title"
property.

Click on the "rearrange filters" tab to set the order in which filters are
applied.  Since the Attached Node filter outputs HTML with line breaks and other
things, you'll probably want this filter to be the last one applied to the output.
If, for instance, the HTML filter is run after the Attached Node filter, then
much of the output could be rearranged or removed.  Just give it a large (heavy)
weight.  The default is 10, which should send it to the bottom of the 
list (where it belongs).


ADVANCED USAGE

The tag format is fairly simple.  The most basic tag would be in the following form:

      [node:<node id>]

Parameters follow the <node id> part and are comma separated name="value" pairs:

      [node:123]
      [node:123,title="Original version of the picture"]

Note that the values must be encased in double quotes.  This is to allow users
to include commas in the value.  The side effect is that double quotes cannot
be used (currently) without causing problems. 

Closing square brackets are not allowed inside the tag.

Thanks to:
Mark Howell for attached_node, on which we are based
Chris Searle for the initial Drupal 6 port

Questions/comments/etc:
Leave them where you found this module
