/*
 *
 *  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved. 
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 *  Mountain View, CA  94043, or:
 * 
 *  http://www.sgi.com 
 * 
 *  For further information regarding this notice, see: 
 * 
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */


#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoNormalElement.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoTextureCoordinateBindingElement.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoVertexShape.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoNonIndexedShape.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPackedColor.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoNurbsSurface.h>
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoIndexedNurbsSurface.h>
#include <Inventor/nodes/SoIndexedNurbsCurve.h>
#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/SbColor.h>
#include <Inventor/SbDict.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>

void purgeOldStuff(SoSeparator *);
void convertAllVertexShapes(SoSeparator *);
void untangleInstances(SoSeparator *);
SoCallbackAction::SoCallbackActionCB convertVertexShape;
void splitGraph( int refpath, int copypath, SoPathList pathList);
void removeInstances(SoSeparator *root, SoPathList& vertexPathList);
SoPathList instancedPathList;
SbBool doCopy, saveNormals;

//Counters for work done:
int numDeleted, numVPInserted, numCopied;

static void
print_usage(const char *progname)
{
    (void)fprintf(stderr, "Usage: %s [-bch] [-o filename] file ...\n",
		  progname);
    (void)fprintf(stderr, "-b : Output binary format (default ASCII)\n");
    (void)fprintf(stderr, "-o : Write output to [filename]\n");
    (void)fprintf(stderr, "-n : Keep unused normal vectors\n");
    (void)fprintf(stderr, "-c : Make copies of instanced nodes\n");
    (void)fprintf(stderr, "-h : This message (help)\n");
    (void)fprintf(stderr, "If given a filename of '-' or if not given\n");
    (void)fprintf(stderr, "any filenames, standard input will be read\n");
    (void)fprintf(stderr, "converts vertex shapes in input to use\n");
    (void)fprintf(stderr, "Vertex Property nodes, unless they are instanced\n"); 
    (void)fprintf(stderr, "If lighting is off, does not use normals \n");
    (void)fprintf(stderr, "unless option -n is specified.\n");
    (void)fprintf(stderr, "Requires Inventor 2.1\n");
    exit(99);
}

static int
parse_args(int argc, char **argv, char **outfilename)

{
    int err = 0;	// Flag: error in options?
    int c;
    int result=0;	// 0 = ASCII, 1 = BINARY
    doCopy =  saveNormals = FALSE;
    
    while ((c = getopt(argc, argv, "bctno:h")) != -1) {
	switch(c) {
	  case 'b':
	    result = 1;
	    break;
	  case 'c':
	    doCopy = TRUE;
	    break;	
	  case 'n':
	    saveNormals = TRUE;
	    break;
	  case 'o':
	    *outfilename = optarg;
	    break;
	  case 'h':	// Help
	  default:
	    err = 1;
	    break;
	}
    }

    if (err) {
	print_usage(argv[0]);
    }

    return result;
}


int
main(int argc, char **argv)
{
    numDeleted = 0;
    numVPInserted = 0;
    numCopied = 0;
    SoInteraction::init();

    // Parse arguments
    char *outputfile = NULL;
    int binary = 0;

    binary = parse_args(argc, argv, &outputfile);

    // read stuff:
    SoInput in;
    SoSeparator *root;

    if (optind == argc) {
	++argc;	            // Act like one argument "-" was given
	argv[optind] = "-";
    }
    for (; optind < argc; optind++) {
	char *filename = argv[optind];

	if (strcmp(filename, "-") == 0) {
	    if (isatty(fileno(stdin))) {
		fprintf(stderr, "Trying to read from standard input, ");
		fprintf(stderr, "but standard input is a tty!\n");
		print_usage(argv[0]);
	    }
	    in.setFilePointer(stdin);
	   
	}
	else if (in.openFile(filename) == FALSE) {
	    fprintf(stderr, "Could not open file %s\n", filename);
	}
	
    }

    root = SoDB::readAll(&in);
    root->ref();
	
    
    //Now convert the VertexShape nodes:   
    convertAllVertexShapes(root);
    
    
    //Delete the unnecessary nodes from the graph:
    purgeOldStuff(root);

    
    // write stuff
    SoOutput out;
    out.setBinary(binary);
    if (outputfile == NULL) {
	out.setFilePointer(stdout);
    }
    else {
	if (out.openFile(outputfile) == FALSE) {
	    fprintf(stderr, "Couldn't open %s for writing\n",
		    outputfile);
	    print_usage(argv[0]);
	}
    }
    SoWriteAction writer(&out);

    
    writer.apply((SoNode*)root);
    
    if (outputfile != NULL) out.closeFile();
    fprintf(stderr, "Number of Vertex Property nodes inserted: %d\n", 
	numVPInserted);
    if (numCopied)fprintf(stderr, "Number of instanced nodes copied: %d\n",
	 numCopied);
    fprintf(stderr, "Number of property nodes deleted: %d\n", numDeleted);    
    return 0;
}

///////////////////////////////////////////////////////////////////////////
//
// Description:
//
// This callback function converts VertexShape nodes to use the vertexProperty
// field.
//
////////////////////////////////////////////////////////////////////////////

SoCallbackAction::Response
convertVertexShape(void *, SoCallbackAction *action, const SoNode *node)
{
    SoVertexShape* vertShape = (SoVertexShape *)node;
    if (vertShape->vertexProperty.getValue() != NULL)
	return SoCallbackAction::ABORT;

    SoState* state = action->getState();
    SoVertexProperty *vp = new SoVertexProperty;
    vertShape->vertexProperty.setValue(vp);
    numVPInserted++;
    int startIndex = 0;
    // other indices start at zero unless binding is PER_VERTEX:
    int startMtlIndex = 0;
    int startNrmIndex = 0;
    int startTxtIndex = 0;
    
    if (vertShape->isOfType(SoNonIndexedShape::getClassTypeId())){
	startIndex = ((SoNonIndexedShape*)vertShape)->startIndex.getValue();
	((SoNonIndexedShape*)vertShape)->startIndex.setValue(0);
    }
    
    //Put the current coordinates into the vertex property node, if 3D
    const SoCoordinateElement* ce = SoCoordinateElement::getInstance(state);
    if (ce->is3D() ){
	vp->vertex.setValues(0, ce->getNum() - startIndex, &ce->get3(startIndex));
       	
    }
    
    //If it is a triangle strip set, line set, or point set, or face set,
    //and if numVertices == -1, we need to count the vertices.
    if (vertShape->isOfType(SoTriangleStripSet::getClassTypeId())){
	SoTriangleStripSet* tss = (SoTriangleStripSet*)vertShape;
	int last = tss->numVertices.getNum()-1;
	if (tss->numVertices[last] == -1){
	    // Count the vertices that are specified in numVertices field
	    int totvert=0;
	    for (int i=0; i< last ; i++){
		totvert+= tss->numVertices[i];
	    }
	    int32_t totleft = ce->getNum() - startIndex - totvert;
	    tss->numVertices.set1Value(last, totleft);	    
	}
	
    }
    if (vertShape->isOfType(SoFaceSet::getClassTypeId())){
	SoFaceSet* fs = (SoFaceSet*)vertShape;
	int last = fs->numVertices.getNum()-1;
	if (fs->numVertices[last] == -1){
	    // Count the vertices that are specified in numVertices field
	    int totvert=0;
	    for (int i=0; i< last ; i++){
		totvert+= fs->numVertices[i];
	    }
	    int32_t totleft = ce->getNum() - startIndex - totvert;
	    fs->numVertices.set1Value(last, totleft);	    
	}
	
    }
       
    if (vertShape->isOfType(SoLineSet::getClassTypeId())){
	SoLineSet* ls = (SoLineSet*)vertShape;
	int last = ls->numVertices.getNum()-1;
	if (ls->numVertices[last] == -1){
	    // Count the vertices that are specified in numVertices field
	    int totvert=0;
	    for (int i=0; i< last ; i++){
		totvert+= ls->numVertices[i];
	    }
	    int32_t totleft = ce->getNum() - startIndex - totvert;
	    ls->numVertices.set1Value(last, totleft);		    
	}
	
    }
    if (vertShape->isOfType(SoPointSet::getClassTypeId())){
	SoPointSet* ps = (SoPointSet*)vertShape;
	if (ps->numPoints.getValue() == -1){
	    int32_t totleft = ce->getNum() - startIndex;
	    ps->numPoints.setValue(totleft);	 
	}
	
    }
    //If there are multiple diffuse colors in the state, or the
    //materialBinding is not OVERALL, put them in
    //the vertex property node.
    SoLazyElement* le = SoLazyElement::getInstance(state);
    int32_t mb = (int32_t) SoMaterialBindingElement::get(state);
    
    if (mb == SoMaterialBindingElement::PER_VERTEX){
	startMtlIndex = startIndex;
    }
    
    if (le->getNumDiffuse() > 1 || (mb != SoMaterialBindingElement::OVERALL)){      
	vp->materialBinding.setValue(mb);
	if (le->isPacked()){
	
	    vp->orderedRGBA.setValues(0, le->getNumDiffuse() - startMtlIndex, 
		SoLazyElement::getPackedColors(state) + startMtlIndex );
	}
	else {
	    for (int i= startMtlIndex ; i< le->getNumDiffuse(); i++){
		float transp;
		if (i<le->getNumTransparencies()) 
		    transp = SoLazyElement::getTransparency(state, i);
		else transp = 0.0;
		vp->orderedRGBA.set1Value(i-startMtlIndex, 
		    (SoLazyElement::getDiffuse(state, i)).getPackedValue(transp));
	    }
	}
    }
    
    //If lighting is on, and there are normals in the state, 
    // or if it is specified to keep normals, put 
    // the normals and normal binding into the
    //vertex property node.
    if (saveNormals || 
	    SoLazyElement::getLightModel(state) == SoLazyElement::PHONG){
	const SoNormalElement* ne = SoNormalElement::getInstance(state);
	SoNormalBinding::Binding nb = 
	    (SoNormalBinding::Binding)SoNormalBindingElement::get(state);
	if (ne != NULL && ne->getNum()>0){
	    vp->normalBinding.setValue(nb);
	
	    if (nb == SoNormalBinding::PER_VERTEX){
		startNrmIndex = startIndex;
	    }
	    vp->normal.setValues(0, (ne->getNum())-startNrmIndex, 
		&ne->get(startNrmIndex));    
	}

    }
     
    // If there are texture coordinates in the state,
    //put them into the vertex property node.
     
    
    SoTextureCoordinateBinding::Binding tcb = 
	(SoTextureCoordinateBinding::Binding)
	    SoTextureCoordinateBindingElement::get(state);
    if (tcb == SoTextureCoordinateBinding::PER_VERTEX)
	startTxtIndex = startIndex;
    const SoTextureCoordinateElement* tce = 
	SoTextureCoordinateElement::getInstance(state);
    if (tce->getType() == SoTextureCoordinateElement::EXPLICIT &&
	(tce->getNum() > 0 )){
	vp->texCoord.setValues(0, tce->getNum() - startTxtIndex, 
	    &tce->get2(startTxtIndex));
    }
 
 
    
    return SoCallbackAction::ABORT;
}
/////////////////////////////////////////////////////////////////////////////
//  Description:
//    This function removes nodes and diffuse colors from the scene graph if
//    they are not necessary after the conversion to using vertexProperty nodes.
//
//
//
//////////////////////////////////////////////////////////////////////////////
void
purgeOldStuff(SoSeparator* root)
{
    SoPathList nonVertexPathList;
    //Make a pathlist of all paths to non-vertex shapes:
    // cube,sphere,cone,cylinder,nurb,text2,text3, unknown shape
    // we will need to be sure to keep the property nodes these shapes
    // depend on.
    SoSearchAction searchAction;
    searchAction.setInterest(SoSearchAction::ALL);
    searchAction.setSearchingAll(TRUE);
    searchAction.setType(SoShape::getClassTypeId());
    searchAction.apply(root);
    nonVertexPathList = searchAction.getPaths();
    
    // delete the vertex-based shapes from this list so we will be left with:
    // cube,sphere,cone,cylinder,nurb,text2,text3.
    searchAction.setType(SoVertexShape::getClassTypeId());
    searchAction.apply(root);
    int i;
    for (i=searchAction.getPaths().getLength()-1; i>= 0; i--)
    {
        int index = nonVertexPathList.findPath(*(searchAction.getPaths()[i]));
	if (index >= 0){
	    nonVertexPathList.remove(index);
	}
	else fprintf(stderr, "Error, shape not found %d\n", index);
    }
    //If we avoided converting instanced vertex-shapes, add this list to
    //the current pathlist.  That way if these depend on property nodes, 
    //we won't be deleting those property nodes:
    
    for (i = 0; i<instancedPathList.getLength(); i++){
	nonVertexPathList.append(instancedPathList[i]);
    }
       
    //Make pathlists  of all potentially deletable property nodes--
    //one for material nodes, one for coord nodes, and one for normals
    //and one for vertex property nodes that are scene graph nodes.
    //These will be pruned, based on nonvertex shapes depending on them. 
    //with the remaining being property nodes that
    //can be deleted 
    
    //SoCoordinate3:
    SoPathList coordPathList;
    searchAction.setType(SoCoordinate3::getClassTypeId());
    searchAction.apply(root);
    coordPathList = searchAction.getPaths();
    
    //SoTextureCoordinate2:    
    searchAction.setType(SoTextureCoordinate2::getClassTypeId());
    searchAction.apply(root);
    for (i = searchAction.getPaths().getLength()-1; i>=0; i--){
	coordPathList.append(searchAction.getPaths()[i]);
    }
    
    //SoNormalBinding:
    searchAction.setType(SoNormalBinding::getClassTypeId());
    searchAction.apply(root);
    SoPathList normalPathList = searchAction.getPaths(); 
    
    //SoNormal:
    searchAction.setType(SoNormal::getClassTypeId());
    searchAction.apply(root);
    for (i = searchAction.getPaths().getLength()-1; i>=0; i--){
        normalPathList.append(searchAction.getPaths()[i]);
    }
    
    //SoVertexProperty:
    searchAction.setType(SoVertexProperty::getClassTypeId());
    searchAction.apply(root);
    SoPathList VPPathList = searchAction.getPaths();       
    
    //Make pathlists of material nodes--
    //paths to SoMaterial, SoPackedColor, BaseColor, SoMaterialBinding.
    //Multiple colors can be deleted if nonvertex shapes don't depend on them 
    //but single colors and overall binding must remain.
    
    searchAction.setType(SoMaterial::getClassTypeId());
    searchAction.apply(root);
    SoPathList materialPathList = searchAction.getPaths();
    
    searchAction.setType(SoMaterialBinding::getClassTypeId());
    searchAction.apply(root);
    SoPathList mBindPathList = searchAction.getPaths();
    
    searchAction.setType(SoPackedColor::getClassTypeId());
    searchAction.apply(root);
    SoPathList packedPathList = searchAction.getPaths();
    
    searchAction.setType(SoBaseColor::getClassTypeId());
    searchAction.apply(root);
    SoPathList baseColorPathList = searchAction.getPaths();
    
    //create a search action for getting last node in path:
    SoSearchAction searchLastNode;
    searchLastNode.setInterest(SoSearchAction::LAST);
    searchLastNode.setSearchingAll(TRUE);
    SoPath* foundPath;

    // now we go through all the nonVertexPathList, finding the last
    // property node that affects the nonVertex shape.  These we will have
    // to keep.    
    //for each path in nonVertexPathList:
    for (i = nonVertexPathList.getLength()-1; i>=0; i--){
	SoPath *path = nonVertexPathList[i];
		
	//find last SoMaterial influencing path
	searchLastNode.setType(SoMaterial::getClassTypeId());
	searchLastNode.apply(path);
	foundPath = searchLastNode.getPath();
	if (foundPath){
	    //delete this from the material path list:
	    int index = materialPathList.findPath(*foundPath);
	    if (index >= 0)
		materialPathList.remove(index);
	    // if we didn't find it, it must have been previously deleted.	     	
	}
	
	//find last SoMaterialBinding influencing path
	searchLastNode.setType(SoMaterialBinding::getClassTypeId());
	searchLastNode.apply(path);
	foundPath = searchLastNode.getPath();
	if (foundPath){
	    //delete this from the material binding path list:
	    int index = mBindPathList.findPath(*foundPath);
	    if (index >= 0)
		mBindPathList.remove(index);	    	
	}
	
	//find last SoPackedColor influencing path
	searchLastNode.setType(SoPackedColor::getClassTypeId());
	searchLastNode.apply(path);
	foundPath = searchLastNode.getPath();
	if (foundPath){
	    //delete this from the packed color path list:
	    int index = packedPathList.findPath(*foundPath);
	    if (index >= 0)
		packedPathList.remove(index);	
	}
		
	//find last SoBaseColor influencing path
	searchLastNode.setType(SoBaseColor::getClassTypeId());
	searchLastNode.apply(path);
	foundPath = searchLastNode.getPath();
	if (foundPath){
	    //delete this from the basecolor path list:
	    int index = baseColorPathList.findPath(*foundPath);
	    if (index >= 0)
		baseColorPathList.remove(index);	    	
	}
	
	//find last SoVertexProperty influencing path
	searchLastNode.setType(SoVertexProperty::getClassTypeId());
	searchLastNode.apply(path);
	foundPath = searchLastNode.getPath();
	if(foundPath) {
	    //delete this from the vertexProperty pathlist:
	    int index = VPPathList.findPath(*foundPath);
	    if (index >= 0)
		VPPathList.remove(index);
	}
	
	//If the path is to SoSphere,SoCube,SoCylinder,SoText3,SoText2,
	//we don't need to look further:
	SoNode *node = path->getTail();
	if (node->isOfType(SoSphere::getClassTypeId()) ||
	    node->isOfType(SoCube::getClassTypeId()) ||
	    node->isOfType(SoCylinder::getClassTypeId()) ||
	    node->isOfType(SoText3::getClassTypeId()) ||
	    node->isOfType(SoText2::getClassTypeId()))
		continue;
		
	//If this is a NURBS or unknown shape, it may use coordinate nodes:
		
	//find last SoCoordinate3 influencing path
	searchLastNode.setType(SoCoordinate3::getClassTypeId());
	searchLastNode.apply(path);
	foundPath = searchLastNode.getPath();
	if (foundPath){
	    //delete this from the deletion path list:
	    int index = coordPathList.findPath(*foundPath);
	    if (index >= 0)
		coordPathList.remove(index);	    	
	}	
	
	//find last SoTextureCoordinate2 influencing path
	searchLastNode.setType(SoTextureCoordinate2::getClassTypeId());
	searchLastNode.apply(path);
	foundPath = searchLastNode.getPath();
	if (foundPath){
	     //delete this from the deletion path list:
	    int index = coordPathList.findPath(*foundPath);
	    if (index >= 0)
		coordPathList.remove(index);
	}	    	
	
	//If the path is to NURBS,
	//we don't need to look further:	
	if (node->isOfType(SoNurbsCurve::getClassTypeId()) ||
	    node->isOfType(SoNurbsSurface::getClassTypeId()) ||
	    node->isOfType(SoIndexedNurbsCurve::getClassTypeId()) ||
	    node->isOfType(SoIndexedNurbsSurface::getClassTypeId())) 	 
		continue;
		
	//If this is unknown shape, it may use normals or normal binding:	
	//find last SoNormal influencing path
	searchLastNode.setType(SoNormal::getClassTypeId());
	searchLastNode.apply(path);
	foundPath = searchLastNode.getPath();
	if (foundPath){
	    //delete this from the normal path list:
	    int index = normalPathList.findPath(*foundPath);
	    if (index >= 0)
		normalPathList.remove(index);	    	
	}	
		
	//find last SoNormalBinding influencing path
	searchLastNode.setType(SoNormalBinding::getClassTypeId());
	searchLastNode.apply(path);
	foundPath = searchLastNode.getPath();
	if (foundPath){
	    //delete this from the normal path list:
	    int index = normalPathList.findPath(*foundPath);
	    if (index >= 0)
		normalPathList.remove(index);	    	
	}	
    }
	
    //Now use these path lists to delete nodes from the graph
    //Or, with material nodes, to remove stuff from the graph that
    //vertex shapes won't need.
    // First, remove normals and normal binding: 
    for (i = normalPathList.getLength()-1; i>= 0; i--){
	SoNode* node = normalPathList[i]->getTail();
	SoGroup* parent = (SoGroup*)(normalPathList[i]->getNodeFromTail(1));
	//Make sure parent exists (may have been dereferenced)
	if (parent == NULL) continue;
	//Find the node (it may not still be there, due to instancing)
	int index = parent->findChild(node);
	if (index >=0 ){
	    parent->removeChild(index);
	    numDeleted++;
	}
    }
     
    // then, remove coordinates, texture coords
    for (i = coordPathList.getLength()-1; i>= 0; i--){
	SoNode* node = coordPathList[i]->getTail();
	SoGroup* parent = (SoGroup*)(coordPathList[i]->getNodeFromTail(1));
	if (parent == NULL) continue;
	int index = parent->findChild(node);
	if (index >= 0){
	     parent->removeChild(index);
	     numDeleted++;
	}
    }  
    // material binding can be removed if it is not OVERALL.
    // packed and base_color nodes can be removed if they have multiple colors.
    // diffuse color fields can be emptied if they have >1 color. 
    
    for (i = packedPathList.getLength()-1; i>= 0; i--){
	SoPackedColor* node = (SoPackedColor*)(packedPathList[i]->getTail());
	if ( node->orderedRGBA.getNum() > 1 ){
	    SoGroup* parent = (SoGroup*)(packedPathList[i]->getNodeFromTail(1));
	    if (parent == NULL) continue;	
	    int index = parent->findChild(node);
	    if (index >= 0){ 
		parent->removeChild(index);
		numDeleted++;
	    }
	}
    }  
    
    for (i = baseColorPathList.getLength()-1; i>= 0; i--){
	SoBaseColor* node = (SoBaseColor*)(baseColorPathList[i]->getTail());
	if ( node->rgb.getNum() > 1 ){
	    SoGroup* parent = (SoGroup*)(baseColorPathList[i]->getNodeFromTail(1));
	    if (parent == NULL) continue;
	    int index = parent->findChild(node);
	    if (index >= 0) {
		parent->removeChild(index);
		numDeleted++;
	    }
	}
    } 
     
    for (i = mBindPathList.getLength()-1; i>= 0; i--){
	SoMaterialBinding* node = (SoMaterialBinding*)(mBindPathList[i]->getTail());
	if ( node->value.getValue() != SoMaterialBinding::OVERALL){
	    SoGroup* parent = (SoGroup*)(mBindPathList[i]->getNodeFromTail(1));
	    if (parent == NULL) continue;
	    int index = parent->findChild(node);
	    if (index >= 0){
		 parent->removeChild(index);
		 numDeleted++;
	    }
	}
    }  

    // Delete excess (>1) diffuse colors from material nodes:    
    for (i = materialPathList.getLength()-1; i>= 0; i--){
	SoMaterial* node = (SoMaterial*)(materialPathList[i]->getTail());
	if (node && node->diffuseColor.getNum() > 1){
	    node->diffuseColor.deleteValues(0, -1);	
	}
    }
    
    //delete all stuff (except possibly exactly 1 diffuse color) from
    //vertexProperty nodes in scene graph that are still in VPPathList.
    for (i = VPPathList.getLength()-1; i>= 0; i--){
	SoVertexProperty* node = (SoVertexProperty*)(VPPathList[i]->getTail());
	//If there is exactly 1 diffuse color, and the material binding
	//is OVERALL, we need to keep it.  Everything else can go.
	if( node && (node->orderedRGBA.getNum()== 1) &&
	    (node->materialBinding.getValue() == SoVertexProperty::OVERALL)){
		node->normal.deleteValues(0, -1);
		node->texCoord.deleteValues(0, -1);
		node->vertex.deleteValues(0, -1);
	    }
	else {
	    SoGroup* parent = (SoGroup*)(VPPathList[i]->getNodeFromTail(1));
	    if (parent == NULL) continue;
	    int index = parent->findChild(node);
	    if (index >= 0){
		parent->removeChild(index);
		numDeleted++;
	    }
	}		
    }  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Description:  find all vertex shapes in graph, convert them to
//  use the vertexProperty node.  If multiply instanced, make copies of
//  subsequent instances, or ignore instanced things.
//
//////////////////////////////////////////////////////////////////////////////
void
convertAllVertexShapes(SoSeparator *root)
{ 
    //Construct a pathlist of all paths to vertex shapes
    SoPathList vertexPathList;
    SoSearchAction searchAction;
    searchAction.setInterest(SoSearchAction::ALL);
    searchAction.setSearchingAll(TRUE);
    searchAction.setType(SoVertexShape::getClassTypeId());    
           
    //Go through each path in pathlist, replacing instances with copies,
    //but only do this if doCopy was specified:
    
    if (doCopy) untangleInstances(root);
           
    //Now build the pathlist using the new scene graph:
     
    searchAction.apply(root);
    vertexPathList = searchAction.getPaths();
    
    //If doCopy was not specified, we have to remove all paths to
    // instanced nodes in VertexPathList, and put them into instancedPathList.
    if (!doCopy) {
	instancedPathList.truncate(0);
	removeInstances(root, vertexPathList);
    }
     
    //Now go through and convert the tail nodes of the paths:
    SoCallbackAction cbAction;
    cbAction.addPreTailCallback(convertVertexShape, NULL);
    int* indexArray = NULL;
    for (int i = 0; i< vertexPathList.getLength();  i++){
        //If there are SoSwitch nodes in the path, and they don't
	//follow the path, temporarily modify them before doing the
	//conversion:
	int pathlen = vertexPathList[i]->getLength();
	//Look for switches in the path:
	for (int j= 0; j< pathlen-1; j++){	    
	    if ((vertexPathList[i]->getNode(j))->
		    isOfType(SoSwitch::getClassTypeId())){
		SoSwitch* sw = (SoSwitch*)vertexPathList[i]->
			getNode(j);
		// identify if the switch doesn't follow the path:
		if ((sw->whichChild.getValue() != -3 )&&
			(sw->whichChild.getValue() != 
			vertexPathList[i]->getIndex(j+1))){
		    if (indexArray == NULL) {
			indexArray = new int[pathlen];
			for (int k=0;k<pathlen;k++)
			    indexArray[k] = -100;
		    }
		    //save the old switch value in indexArray:
		    indexArray[j] = sw->whichChild.getValue();
		    sw->whichChild.setValue(vertexPathList[i]->
			    getIndex(j+1));  			
		}
	    }
	}
		    	
	//Traverse the path, convert the tail node
	cbAction.apply(vertexPathList[i]);
	
	// if switches were changed, reset to old values:
	if (indexArray != NULL){
	    for (int j= 0; j<pathlen-1; j++){
		if (indexArray[j] != -100){
		    SoSwitch* swtch = (SoSwitch*) vertexPathList[i]->
			    getNode(j);
		    swtch->whichChild.setValue(indexArray[j]);		    
		}		
	    }
	delete [] indexArray;
	indexArray = NULL;
	}
    }	   
   
}
////////////////////////////////////////////////////////////////////////////
//
// Description:  Search a graph for instanced vertex shapes.
//  Whenever one is found split the graph by making copies of all 
//  instanced nodes in the paths above the two nodes.
//  Repeatedly do this until all instances have been removed.
//
///////////////////////////////////////////////////////////////////////////
void
untangleInstances(SoSeparator *root)
{ 

    //Construct a pathlist of all paths to vertex shapes
    SoPathList vertexPathList;
    SoSearchAction searchAction;
    searchAction.setInterest(SoSearchAction::ALL);
    searchAction.setSearchingAll(TRUE);
    searchAction.setType(SoVertexShape::getClassTypeId());
    //Create a dictionary to look for instanced nodes
    SbDict* nodeDict = new SbDict;
    //Repeatedly apply this search until graph is untangled.
    //The search action has to be repeated each time a node is de-instanced,
    //because the original search action is not valid after the splitting.
    while(1){
	SbBool done = TRUE;
	searchAction.apply(root);
	vertexPathList = searchAction.getPaths();
    
	//Clear the dictionary
	nodeDict->clear();
     
	//Go through each path in pathlist, looking for instancing
      
	for (int i = 0; i< vertexPathList.getLength();  i++){
	    SoVertexShape* node = (SoVertexShape*)(vertexPathList[i]->getTail());
	
	    // see if it's been previously instanced:	
	    uint32_t key = (uint32_t)(node->getNodeId());
	    void *value;
	    if (!nodeDict->find(key, value)){	
		nodeDict->enter(key, (void*)i);
	    }

	    else{
		// an instance was found, split the graph there:
		int matchvalue = (long)value;
		splitGraph(matchvalue, i, vertexPathList);
		done = FALSE;
		break;		
	    }
	}
	if (done) return;
    }
    
}
	

////////////////////////////////////////////////////////////////////////////
//
// Description:  Search a graph for instanced vertex shapes.
//  Whenever one is found, put its path into instancedPathList. 
//  Then go through inputPathList, and remove all of these instanced
//  paths from it.
//
///////////////////////////////////////////////////////////////////////////
void
removeInstances(SoSeparator *root, SoPathList& inputPathList)
{ 

    //Construct a pathlist of all paths to vertex shapes
    SoPathList vertexPathList;
    SoSearchAction searchAction;
    searchAction.setInterest(SoSearchAction::ALL);
    searchAction.setSearchingAll(TRUE);
    searchAction.setType(SoVertexShape::getClassTypeId());
    //Create a dictionary to look for instanced nodes,
    //And a dictionary to identify nodes that have been put into instanced list:
    SbDict* nodeDict = new SbDict;
    SbDict* instDict = new SbDict;
 
    searchAction.apply(root);
    vertexPathList = searchAction.getPaths();
    
    //Clear the dictionaries
    nodeDict->clear();
    instDict->clear();
     
    //Go through each path in pathlist, looking for instanced nodes
      
    int i;
    for (i = 0; i< vertexPathList.getLength();  i++){
	SoVertexShape* node = (SoVertexShape*)(vertexPathList[i]->getTail());
	
	// see if it's been previously instanced:	
	uint32_t key = (uint32_t)(node->getNodeId());
	void *value;
	if (!nodeDict->find(key, value)){	
	    nodeDict->enter(key, (void*)i);
	}

	else{
	    // an instance was found
	    // we always put the new one "i" in instanced list.
	    // we conditionally put the matching one in if it's not
	    // already there:
	    int matchvalue = (long)value;
	    if (!instDict->find(key, value)){
		instDict->enter(key, (void*)i);
		instancedPathList.append(vertexPathList[matchvalue]);
	    }
	    instancedPathList.append(vertexPathList[i]);	 
	}		
    }
    
    //Now remove every one of these paths from the inputPathList
    for (i=0; i<instancedPathList.getLength(); i++){
	int index = inputPathList.findPath(*(instancedPathList[i]));
	if( index < 0 ) fprintf(stderr, "Error,  instanced path not found");
	else {
	    inputPathList.remove(index);
	}
    }
    
}
    
//////////////////////////////////////////////////////////////////////////////
//
// Description:
//   Given two paths, refpath and copypath in a pathList, 
//   which is rooted at root, and which have the same
//   ending node: Make copies of all nodes in path 2 that are instanced
//   from path1, producing a new scene graph.  
//    
/////////////////////////////////////////////////////////////////////////
void
splitGraph(int refpath, int copypath, SoPathList pathList)
{
    //First, find how far from the tail of path2 is there instancing.
    //If the same group node occurs in both paths, it is not necessarily
    //instanced; it could be that two of its children are instanced.
    //The instancing ends at the first node from the bottom which is
    //either not the same in both paths, or which has two different
    //child indices for the next lower node
    
    int reflen = pathList[refpath]->getLength();
    int copylen = pathList[copypath]->getLength();

    // find the furthest position (up from the end) where
    // the two paths are the same.  We will have to copy that node.
  
    int j;
    for (j= 0; (j<copylen-1)&& (j<reflen-1); j++){	
	if (pathList[refpath]->getNodeFromTail(j) !=
	    pathList[copypath]->getNodeFromTail(j)) break;
	if ((pathList[refpath]->getIndexFromTail(j)) !=
	    pathList[copypath]->getIndexFromTail(j))
		{ j++; break;}		
    }
    if (j >= copylen)
	fprintf(stderr, "Error, paths are identical\n");
    if (j == 0) 
	fprintf(stderr, "Error, instanced node not found\n");
	    
    int firstmatch = j-1;
	    
    //Make copies at the node where they meet:
    SoGroup* parent = 
	(SoGroup*)(pathList[copypath]->getNodeFromTail(firstmatch+1));
    if (parent == NULL) {
	fprintf(stderr, "Error, no parent for instances\n");
     }  
    SoNode* node = (SoNode*)(pathList[copypath]->getNodeFromTail(firstmatch));
    SoNode* newNode = node->copy(TRUE);
	    
    //Replace node with newNode in the scene graph:	 
    int index = pathList[copypath]->getIndexFromTail(firstmatch);
  
    parent->replaceChild(index, newNode);
    numCopied++;
    return;
}
	
	
