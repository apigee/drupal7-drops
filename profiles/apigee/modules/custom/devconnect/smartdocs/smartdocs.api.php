<?php

/**
 * @file
 * API documentation for SmartDocs module.
 */

/**
 * Allows a module to take some action immediately after a SmartDocs model
 * or one of its child objects (revision, resource, method) is created,
 * updated or deleted.
 *
 * @param string $model_uuid
 *    The unique identifier for the model being updated.
 */
function hook_smartdocs_model_update($model_uuid) {
  drupal_set_message('Model ' . $model_uuid . ' has just been updated.');
}

/**
 * Allows a module to alter the rendered output of a SmartDocs node right
 * before it is cached and served.
 *
 * @param string $content
 *    The rendered HTML output for the SmartDocs method.
 * @param stdClass $node
 *    The SmartDocs method node which is being rendered.
 */
function hook_smartdocs_method_alter(&$content, stdClass $node) {
  $content = str_replace('%node-title%', $node->title, $content);
}

/**
 * Allows a module to take some action before a method is saved to the Modeling
 * API.
 *
 * @param Apigee\SmartDocs\Method $method
 *   Method that is going to be saved.
 * @param string $model_name
 *   Name of the model which this method belongs to.
 * @param int $revision_uuid
 *   UUID of the affected revision.
 * @param string $resource_uuid
 *   UUID of the affected resource.
 * @param bool $is_update
 *   Indicates whether this is an update or an insert.
 */
function hook_smartdocs_method_presave(Apigee\SmartDocs\Method $method, $model_name, $revision_uuid, $resource_uuid, $is_update) {
  watchdog(__FUNCTION__, '"@method" method is going to be @action.', array('@method' => $method->getDisplayName(), '@action' => $is_update ? t('updated') : t('created')), WATCHDOG_INFO);
}

/**
 * Allows a module to take some action after a method is saved to the Modeling
 * API.
 *
 * @param Apigee\SmartDocs\Method $method
 *   Method that is saved.
 * @param string $model_name
 *   Name of the model which this method belongs to.
 * @param int $revision_uuid
 *   UUID of the affected revision.
 * @param string $resource_uuid
 *   UUID of the affected resource.
 * @param bool $is_update
 *   Indicates whether this was an update or an insert.
 */
function hook_smartdocs_method_postsave(Apigee\SmartDocs\Method $method, $model_name, $revision_uuid, $resource_uuid, $is_update) {
  watchdog(__FUNCTION__, '"@method" method is @action.', array('@method' => $method->getDisplayName(), '@action' => $is_update ? t('updated') : t('created')), WATCHDOG_INFO);
}

/**
 * Allows a module to take some action before a method is deleted from the
 * Modeling API.
 *
 * @param Apigee\SmartDocs\Method $method
 *   Method which is going to be deleted.
 * @param string $model_uuid
 *   UUID of the affected model which the affected method belongs to.
 * @param string $revision_uuid
 *   UUID of the affected revision.
 * @param string $resource_uuid
 *   UUID of the affected resource.
 */
function hook_smartdocs_method_predelete(Apigee\SmartDocs\Method $method, $model_uuid, $revision_uuid, $resource_uuid) {
}

/**
 * Allows a module to take some action after a method is deleted from the
 * Modeling API.
 *
 * @param Apigee\SmartDocs\Method $method
 *   Method that has just been deleted.
 * @param string $model_uuid
 *   UUID of the model, which the affected method belonged to.
 * @param string $revision_uuid
 *   UUID of the affected revision.
 * @param string $resource_uuid
 *   UUID of the affected resource.
 */
function hook_smartdocs_method_postdelete(Apigee\SmartDocs\Method $method, $model_uuid, $revision_uuid, $resource_uuid) {
}

/**
 * Allows a module to take some action before a method node is saved to Drupal.
 *
 * @param stdClass $node
 *   smart_method node object which is going to be saved.
 * @param array $model
 *   Array representation of the model to which the method belongs.
 * @param array $revision
 *   Array representation of the revision to which the method belongs.
 * @param array $resource
 *   Array representation of the resource to which the method is attached.
 * @param array $method
 *   Array representation of the method itself.
 */
function hook_smartdocs_method_node_presave(stdClass $node, array $model, array $revision, array $resource, array $method) {
  $node->title = $model['displayName'] . ': ' . $node->title;
}

/**
 * Allows a module to take some action after a method node is saved to Drupal.
 *
 * @param stdClass $node
 *   smart_method node object which is has just been saved.
 * @param array $model
 *   Array representation of the model to which the method belongs.
 * @param array $revision
 *   Array representation of the revision to which the method belongs.
 * @param array $resource
 *   Array representation of the resource to which the method is attached.
 * @param array $method
 *   Array representation of the method itself.
 */
function hook_smartdocs_method_node_postsave(stdClass $node, array $model, array $revision, array $resource, array $method) {
  $node->title = $model['displayName'] . ': ' . $node->title;
}

/**
 * Allows a module to take some action before a method is deleted from the
 * Modeling API, in cases in which the corresponding node will NOT be deleted.
 *
 * @param Apigee\SmartDocs\Method $method
 *   Method object to which the affected node corresponds.
 * @param int $nid
 *   Nid of the affected method node.
 * @param string $model_uuid
 *   UUID of the model to which the affected method belongs.
 * @param string $revision_uuid
 *   UUID of the revision to which the affected method belongs.
 * @param string $resource_uuid
 *   UUID of the resource to which the affected method belongs.
 * @param string $action
 *   Either 'keep' or 'unpublish'.
 */
function hook_smartdocs_method_node_preaction(Apigee\SmartDocs\Method $method, $nid, $model_uuid, $revision_uuid, $resource_uuid, $action) {
}

/**
 * Allows a module to take some action after a method is deleted from the
 * Modeling API, in cases in which the corresponding node will NOT be deleted.
 *
 * @param Apigee\SmartDocs\Method $method
 *   Method object to which the affected node corresponds.
 * @param int $nid
 *   Nid of the affected method node.
 * @param string $model_uuid
 *   UUID of the model to which the affected method belongs.
 * @param string $revision_uuid
 *   UUID of the revision to which the affected method belongs.
 * @param string $resource_uuid
 *   UUID of the resource to which the affected method belongs.
 * @param string $action
 *   Either 'keep' or 'unpublish'.
 */
function hook_smartdocs_method_node_postaction(Apigee\SmartDocs\Method $method, $nid, $model_uuid, $revision_uuid, $resource_uuid, $action) {
}

/**
 * Allows a module to take some action before a method is deleted from the
 * Modeling API, in cases in which the corresponding node WILL BE deleted.
 *
 * @param Apigee\SmartDocs\Method $method
 *   Method object to which the affected node corresponds.
 * @param int $nid
 *   Nid of the affected method node.
 * @param string $model_uuid
 *   UUID of the model to which the affected method belongs.
 * @param string $revision_uuid
 *   UUID of the revision to which the affected method belongs.
 * @param string $resource_uuid
 *   UUID of the resource to which the affected method belongs.
 */
function hook_smartdocs_method_node_predelete($method, $nid, $model_uuid, $revision_uuid, $resource_uuid) {
}

/**
 * Allows a module to take some action after a method is deleted from the
 * Modeling API, in cases in which the corresponding node WILL BE deleted.
 *
 * @param Apigee\SmartDocs\Method $method
 *   Method object to which the affected node corresponds.
 * @param int $nid
 *   Nid of the affected method node.
 * @param string $model_uuid
 *   UUID of the model to which the affected method belongs.
 * @param string $revision_uuid
 *   UUID of the revision to which the affected method belongs.
 * @param string $resource_uuid
 *   UUID of the resource to which the affected method belongs.
 */
function hook_smartdocs_method_node_postdelete($method, $nid, $model_uuid, $revision_uuid, $resource_uuid) {
}

/**
 * Allows a module to take some action before a model is saved to the Modeling
 * API. This action may include modifying the model itself.
 *
 * @param Apigee\SmartDocs\Model $model
 *   Model which is going to be saved.
 */
function hook_smartdocs_model_presave(Apigee\SmartDocs\Model $model) {
  $model->setDisplayName('Demo: ' . $model->getDisplayName());
}

/**
 * Allows a module to take some action after a model has been saved to the
 * Modeling API.
 *
 * @param Apigee\SmartDocs\Model $model
 *   Model which is has just been saved.
 */
function hook_smartdocs_model_postsave(Apigee\SmartDocs\Model $model) {
  watchdog(__FUNCTION__, '"@model" model is successfully saved.', array('@model' => $model->getDisplayName()), WATCHDOG_INFO);
}

/**
 * Allows a module to take some action before a model is deleted from the
 * Modeling API.
 *
 * @param string $model_name
 *   Machine name of the model which is going to be deleted.
 */
function smartdocs_model_predelete($model_name) {
  watchdog(__FUNCTION__, '@model is going to be deleted.', array('@model' => $model_name), WATCHDOG_INFO);
}

/**
 * Allows a module to take some action after a model is deleted from the
 * Modeling API.
 *
 * @param string $model_name
 *   Machine name of the model which has just been deleted.
 */
function smartdocs_model_postdelete($model_name) {
  watchdog(__FUNCTION__, '@model is deleted.', array('@model' => $model_name), WATCHDOG_INFO);
}

/**
 * Allows a module to take some action before a model's template is saved to the
 * Modeling API. This may include altering the template's content.
 *
 * @param string $model_name
 *   Name of the affected model.
 * @param string $template_content
 *   Raw template content.
 */
function hook_smartdocs_template_presave($model_name, &$template_content) {
  $template_content .= '<p><strong>' . t('Powered by Apigee!') . '</strong></p>';
}

/**
 * Allows a module to take some action after a model's template is saved to the
 * Modeling API.
 *
 * @param string $model_name
 *   Name of the model affected model.
 * @param string $template_content
 *   Raw template content.
 */
function hook_smartdocs_template_postsave($model_name, $template_content) {
  watchdog(__FUNCTION__, "Model's (@model) template successfully updated.", array('@model' => $model_name), WATCHDOG_INFO);
}

/**
 * Allows a module to take some action after a model's template is reverted to
 * the default.
 *
 * @param string $model_name
 *   Name of the model affected model.
 */
function hook_smartdocs_template_reverted($model_name) {
  watchdog(__FUNCTION__, "Model's (@model) template successfully reverted.", array('@model' => $model_name), WATCHDOG_INFO);
}

/**
 * Allows a module to take some action after a model is successfully imported.
 *
 * @param string $raw_source
 *   Raw content which was imported.
 * @param string $mime_type
 *   Content type of the file content. One from:  application/{json, xml, yml}.
 * @param string $document_format
 *   Document format of the file content. One from: apimodel, swagger, wadl.
 * @param Apigee\SmartDocs\Model $model
 *   Affected model.
 * @param string $source
 *   Either 'url' or 'file'.
 */
function hook_smartdocs_model_import($raw_source, $mime_type, $document_format, Apigee\SmartDocs\Model $model, $source) {
}

/**
 * Allows a module to take some action before resources are saved to a model's
 * revision.
 *
 * @param Apigee\SmartDocs\Model $model
 *   Affected model.
 * @param Apigee\SmartDocs\Revision $revision
 *   Affected revision of the model.
 * @param Apigee\SmartDocs\Resource[] $resources
 *   Array of resources which are going to be imported.
 */
function hook_smartdocs_import_revision_resources_presave(Apigee\SmartDocs\Model $model, Apigee\SmartDocs\Revision $revision, array $resources) {
}

/**
 * Allows a module to take some action after resources are saved to a model's
 * revision.
 *
 * @param Apigee\SmartDocs\Model $model
 *   Affected model.
 * @param Apigee\SmartDocs\Revision $revision
 *   Affected revision of the model.
 * @param Apigee\SmartDocs\Resource[] $resources
 *   Array of resources which are going to be imported.
 */
function hook_smartdocs_import_revision_resources_postsave(Apigee\SmartDocs\Model $model, Apigee\SmartDocs\Revision $revision, array $resources) {
}

/**
 * Allows a module to take some action before methods are saved to a model's
 * revision.
 *
 * @param Apigee\SmartDocs\Model $model
 *   Affected model.
 * @param Apigee\SmartDocs\Revision $revision
 *   Affected revision of the model.
 * @param Apigee\SmartDocs\Method[] $methods
 *   Array of methods which are going to be imported.
 */
function hook_smartdocs_import_revision_methods_presave(Apigee\SmartDocs\Model $model, Apigee\SmartDocs\Revision $revision, array $methods) {
}

/**
 * Allows a module to take some action after methods are saved to a model's
 * revision.
 *
 * @param Apigee\SmartDocs\Model $model
 *   Affected model.
 * @param Apigee\SmartDocs\Revision $revision
 *   Affected revision of the model.
 * @param Apigee\SmartDocs\Method[] $methods
 *   Array of methods which have been imported.
 */
function hook_smartdocs_import_revision_methods_postsave(Apigee\SmartDocs\Model $model, Apigee\SmartDocs\Revision $revision, array $methods) {
}

/**
 * Allows a module to take some action before a resource is saved to a model's
 * revision.
 *
 * @param Apigee\SmartDocs\Resource $resource
 *   Resource which is going to be imported.
 * @param Apigee\SmartDocs\Revision $revision
 *   Affected revision of the model.
 * @param Apigee\SmartDocs\Model $model
 *   Affected model.
 * @param bool $is_update
 *   Indicates whether this is an update or an insert.
 */
function hook_smartdocs_resource_presave(Apigee\SmartDocs\Resource $resource, Apigee\SmartDocs\Revision $revision, Apigee\SmartDocs\Model $model, $is_update) {
}

/**
 * Allows a module to take some action after a resource is saved to a model's
 * revision.
 *
 * @param Apigee\SmartDocs\Resource $resource
 *   Resource which has just been imported.
 * @param Apigee\SmartDocs\Revision $revision
 *   Affected revision of the model.
 * @param Apigee\SmartDocs\Model $model
 *   Affected model.
 * @param bool $is_update
 *   Indicates whether it was an update or a create.
 */
function hook_smartdocs_resource_postsave(Apigee\SmartDocs\Resource $resource, Apigee\SmartDocs\Revision $revision, Apigee\SmartDocs\Model $model, $is_update) {
}

/**
 * Allows a module to take some action before a resource is deleted from a
 * model's revision.
 *
 * @param Apigee\SmartDocs\Resource $resource
 *   Resouce which is going to be deleted.
 * @param string $revision_uuid
 *   UUID of the revision to which this resource belongs.
 * @param string $model_name
 *   Name of the model to which this revision and resource belong.
 */
function hook_smartdocs_resource_predelete(Apigee\SmartDocs\Resource $resource, $revision_uuid, $model_name) {
}

/**
 * Allows a module to take some action after a resource is deleted from a
 * model's revision.
 *
 * @param Apigee\SmartDocs\Resource $resource
 *   Resource that has just been deleted.
 * @param string $revision_uuid
 *   UUID of the revision to which this resource belonged.
 * @param string $model_name
 *   Name of the model to which this resource belonged.
 */
function hook_smartdocs_resource_postdelete(Apigee\SmartDocs\Resource $resource, $revision_uuid, $model_name) {
}

/**
 * Allows a module to take some action before a model's template auth scheme is
 * saved to the Modeling API.
 *
 * @param Apigee\SmartDocs\TemplateAuth $template_auth
 *   Affected template auth.
 * @param Apigee\SmartDocs\Security\TemplateAuthScheme $scheme
 *   Scheme which is going to be saved.
 * @param string $model_uuid
 *   UUID of the affected model to which the affected template auth belongs.
 * @param bool $is_update
 *   Indicates whether it is an update or a create.
 */
function hook_smartdocs_template_auth_scheme_presave(Apigee\SmartDocs\TemplateAuth $template_auth, Apigee\SmartDocs\Security\TemplateAuthScheme $scheme, $model_uuid, $is_update) {
}

/**
 * Allows a module to take some action after a model's template auth scheme is
 * saved to the Modeling API.
 *
 * @param Apigee\SmartDocs\TemplateAuth $template_auth
 *   Affected template auth.
 * @param Apigee\SmartDocs\Security\TemplateAuthScheme $scheme
 *   Scheme which has been saved.
 * @param string $model_uuid
 *   UUID of the affected model to which the affected template auth belongs.
 * @param bool $is_update
 *   Indicates whether it was an update or a create.
 */
function hook_smartdocs_template_auth_scheme_postsave(Apigee\SmartDocs\TemplateAuth $template_auth, Apigee\SmartDocs\Security\TemplateAuthScheme $scheme, $model_uuid, $is_update) {
}

/**
 * Allows a module to take some action before a model's template auth scheme is
 * deleted from the Modeling API.
 *
 * @param string $scheme_name
 *   Name of the scheme which is going to be deleted.
 * @param string $model_uuid
 *   UUID of the model to which this security scheme belongs.
 */
function hook_smartdocs_template_auth_scheme_predelete($scheme_name, $model_uuid) {
}

/**
 * Allows a module to take some action after a model's template auth scheme is
 * deleted from the Modeling API.
 *
 * @param string $scheme_name
 *   Name of the scheme that was deleted.
 * @param string $model_uuid
 *   UUID of the model to which this security scheme belonged.
 */
function hook_smartdocs_template_auth_scheme_postdelete($scheme_name, $model_uuid) {
}

/**
 * Allows a module to alter security scheme definition before it is saved to
 * Modeling API.
 *
 * @param array $payload
 *   Security scheme definition.
 */
function hook_smartdocs_model_security_scheme_alter(array &$payload) {
}

/**
 * Allows a module to take some action before a revision's security scheme is
 * saved to the Modeling API.
 *
 * @param Apigee\SmartDocs\Security\SecurityScheme $scheme
 *   Security scheme which is going to be saved.
 * @param Apigee\SmartDocs\Model $model
 *   Affected model to which this security scheme corresponds.
 * @param Apigee\SmartDocs\Revision $revision
 *   Affected revision of the model.
 * @param bool $is_update
 *   Indicates whether it is an update or a create.
 */
function hook_smartdocs_model_security_scheme_presave(Apigee\SmartDocs\Security\SecurityScheme $scheme, Apigee\SmartDocs\Model $model, Apigee\SmartDocs\Revision $revision, $is_update) {
}

/**
 * Allows a module to take some action after a revision's security scheme is
 * saved to the Modeling API.
 *
 * @param Apigee\SmartDocs\Security\SecurityScheme $scheme
 *   Security scheme which is going to be saved.
 * @param Apigee\SmartDocs\Model $model
 *   Affected model to which this security scheme corresponds.
 * @param Apigee\SmartDocs\Revision $revision
 *   Affected revision of the model.
 * @param bool $is_update
 *   Indicates whether it was an update or a create.
 */
function hook_smartdocs_model_security_scheme_postsave(Apigee\SmartDocs\Security\SecurityScheme $scheme, Apigee\SmartDocs\Model $model, Apigee\SmartDocs\Revision $revision, $is_update) {
}

/**
 * Allows a module to take some action before a revision's security scheme is
 * deleted from the Modeling API.
 *
 * @param Apigee\SmartDocs\Security\SecurityScheme $scheme
 *   Security scheme which is going to be deleted.
 * @param string $model_uuid
 *   UUID of the model to which this security scheme belongs.
 * @param string $revision_uuid
 *   UUID of the affected revision of the model.
 */
function hook_smartdocs_security_predelete(Apigee\SmartDocs\Security\SecurityScheme $scheme, $model_uuid, $revision_uuid) {
}

/**
 * Allows a module to take some action after a revision's security scheme is
 * deleted from the Modeling API.
 *
 * @param Apigee\SmartDocs\Security\SecurityScheme $scheme
 *   Security scheme that has just been deleted.
 * @param string $model_uuid
 *   UUID of the model to which this security scheme belongs.
 * @param string $revision_uuid
 *   UUID of the affected revision of the model.
 */
function hook_smartdocs_security_postdelete(Apigee\SmartDocs\Security\SecurityScheme $scheme, $model_uuid, $revision_uuid) {
}

/**
 * Allows a module to take some action before a revision is saved to the
 * Modeling API.
 *
 * @param Apigee\SmartDocs\Revision $revision
 *   Revision of the model.
 * @param Apigee\SmartDocs\Model $model
 *   Affected model.
 */
function hook_smartdocs_revision_presave(Apigee\SmartDocs\Revision $revision, Apigee\SmartDocs\Model $model) {
}

/**
 * Allows a module to take some action after a revision is saved to the
 * Modeling API.
 *
 * @param Apigee\SmartDocs\Revision $revision
 *   Revision of the model that has just been saved.
 * @param Apigee\SmartDocs\Model $model
 *   Affected model.
 */
function hook_smartdocs_revision_postsave(Apigee\SmartDocs\Revision $revision, Apigee\SmartDocs\Model $model) {
}
