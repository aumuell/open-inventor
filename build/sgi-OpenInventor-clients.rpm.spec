Name: sgi-OpenInventor-clients
Version: 2.1.5
Release: 6
Distribution: Red Hat
Packager: Silicon Graphics, Inc.
Source: oiv.tar.gz

#
# Change the following as appropriate.
#
Summary: Open Inventor Execution Only Environment
Copyright: Silicon Graphics, Inc.
Vendor: SGI
URL: http://oss.sgi.com/projects/inventor/

#
# The setting of the Group tag should be picked from the list
# of values pre-defined by Red Hat in the file /usr/doc/rpm-<version>/groups.
# The value is similar to a directory path (e.g. "Networking/Daemons").
#
Group: Applications/Multimedia

%description
The Open Inventor 3D Toolkit is an object-oriented toolkit that simplifies and
abstracts the task of writing graphics programming into a set of easy to use
objects.  These objects range from low-level data-centered objects such as
Sphere, Camera, Material, Light, and Group, to high-level application-level
objects such as Walk Viewer and Material Editor.  The foundation concept in
Inventor is the "scene database" which defines the objects to be used in an
application.  When using Inventor, a programmer creates, edits, and composes
these objects into hierarchical 3D scene graphs (i.e. database).  A variety of
fundamental application tasks such as rendering, picking, event handling, and
file reading/writing are built-in operations of all objects in the database and
thus are simple to invoke.

This distribution installs shared libraries in /usr/lib, applications in
/usr/bin, and man pages in /usr/man/man1.


%prep
%setup -n inventor

%build
make clobber
make install

%post
type1=/usr/lib/X11/fonts/Type1
dps=/usr/lib/DPS/outline/base
if [ ! -d $type1 ]; then
  echo "Warning: $type1 does not exist"
  exit 0
fi
[ ! -d $dps ] && mkdir -p $dps
for i in $type1/*.pfa; do
  out=$dps`egrep ^/FontName $i | cut -d' ' -f2`
  [ ! -e $out ] && ln -s $i $out
done
exit 0


%files
%attr(755, root, root) /usr/bin/SceneViewer
%attr(755, root, root) /usr/bin/iv2toiv1
%attr(755, root, root) /usr/bin/ivcat
%attr(755, root, root) /usr/bin/ivdowngrade
%attr(755, root, root) /usr/bin/ivfix
%attr(755, root, root) /usr/bin/ivinfo
%attr(755, root, root) /usr/bin/ivview

%attr(755, root, root) /usr/lib/libInventor.so
%attr(755, root, root) /usr/lib/libInventorXt.so

%attr(644, root, root) /usr/man/man1/SceneViewer.1
%attr(644, root, root) /usr/man/man1/inventor.1
%attr(644, root, root) /usr/man/man1/iv2toiv1.1
%attr(644, root, root) /usr/man/man1/ivcat.1
%attr(644, root, root) /usr/man/man1/ivdowngrade.1
%attr(644, root, root) /usr/man/man1/ivfix.1
%attr(644, root, root) /usr/man/man1/ivinfo.1
%attr(644, root, root) /usr/man/man1/ivview.1
