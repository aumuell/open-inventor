Name: sgi-OpenInventor-data
Version: 2.1.5
Release: 6
Distribution: Red Hat
Packager: Silicon Graphics

#
# Change the following as appropriate
#
Summary: Open Inventor data - example models
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


%files
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


#/usr/share/data/models/Banana.iv
#/usr/share/data/models/Pear.iv
/usr/share/data/models/README
#/usr/share/data/models/SgiLogo.iv
#/usr/share/data/models/X29.iv
/usr/share/data/models/bird.iv
/usr/share/data/models/chair.iv
/usr/share/data/models/diamond.iv
/usr/share/data/models/engine.iv
/usr/share/data/models/heart.iv
/usr/share/data/models/moon.iv
/usr/share/data/models/shamrock.iv
/usr/share/data/models/shell.iv
/usr/share/data/models/slotMachine.iv
/usr/share/data/models/spongetri4.iv
/usr/share/data/models/star.iv
/usr/share/data/models/torus.iv
/usr/share/data/models/wheel.iv

/usr/share/data/models/CyberHeads/README
/usr/share/data/models/CyberHeads/josie.iv
/usr/share/data/models/CyberHeads/paul.iv
/usr/share/data/models/CyberHeads/rikk.iv

/usr/share/data/models/buildings/Barcelona.iv
/usr/share/data/models/buildings/temple.iv
/usr/share/data/models/buildings/windmill.iv

/usr/share/data/models/chess/bishop.iv
/usr/share/data/models/chess/chessboard.iv
/usr/share/data/models/chess/king.iv
/usr/share/data/models/chess/knight.iv
/usr/share/data/models/chess/pawn.iv
/usr/share/data/models/chess/queen.iv
/usr/share/data/models/chess/rook.iv

/usr/share/data/models/food/apple.iv
/usr/share/data/models/food/banana.iv
/usr/share/data/models/food/mushroom.iv
/usr/share/data/models/food/pear.iv

/usr/share/data/models/furniture/ceilingLamp.iv
/usr/share/data/models/furniture/table.iv

/usr/share/data/models/geography/coasts.iv
/usr/share/data/models/geography/earth.iv

/usr/share/data/models/household/canstick.iv
/usr/share/data/models/household/manillaFolder.iv
/usr/share/data/models/household/martini.iv
/usr/share/data/models/household/mug.iv
/usr/share/data/models/household/outlet.iv
/usr/share/data/models/household/plant.iv
/usr/share/data/models/household/plug.iv
/usr/share/data/models/household/pump.iv
/usr/share/data/models/household/teapot.iv
/usr/share/data/models/household/trackLights.iv

/usr/share/data/models/machines/coarseGear.iv
/usr/share/data/models/machines/compressor.iv
/usr/share/data/models/machines/fineGear.iv
/usr/share/data/models/machines/lathe.iv
/usr/share/data/models/machines/simpleEngine.iv
/usr/share/data/models/machines/simpleMonitor.iv
/usr/share/data/models/machines/vise.iv
/usr/share/data/models/machines/windTunnel.iv

/usr/share/data/models/robots/cubeMan.iv
/usr/share/data/models/robots/cylMan.iv
/usr/share/data/models/robots/kitMan.iv
/usr/share/data/models/robots/roboHand.iv
/usr/share/data/models/robots/robotcop.iv
/usr/share/data/models/robots/sphereMan.iv

/usr/share/data/models/scenes/README
/usr/share/data/models/scenes/chesschairs.env
/usr/share/data/models/scenes/chesschairs.iv
/usr/share/data/models/scenes/curtain.iv
/usr/share/data/models/scenes/glider.env
/usr/share/data/models/scenes/glider.iv
/usr/share/data/models/scenes/stagedancers.env
/usr/share/data/models/scenes/stagedancers.iv

/usr/share/data/models/sgi/indigo.iv
/usr/share/data/models/sgi/logo.iv
/usr/share/data/models/sgi/sgilogo.iv

/usr/share/data/models/simple/10by10.iv
/usr/share/data/models/simple/README
/usr/share/data/models/simple/axis.iv
/usr/share/data/models/simple/cone.iv
/usr/share/data/models/simple/cube.iv
/usr/share/data/models/simple/cylinder.iv
/usr/share/data/models/simple/faceSet.iv
/usr/share/data/models/simple/indexedFaceSet.iv
/usr/share/data/models/simple/indexedTriangleMesh.iv
/usr/share/data/models/simple/quadMesh.iv
/usr/share/data/models/simple/sphere.iv
/usr/share/data/models/simple/star.iv
/usr/share/data/models/simple/tet.iv
/usr/share/data/models/simple/text.iv
/usr/share/data/models/simple/text3.iv
/usr/share/data/models/simple/texture.iv
/usr/share/data/models/simple/three.iv
/usr/share/data/models/simple/tri.iv
/usr/share/data/models/simple/triangleStripSet.iv

/usr/share/data/models/toys/cannon.iv
/usr/share/data/models/toys/dart.iv
/usr/share/data/models/toys/jackInTheBox.iv
/usr/share/data/models/toys/lavalamp.iv
/usr/share/data/models/toys/legoDog.iv
/usr/share/data/models/toys/top.iv

/usr/share/data/models/vehicles/767.iv
/usr/share/data/models/vehicles/aircar.iv
/usr/share/data/models/vehicles/f16.iv
/usr/share/data/models/vehicles/fiero.iv
/usr/share/data/models/vehicles/gunstar.iv
/usr/share/data/models/vehicles/kittyHawk.iv
/usr/share/data/models/vehicles/spacestation.iv
/usr/share/data/models/vehicles/x29.iv
