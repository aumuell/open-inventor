Name: sgi-OpenInventor-data
Version: 2.1.5
Release: 6
Distribution: Red Hat
Packager: Silicon Graphics, Inc.
Source: oiv.tar.gz

#
# Change the following as appropriate.
#
Summary: Open Inventor Sample Data
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

This distribution installs data files in /usr/share/data/models.


%prep
%setup -n inventor

%build
export LSUBDIRS=data
make -e install

%files
%attr(-, root, root) %dir /usr/share/data
%attr(-, root, root) %dir /usr/share/data/models
%attr(-, root, root) %dir /usr/share/data/models/CyberHeads
%attr(-, root, root) %dir /usr/share/data/models/buildings
%attr(-, root, root) %dir /usr/share/data/models/chess
%attr(-, root, root) %dir /usr/share/data/models/food
%attr(-, root, root) %dir /usr/share/data/models/furniture
%attr(-, root, root) %dir /usr/share/data/models/geography
%attr(-, root, root) %dir /usr/share/data/models/household
%attr(-, root, root) %dir /usr/share/data/models/machines
%attr(-, root, root) %dir /usr/share/data/models/robots
%attr(-, root, root) %dir /usr/share/data/models/scenes
%attr(-, root, root) %dir /usr/share/data/models/simple
%attr(-, root, root) %dir /usr/share/data/models/sgi
%attr(-, root, root) %dir /usr/share/data/models/toys
%attr(-, root, root) %dir /usr/share/data/models/vehicles

%attr(644, root, root) /usr/share/data/models/README
%attr(644, root, root) /usr/share/data/models/bird.iv
%attr(644, root, root) /usr/share/data/models/chair.iv
%attr(644, root, root) /usr/share/data/models/diamond.iv
%attr(644, root, root) /usr/share/data/models/engine.iv
%attr(644, root, root) /usr/share/data/models/heart.iv
%attr(644, root, root) /usr/share/data/models/moon.iv
%attr(644, root, root) /usr/share/data/models/shamrock.iv
%attr(644, root, root) /usr/share/data/models/shell.iv
%attr(644, root, root) /usr/share/data/models/slotMachine.iv
%attr(644, root, root) /usr/share/data/models/spongetri4.iv
%attr(644, root, root) /usr/share/data/models/star.iv
%attr(644, root, root) /usr/share/data/models/torus.iv
%attr(644, root, root) /usr/share/data/models/wheel.iv

%attr(644, root, root) /usr/share/data/models/CyberHeads/README
%attr(644, root, root) /usr/share/data/models/CyberHeads/josie.iv
%attr(644, root, root) /usr/share/data/models/CyberHeads/paul.iv
%attr(644, root, root) /usr/share/data/models/CyberHeads/rikk.iv

%attr(644, root, root) /usr/share/data/models/buildings/Barcelona.iv
%attr(644, root, root) /usr/share/data/models/buildings/temple.iv
%attr(644, root, root) /usr/share/data/models/buildings/windmill.iv

%attr(644, root, root) /usr/share/data/models/chess/bishop.iv
%attr(644, root, root) /usr/share/data/models/chess/chessboard.iv
%attr(644, root, root) /usr/share/data/models/chess/king.iv
%attr(644, root, root) /usr/share/data/models/chess/knight.iv
%attr(644, root, root) /usr/share/data/models/chess/pawn.iv
%attr(644, root, root) /usr/share/data/models/chess/queen.iv
%attr(644, root, root) /usr/share/data/models/chess/rook.iv

%attr(644, root, root) /usr/share/data/models/food/apple.iv
%attr(644, root, root) /usr/share/data/models/food/banana.iv
%attr(644, root, root) /usr/share/data/models/food/mushroom.iv
%attr(644, root, root) /usr/share/data/models/food/pear.iv

%attr(644, root, root) /usr/share/data/models/furniture/ceilingLamp.iv
%attr(644, root, root) /usr/share/data/models/furniture/table.iv

%attr(644, root, root) /usr/share/data/models/geography/README
%attr(644, root, root) /usr/share/data/models/geography/coasts.iv
%attr(644, root, root) /usr/share/data/models/geography/earth.iv

%attr(644, root, root) /usr/share/data/models/household/canstick.iv
%attr(644, root, root) /usr/share/data/models/household/manillaFolder.iv
%attr(644, root, root) /usr/share/data/models/household/martini.iv
%attr(644, root, root) /usr/share/data/models/household/mug.iv
%attr(644, root, root) /usr/share/data/models/household/outlet.iv
%attr(644, root, root) /usr/share/data/models/household/plant.iv
%attr(644, root, root) /usr/share/data/models/household/plug.iv
%attr(644, root, root) /usr/share/data/models/household/pump.iv
%attr(644, root, root) /usr/share/data/models/household/teapot.iv
%attr(644, root, root) /usr/share/data/models/household/trackLights.iv

%attr(644, root, root) /usr/share/data/models/machines/coarseGear.iv
%attr(644, root, root) /usr/share/data/models/machines/compressor.iv
%attr(644, root, root) /usr/share/data/models/machines/fineGear.iv
%attr(644, root, root) /usr/share/data/models/machines/lathe.iv
%attr(644, root, root) /usr/share/data/models/machines/simpleEngine.iv
%attr(644, root, root) /usr/share/data/models/machines/simpleMonitor.iv
%attr(644, root, root) /usr/share/data/models/machines/vise.iv
%attr(644, root, root) /usr/share/data/models/machines/windTunnel.iv

%attr(644, root, root) /usr/share/data/models/robots/cubeMan.iv
%attr(644, root, root) /usr/share/data/models/robots/cylMan.iv
%attr(644, root, root) /usr/share/data/models/robots/kitMan.iv
%attr(644, root, root) /usr/share/data/models/robots/roboHand.iv
%attr(644, root, root) /usr/share/data/models/robots/robotcop.iv
%attr(644, root, root) /usr/share/data/models/robots/sphereMan.iv

%attr(644, root, root) /usr/share/data/models/scenes/README
%attr(644, root, root) /usr/share/data/models/scenes/chesschairs.env
%attr(644, root, root) /usr/share/data/models/scenes/chesschairs.iv
%attr(644, root, root) /usr/share/data/models/scenes/curtain.iv
%attr(644, root, root) /usr/share/data/models/scenes/glider.env
%attr(644, root, root) /usr/share/data/models/scenes/glider.iv
%attr(644, root, root) /usr/share/data/models/scenes/stagedancers.env
%attr(644, root, root) /usr/share/data/models/scenes/stagedancers.iv

%attr(644, root, root) /usr/share/data/models/sgi/indigo.iv
%attr(644, root, root) /usr/share/data/models/sgi/logo.iv
%attr(644, root, root) /usr/share/data/models/sgi/sgilogo.iv

%attr(644, root, root) /usr/share/data/models/simple/10by10.iv
%attr(644, root, root) /usr/share/data/models/simple/README
%attr(644, root, root) /usr/share/data/models/simple/axis.iv
%attr(644, root, root) /usr/share/data/models/simple/cone.iv
%attr(644, root, root) /usr/share/data/models/simple/cube.iv
%attr(644, root, root) /usr/share/data/models/simple/cylinder.iv
%attr(644, root, root) /usr/share/data/models/simple/faceSet.iv
%attr(644, root, root) /usr/share/data/models/simple/indexedFaceSet.iv
%attr(644, root, root) /usr/share/data/models/simple/indexedTriangleMesh.iv
%attr(644, root, root) /usr/share/data/models/simple/quadMesh.iv
%attr(644, root, root) /usr/share/data/models/simple/sphere.iv
%attr(644, root, root) /usr/share/data/models/simple/star.iv
%attr(644, root, root) /usr/share/data/models/simple/tet.iv
%attr(644, root, root) /usr/share/data/models/simple/text.iv
%attr(644, root, root) /usr/share/data/models/simple/text3.iv
%attr(644, root, root) /usr/share/data/models/simple/texture.iv
%attr(644, root, root) /usr/share/data/models/simple/three.iv
%attr(644, root, root) /usr/share/data/models/simple/tri.iv
%attr(644, root, root) /usr/share/data/models/simple/triangleStripSet.iv

%attr(644, root, root) /usr/share/data/models/toys/cannon.iv
%attr(644, root, root) /usr/share/data/models/toys/dart.iv
%attr(644, root, root) /usr/share/data/models/toys/jackInTheBox.iv
%attr(644, root, root) /usr/share/data/models/toys/lavalamp.iv
%attr(644, root, root) /usr/share/data/models/toys/legoDog.iv
%attr(644, root, root) /usr/share/data/models/toys/top.iv

%attr(644, root, root) /usr/share/data/models/vehicles/767.iv
%attr(644, root, root) /usr/share/data/models/vehicles/aircar.iv
%attr(644, root, root) /usr/share/data/models/vehicles/f16.iv
%attr(644, root, root) /usr/share/data/models/vehicles/fiero.iv
%attr(644, root, root) /usr/share/data/models/vehicles/gunstar.iv
%attr(644, root, root) /usr/share/data/models/vehicles/kittyHawk.iv
%attr(644, root, root) /usr/share/data/models/vehicles/spacestation.iv
%attr(644, root, root) /usr/share/data/models/vehicles/x29.iv
