# CONTENTS OF THIS FILE

* Introduction
* Requirements
* Recommended modules
* Installation
* Configuration
* Maintainers
* Notes


# INTRODUCTION

The **Content Access** module let you content manage access permission in a flexible and transparant way.

It provides two new permissions: *view all* (allows anyone to view the
content) and *view own* (allows only the content creator to see
his/her own content). It also gives access to the existing core
permissions *edit* and *delete* on the same settings page.

It provides the following modalities:

* Each *content type* can have its own default content access settings by role.
* Optionally you can enable role based access control settings per *content node*.
* Access control can be further customized per *user* if you have the **ACL** module enabled.

For more information and reporting, please visit:

* For a description of the module, visit the [project page][1].
* For on-screen documentation, visit the [documentation page][2],
  or enable [**Advanced Help**][6].
* To submit bug reports and feature suggestions, or to track changes
  visit the project's [issue tracker][3].

Features:

* It comes with sensible defaults, so you need not configure anything
  and everything stays working.
* It is as flexible as you want. It can work with per content type
  settings, per content node settings as well as with flexible Access
  Control Lists with the help of the **ACL** module
  ([see Note 1](#ACL) at the end).
* It reuses existing functionality instead of reimplementing it. So
  one can install the **ACL** module and set per user access control
  settings per content node.
* It comes with a submodule named **Content Access Rules
  Interations**. It provides conditions and actions for the **Rules**
  module, which allows one to configure rule-based access permissions.
* It optimizes the written content node grants, so that only the
  necessary grants are written.  This is important for the
  performance of your site.
* The module has a comes with automated testing to ensure everything
  stays working correctly.
* It respects and makes use of Drupal's core permissions. This means
  that the "Access Control" tab provided by this module takes them
  into account and provides you a good overview of *all* applied
  access control settings on a single page (but see [Note 2](#adv) at
  the end).

The module is designed to be simple to use, but can be configured to
provide really fine-grained content access permissions.


# REQUIREMENTS

None.

# RECOMMENDED MODULES

* [**ACL**][4]:
  To use Access Control Lists for per user access control.
* [**Advanced Help Hint**][7]:
  Links help text provided by `hook_help` to online help and
  **Advanced Help**.
* [**Advanced Help**][6]:
  When this module is enabled, the project's `README.md` will be
  displayed when you visit `help/content_access/README.md`.
* [**Markdown**][8]:
  When this module is enabled, display of the project's `README.md`
  will be rendered with the markdown filter.
* [**Node Export**][N]
  To export content access rules.
* [**Rules**][5]:
  To configure rule-based access permissions.


# INSTALLATION

The module is set up to require PHP ver. 5.6 or later.  I do not want
to keep supporting legacy PHP (at least not for free). If you need a
version that runs on something more ancient, you have two options:

1. Keep useing version 7.x-1.2-beta2. It does not restrict PHP
   version. It is very old tho', and is no longer recommended, nor is
   it supported.
2. Ask me to create a separate branch of the project for a backport
   the current supported version to whatever version of PHP you need.
   Then get someone to support this branch.

If you go for the backport option: If you're a developer, request to
become a co-maintainer of this project and then create and support the
backport yourself.  If you're not, sponsor someone to do the backport
and to support it.

To install and enable, do the following:

1. Install as you would normally install a contributed drupal
   module. See: [Installing modules][9] for further information.
2. Enable the **Content Access** module on the *Modules* list
   page.
3. If you want to use access control lists, download, install and
   configure the **ACL** module.


# CONFIGURATION

Note that users need at least the permission "View published content"
to be able to access published content. Furthermore note that content
which is not published is treated in a different way by Drupal: It can
be viewed only by its author or users with "Bypass content access
control" permission.  You can *not* use this project to manage
access to unpublished content.

To inspect and change those permissions, navigate to *Administration »
People » Permisions* and scroll down to the "Node" section.

## Role based access control

To set up access control for a content type, navigate to
*Administration » Structure* and click on "edit" for the content type
you want to set up.  There will be a new tab named "Access Control"
that let you control access.

To set up role based access control, tick the boxes under "Role based
access control settings".  Note that only the "View" permissions are
new permissions provided by this module.  The "Edit" and "Delete"
permissions are provided by the Drupal core, and can also be found if
you navigate to *Administration » People » Permisions*.  They are
shown here to provide the full picture of what permission is set for
the content type and role. It does not matter where you change these.

## Per content node access control

There is a a checkbox to enable per content node access control
settings.  If enabled, a new tab for the content access settings
appears when viewing content.

To configure permission to access these settings, navigate to
*Administration » People » Permisions* and set the "Grant content
access" permission for the relevant roles.

## Advanced access control

The "Advanced" settings are only relevant if you are running multiple
node access modules on a site.

A Drupal node access module can only grant access to content nodes,
but not deny it. So if you are using multiple node access modules,
access will be granted to a node as soon as one of the module grants
access to it.

However you can influence the behaviour by changing the priority of
the content access module as drupal applies *only* the grants with the
highest priority. So if content access has the highest priority
*alone*, only its grants will be applied.

By default node access modules should use priority 0 (zero).


## Using access control lists

To make use of access control lists you'll need to enable per content
node access control settings for the content type. At the access control
tab of such a content node you are able to grant view, edit or delete
permission for specific users..


# MAINTAINERS

**Content Access** was created by [fago][10] (Wolfgang Ziegler).  
It contains a lot of contributions from  [good_man][11] (Khaled Al Hourani).  
The current maintainer is [gisle][12] (Gisle Hannemyr).

Development and maintenance is sponsored by [Hannemyr Nye Medier AS][13].

Any help with development (patches, reviews, comments) are welcome.

# NOTES

**Note 1**<a id="ACL"></a>: ACL integration is not yet working right.

**Note 2**<a id="adv"></a>: Note that this overview can't take other
modules into account, which might also alter node access.  If you have
multiple modules installed that alter node access, read the paragraph
about "Advanced access control".


[1]: https://drupal.org/project/content_access
[2]: https://drupal.org/node/1194974
[3]: https://drupal.org/project/issues/content_access
[4]: https://www.drupal.org/project/acl
[5]: https://www.drupal.org/project/rules
[6]: https://www.drupal.org/project/advanced_help
[7]: https://www.drupal.org/project/advanced_help_hint
[8]: https://www.drupal.org/project/markdown
[N]: https://www.drupal.org/project/node_export
[9]: https://www.drupal.org/docs/7/extend/installing-modules
[10]: https://www.drupal.org/u/fago
[11]: https://www.drupal.org/u/good_man
[12]: https://www.drupal.org/u/gisle
[13]: https://hannemyr.no
