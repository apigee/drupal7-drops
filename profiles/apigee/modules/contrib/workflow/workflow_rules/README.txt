WORKFLOW NODE
=============
When using the conventional 'Workflow Node API', Rules should be triggered upon
the workflow-specific 'transition post' event.

WORKFLOW FIELD
==============
As of Workflow 7.x-2.0, the module Workflow Rules supports Workflow Field API.
This is used when you add a new 'Workflow' field to an entity.
When using the new 'Workflow Field API', Rules should be triggered upon
the general 'content updated' event.
This is because the workflow-specific code is now called DURING an update,
and is not used after the commit has ben executed.
