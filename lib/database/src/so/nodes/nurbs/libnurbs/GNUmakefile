IVDEPTH = ../../../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../../../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include  -I../head  -DLIBRARYBUILD  -DNDEBUG -DDEBUG

CXXFILES = \
 	arc.c++ \
	arcsorter.c++ \
	arctess.c++ \
	backend.c++ \
	basiccrveval.c++ \
	basicsurfeval.c++ \
	bin.c++ \
	bufpool.c++ \
	cachingeval.c++ \
	ccw.c++ \
	coveandtiler.c++ \
	curve.c++ \
	curvelist.c++ \
	curvesub.c++ \
	displaylist.c++ \
	flist.c++ \
	flistsorter.c++ \
	hull.c++ \
	intersect.c++ \
	knotvector.c++ \
	mapdesc.c++ \
	mapdescv.c++ \
	maplist.c++ \
	mesher.c++ \
	monotonizer.c++ \
	mycode.c++ \
	nurbsinterfac.c++ \
	nurbstess.c++ \
	patch.c++ \
	patchlist.c++ \
	quilt.c++ \
	reader.c++ \
	renderhints.c++ \
	slicer.c++ \
	sorter.c++ \
	splitarcs.c++ \
	subdivider.c++ \
	tobezier.c++ \
	trimline.c++ \
	trimregion.c++ \
	trimvertpool.c++ \
	uarray.c++ \
	varray.c++

all:: $(OBJECTS)

include $(COMMONRULES)
