Name: sgi-OpenInventor
Version: 2.1.5
Release: 6
Distribution: Red Hat
Packager: Silicon Graphics
Source: oiv.tar.gz

#
# Change the following as appropriate
#
Summary: Open Inventor - a toolkit for writing 3d programs
Copyright: Silicon Graphics, Inc.
Vendor: SGI
URL: http://www.sgi.com
#
# The setting of the Group tag should be picked from the list
# of values pre-defined by Red Hat in the file /usr/doc/rpm-<version>/groups.
# The value is similar to a directory path (e.g. "Networking/Daemons").
#
Group: Libraries
%description
       The Open Inventor 3d Toolkit is an object-oriented toolkit
       that simplifies and abstracts the task of writing graphics
       programming  into  a  set  of  easy to use objects.  These
       objects range from low level data-centered objects such as
       Sphere,  Camera, Material, Light, and Group, to high level
       application-level objects such as Walkthrough  Viewer  and
       Material  Editor.   The  foundation concept in Inventor is
       the "scene database" which defines the objects to be  used
       in an application.  When using Inventor, a programmer cre­
       ates, edits, and composes these objects into  hierarchical
       3d  scene graphs (i.e., database).  A variety of fundamen­
       tal application tasks such as  rendering,  picking,  event
       handling, and file reading/writing are built-in operations
       of all objects in the database  and  thus  are  simple  to
       invoke.


%prep

%setup -n inventor

%build
make clobber
make install

%install

mkdir -p /usr/lib/DPS/outline/base
ln -s -f /usr/lib/X11/fonts/Type1/UTRG____.pfa /usr/lib/DPS/outline/base/Utopia-Regular

%postun

%files
%attr(-, root, root) %dir /usr/bin
%attr(-, root, root) %dir /usr/lib
%attr(-, root, root) %dir /usr/man/man1

/usr/bin/SceneViewer
/usr/bin/iv2toiv1
/usr/bin/ivcat
/usr/bin/ivdowngrade
/usr/bin/ivfix
/usr/bin/ivinfo
/usr/bin/ivquicken
/usr/bin/ivview
/usr/lib/libInventor.so
/usr/lib/libInventorXt.so
/usr/lib/libFL.so
/usr/man/man1/SceneViewer.1
/usr/man/man1/iv2toiv1.1
/usr/man/man1/ivcat.1
/usr/man/man1/ivdowngrade.1
/usr/man/man1/ivfix.1
/usr/man/man1/ivinfo.1
/usr/man/man1/ivview.1

%attr(-, root, root) %dir /usr/lib/DPS/outline/base/Utopia-Regular
