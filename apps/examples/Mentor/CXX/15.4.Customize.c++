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

/*----------------------------------------------------------------
 *  This is an example from the Inventor Mentor.
 *  chapter 15, example 4.
 *
 *  Same as chapter 15, example 2, with one difference:
 *  The draggers are customized to use different geometry.
 *  We create our own scene graphs for the parts "translator"
 *  and "translatorActive."
 *  Then we call setPart() to replace these two parts with our 
 *  new scene graphs. (Remember, draggers are derived from 
 *  nodekits, so it's easy to change the parts).
 *----------------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/nodekits/SoShapeKit.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTransform.h>

int
main(int , char **argv)
{
   Widget myWindow = SoXt::init(argv[0]);
   if (myWindow == NULL) exit(1);

   SoSeparator *root = new SoSeparator;
   root->ref();

   // Create 3 translate1Draggers and place them in space.
   SoSeparator *xDragSep = new SoSeparator;
   SoSeparator *yDragSep = new SoSeparator;
   SoSeparator *zDragSep = new SoSeparator;
   root->addChild(xDragSep);
   root->addChild(yDragSep);
   root->addChild(zDragSep);
   // Separators will each hold a different transform
   SoTransform *xDragXf = new SoTransform;
   SoTransform *yDragXf = new SoTransform;
   SoTransform *zDragXf = new SoTransform;
   xDragXf->set("translation  0 -4 8");
   yDragXf->set("translation -8  0 8 rotation 0 0 1  1.57");
   zDragXf->set("translation -8 -4 0 rotation 0 1 0 -1.57");
   xDragSep->addChild(xDragXf);
   yDragSep->addChild(yDragXf);
   zDragSep->addChild(zDragXf);

   // Add the draggers under the separators, after transforms
   SoTranslate1Dragger *xDragger = new SoTranslate1Dragger;
   SoTranslate1Dragger *yDragger = new SoTranslate1Dragger;
   SoTranslate1Dragger *zDragger = new SoTranslate1Dragger;
   xDragSep->addChild(xDragger);
   yDragSep->addChild(yDragger);
   zDragSep->addChild(zDragger);

/////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

   // Create myTranslator and myTranslatorActive.
   // These are custom geometry for the draggers.
   SoSeparator *myTranslator = new SoSeparator;
   SoSeparator *myTranslatorActive = new SoSeparator;
   myTranslator->ref();
   myTranslatorActive->ref();
   // Materials for the dragger in regular and active states
   SoMaterial *myMtl = new SoMaterial;
   SoMaterial *myActiveMtl = new SoMaterial;
   myMtl->diffuseColor.setValue(1,1,1);
   myActiveMtl->diffuseColor.setValue(1,1,0);
   myTranslator->addChild(myMtl);
   myTranslatorActive->addChild(myActiveMtl);
   // Same shape for both versions.
   SoCube  *myCube = new SoCube;
   myCube->set("width 3 height .4 depth .4");
   myTranslator->addChild(myCube);
   myTranslatorActive->addChild(myCube);

   // Now, customize the draggers with the pieces we created.
   xDragger->setPart("translator",myTranslator);
   xDragger->setPart("translatorActive",myTranslatorActive);
   yDragger->setPart("translator",myTranslator);
   yDragger->setPart("translatorActive",myTranslatorActive);
   zDragger->setPart("translator",myTranslator);
   zDragger->setPart("translatorActive",myTranslatorActive);

// CODE FOR The Inventor Mentor ENDS HERE
/////////////////////////////////////////////////////////////

   // Create shape kit for the 3D text
   // The text says 'Slide Cubes To Move Me'
   SoShapeKit *textKit = new SoShapeKit;
   root->addChild(textKit);
   SoText3 *myText3 = new SoText3;
   textKit->setPart("shape", myText3);
   myText3->justification = SoText3::CENTER;
   myText3->string.set1Value(0,"Slide Cubes");
   myText3->string.set1Value(1,"To");
   myText3->string.set1Value(2,"Move Me");
   textKit->set("font { size 2}");
   textKit->set("material { diffuseColor 1 1 0}");

   // Create shape kit for surrounding box.
   // It's an unpickable cube, sized as (16,8,16)
   SoShapeKit *boxKit = new SoShapeKit;
   root->addChild(boxKit);
   boxKit->setPart("shape", new SoCube);
   boxKit->set("drawStyle { style LINES }");
   boxKit->set("pickStyle { style UNPICKABLE }");
   boxKit->set("material { emissiveColor 1 0 1 }");
   boxKit->set("shape { width 16 height 8 depth 16 }");

   // Create the calculator to make a translation
   // for the text.  The x component of a translate1Dragger's 
   // translation field shows how far it moved in that 
   // direction. So our text's translation is:
   // (xDragTranslate[0],yDragTranslate[0],zDragTranslate[0])
   SoCalculator *myCalc = new SoCalculator;
   myCalc->ref();
   myCalc->A.connectFrom(&xDragger->translation);
   myCalc->B.connectFrom(&yDragger->translation);
   myCalc->C.connectFrom(&zDragger->translation);
   myCalc->expression = "oA = vec3f(A[0],B[0],C[0])";

   // Connect the the translation in textKit from myCalc
   SoTransform *textXf 
      = (SoTransform *) textKit->getPart("transform",TRUE);
   textXf->translation.connectFrom(&myCalc->oA);

   SoXtExaminerViewer *myViewer = 
            new SoXtExaminerViewer(myWindow);
   myViewer->setSceneGraph(root);
   myViewer->setTitle("Customized Sliders");
   myViewer->viewAll();
   myViewer->show();

   SoXt::show(myWindow);
   SoXt::mainLoop();
}
