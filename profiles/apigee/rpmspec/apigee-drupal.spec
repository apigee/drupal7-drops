# Disable automatic requires/provides processing
AutoReqProv: no

%define drupaldir /var/www/html
Name: apigee-drupal
Version:  %{?VERSION}
Release:  %{?BUILD_NUMBER}
Summary: An open-source content-management platform

Group: Applications/Publishing
License: GPLv2+ and BSD and MIT
URL: http://www.drupal.org
Source0: apigee-drupal.tar.gz
Source1: %{name}.conf
Source3: %{name}-cron
Source6: %{name}.attr
Source7: %{name}.prov
Source9: %{name}.prov.rpm-lt-4-9-compat
Source10: %{name}.req
Source11: %{name}.req.rpm-lt-4-9-compat


BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}-7.x-%{version}.%{release}-root-%(%{__id_u} -n)
Requires: php, php-gd, php-mbstring, php-mcrypt, php-mysql, php-pecl-apc, php-xml
Requires: crontabs

# Virtual provides
## Core
Provides: apigee-drupal(core) = %{version}
## Modules
Provides: apigee-drupal(aggregator) = %{version}
Provides: apigee-drupal(block) = %{version}
Provides: apigee-drupal(blog) = %{version}
Provides: apigee-drupal(book) = %{version}
Provides: apigee-drupal(color) = %{version}
Provides: apigee-drupal(comment) = %{version}
Provides: apigee-drupal(contact) = %{version}
Provides: apigee-drupal(contextual) = %{version}
Provides: apigee-drupal(dashboard) = %{version}
Provides: apigee-drupal(dblog) = %{version}
Provides: apigee-drupal(field_sql_storage) = %{version}
Provides: apigee-drupal(field_ui) = %{version}
Provides: apigee-drupal(field) = %{version}
Provides: apigee-drupal(file) = %{version}
Provides: apigee-drupal(filter) = %{version}
Provides: apigee-drupal(forum) = %{version}
Provides: apigee-drupal(help) = %{version}
Provides: apigee-drupal(image) = %{version}
Provides: apigee-drupal(list) = %{version}
Provides: apigee-drupal(locale) = %{version}
Provides: apigee-drupal(menu) = %{version}
Provides: apigee-drupal(node) = %{version}
Provides: apigee-drupal(number) = %{version}
Provides: apigee-drupal(openid) = %{version}
Provides: apigee-drupal(options) = %{version}
Provides: apigee-drupal(overlay) = %{version}
Provides: apigee-drupal(path) = %{version}
Provides: apigee-drupal(php) = %{version}
Provides: apigee-drupal(poll) = %{version}
Provides: apigee-drupal(rdf) = %{version}
Provides: apigee-drupal(search) = %{version}
Provides: apigee-drupal(shortcut) = %{version}
Provides: apigee-drupal(simpletest) = %{version}
Provides: apigee-drupal(statistics) = %{version}
Provides: apigee-drupal(syslog) = %{version}
Provides: apigee-drupal(system) = %{version}
Provides: apigee-drupal(taxonomy) = %{version}
Provides: apigee-drupal(text) = %{version}
Provides: apigee-drupal(toolbar) = %{version}
Provides: apigee-drupal(tracker) = %{version}
Provides: apigee-drupal(translation) = %{version}
Provides: apigee-drupal(trigger) = %{version}
Provides: apigee-drupal(update) = %{version}
Provides: apigee-drupal(user) = %{version}
## Themes
Provides: apigee-drupal(bartik) = %{version}
Provides: apigee-drupal(garland) = %{version}
Provides: apigee-drupal(seven) = %{version}
Provides: apigee-drupal(stark) = %{version}
## Profiles
Provides: apigee-drupal(apigee) = %{version}
Provides: apigee-drupal(minimal) = %{version}
Provides: apigee-drupal(standard) = %{version}

%description
Equipped with a powerful blend of features, Drupal is a Content Management
System written in PHP that can support a variety of websites ranging from
personal weblogs to large community-driven websites.  Drupal is highly
configurable, skinnable, and secure.

%package rpmbuild
Summary: Rpmbuild files for %{name}
Group:   Development/Tools

%description rpmbuild
%{summary}.

%prep
%setup -q -n apigee-drupal


chmod -x scripts/drupal.sh
chmod -x scripts/password-hash.sh
chmod -x scripts/run-tests.sh

%build

%install
rm -rf %{buildroot}
install -d %{buildroot}%{drupaldir}
cp -pr * %{buildroot}%{drupaldir}
cp -pr .htaccess %{buildroot}%{drupaldir}
mkdir --mode=777 -p %{buildroot}%{drupaldir}/sites/default/{files,tmp,private}
cp -p sites/default/default.settings.php %{buildroot}%{drupaldir}/sites/default/settings.php
chmod 666 %{buildroot}%{drupaldir}/sites/default/settings.php

# rpmbuild
# RPM >= 4.9
%if 0%{?_fileattrsdir:1}
mkdir -p %{buildroot}%{_sysconfdir}/rpm/
mkdir -p %{buildroot}%{_prefix}/lib/rpm/fileattrs
install -pm0644 %{SOURCE6} %{buildroot}%{_prefix}/lib/rpm/fileattrs/%{name}.attr
install -pm0755 %{SOURCE7} %{buildroot}%{_prefix}/lib/rpm/%{name}.prov
install -pm0755 %{SOURCE10} %{buildroot}%{_prefix}/lib/rpm/%{name}.req
# RPM < 4.9
%else
mkdir -p %{buildroot}%{_sysconfdir}/rpm/
mkdir -p %{buildroot}%{_prefix}/lib/rpm/
install -pm0755 %{SOURCE9} %{buildroot}%{_prefix}/lib/rpm/%{name}.prov
install -pm0755 %{SOURCE11} %{buildroot}%{_prefix}/lib/rpm/%{name}.req
%endif

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%doc CHANGELOG.txt INSTALL* LICENSE* MAINTAINERS.txt UPGRADE.txt sites/README.txt 
%doc COPYRIGHT.txt README.txt
%{drupaldir}
%exclude %{drupaldir}/CHANGELOG.txt
%exclude %{drupaldir}/INSTALL*
%exclude %{drupaldir}/LICENSE*
%exclude %{drupaldir}/MAINTAINERS.txt
%exclude %{drupaldir}/UPGRADE.txt
%exclude %{drupaldir}/COPYRIGHT.txt
%exclude %{drupaldir}/README.txt


%files rpmbuild
%defattr(-,root,root,-)
%{?_fileattrsdir:%{_prefix}/lib/rpm/fileattrs/%{name}.attr}
%{_prefix}/lib/rpm/%{name}.prov
%{_prefix}/lib/rpm/%{name}.req

%changelog
*Mon Nov 20 2013 Tom Stovall <stovak@apigee.comu> 7.x-4.24
*Mon Sep 23 2013 Tom Stovall <stovak@apigee.comu> 7.x-4.23.94
-item 2
