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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoSeparatorKit
 |
 |   Author(s)          : Paul Isaacs and Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>
#include <Inventor/nodekits/SoSeparatorKit.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodekits/SoAppearanceKit.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoUnits.h>
#include <Inventor/nodes/SoTexture2Transform.h>

SO_KIT_SOURCE(SoSeparatorKit);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoSeparatorKit::SoSeparatorKit()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoSeparatorKit);

    isBuiltIn = TRUE;

    // Initialize children catalog and add entries to it
    // These are the macros you use to make a catalog.
    // Use combinations of ...ADD_CATALOG_ENTRY 
    // and ...ADD_CATALOG_LIST_ENTRY.  See SoSubKit.h for more info
    // on syntax of these macros.

    SO_KIT_ADD_CATALOG_ENTRY(topSeparator, SoSeparator, TRUE,
				    this, , FALSE );

    SO_KIT_ADD_CATALOG_ENTRY(pickStyle, SoPickStyle, TRUE,
				       topSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(appearance,     SoAppearanceKit, TRUE,
				    topSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(units,      SoUnits, TRUE,
				    topSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(transform,      SoTransform, TRUE,
				    topSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_ENTRY(texture2Transform, SoTexture2Transform, TRUE,
				    topSeparator, , TRUE );

    SO_KIT_ADD_CATALOG_LIST_ENTRY(childList, SoSeparator, TRUE,
				       topSeparator, , SoShapeKit, TRUE );
    SO_KIT_ADD_LIST_ITEM_TYPE(childList, SoSeparatorKit );

    SO_KIT_ADD_FIELD(renderCaching,            (AUTO));
    SO_KIT_ADD_FIELD(boundingBoxCaching,       (AUTO));
    SO_KIT_ADD_FIELD(renderCulling,            (AUTO));
    SO_KIT_ADD_FIELD(pickCulling,              (AUTO));

    // Set up static info for enum fields
    SO_KIT_DEFINE_ENUM_VALUE(CacheEnabled, OFF);
    SO_KIT_DEFINE_ENUM_VALUE(CacheEnabled, ON);
    SO_KIT_DEFINE_ENUM_VALUE(CacheEnabled, AUTO);

    // Set up info in enumerated type fields
    SO_KIT_SET_SF_ENUM_TYPE(renderCaching,     CacheEnabled);
    SO_KIT_SET_SF_ENUM_TYPE(boundingBoxCaching,CacheEnabled);
    SO_KIT_SET_SF_ENUM_TYPE(renderCulling,     CacheEnabled);
    SO_KIT_SET_SF_ENUM_TYPE(pickCulling,       CacheEnabled);

    SO_KIT_INIT_INSTANCE();

    // For Inventor 2.0, we disabled notification on the part fields because
    // it made things real slow.
    // For Inventor 2.1, the notification scheme was improved to the point
    // where this was not necessary. But caution (and the fact that I only
    // found out about this late) dictate that we not put notification back
    // for all part fields.
    // However, but #274396 reports a problem that arose from disabling 
    // notification on the topSeparator field, because the fieldSensor 
    // never gets notified and so connections are never built between the
    // topSeparator's fields and the cache/cull fields of the SeparatorKit.
    // So we enable notification on the field right here to fix this.
    // Note that we must do this AFTER SO_KIT_INIT_INSTANCE, for that is
    // where the field is DISabled in the first place, within the construction
    // of the partsList.
    topSeparator.enableNotify(TRUE);

    // This sensor will watch the topSeparator part.  If the part changes to a 
    // new node,  then the fields of the old part will be disconnected and
    // the fields of the new part will be connected.
    // Connections are made from/to the renderCaching, boundingBoxCaching,
    // renderCulling and pickCulling fields. This way, the SoSeparatorKit
    // can be treated from the outside just like a regular SoSeparator node.
    // Setting the fields will affect caching and culling, even though the
    // topSeparator takes care of it.
    // oldTopSep keeps track of the part for comparison.
    fieldSensor = new SoFieldSensor( &SoSeparatorKit::fieldSensorCB, this );
    fieldSensor->setPriority(0);
    oldTopSep = NULL;
    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public

SoSeparatorKit::~SoSeparatorKit()
//
////////////////////////////////////////////////////////////////////////
{
    if (oldTopSep) {
	oldTopSep->unref();
	oldTopSep = NULL;
    }
    if (fieldSensor)
	delete fieldSensor;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    overrides method in SoNode to return FALSE.
//
// Use: public
//
SbBool
SoSeparatorKit::affectsState() const
{
    return FALSE;
}


//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoSeparatorKit::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoBaseKit::setUpConnections( onOff, FALSE );

	// Hookup the field-to-field connections on topSeparator.
	connectSeparatorFields( oldTopSep, TRUE );

	// Call sensor CBs to make sure oldTopSep is up-to-date
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (fieldSensor->getAttachedField() != &topSeparator)
	    fieldSensor->attach( &topSeparator );
    }
    else {

	// We disconnect BEFORE base class.

	// Disconnect the field sensors.
	if (fieldSensor->getAttachedField())
	    fieldSensor->detach();

	// Undo the field-to-field connections on topSeparator.
        connectSeparatorFields( oldTopSep, FALSE );

	SoBaseKit::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

void
SoSeparatorKit::connectSeparatorFields( SoSeparator *dest, SbBool onOff )
{
    if (dest == NULL)
	return;
    if (onOff) {
	SoField *f;
	if ( ! dest->renderCaching.getConnectedField(f) ||
	       f != &renderCaching )
	    dest->renderCaching.connectFrom( &renderCaching );
	if ( ! dest->boundingBoxCaching.getConnectedField(f) ||
	       f != &boundingBoxCaching )
	    dest->boundingBoxCaching.connectFrom( &boundingBoxCaching );
	if ( ! dest->renderCulling.getConnectedField(f) ||
	       f != &renderCulling )
	    dest->renderCulling.connectFrom( &renderCulling );
	if ( ! dest->pickCulling.getConnectedField(f) ||
	       f != &pickCulling )
	    dest->pickCulling.connectFrom( &pickCulling );
    }
    else {
	dest->renderCaching.disconnect();
	dest->boundingBoxCaching.disconnect();
	dest->renderCulling.disconnect();
	dest->pickCulling.disconnect();
    }
}

void
SoSeparatorKit::fieldSensorCB( void *inKit, SoSensor *)
{
    SoSeparatorKit *k  = (SoSeparatorKit *) inKit;
    if ( k->oldTopSep == k->topSeparator.getValue())
	return;

    k->connectSeparatorFields( k->oldTopSep, FALSE );

    SoNode *newTopSep = k->topSeparator.getValue();
    if (newTopSep)
	newTopSep->ref();

    if (k->oldTopSep)
	k->oldTopSep->unref();

    k->oldTopSep = (SoSeparator *) newTopSep;
    k->connectSeparatorFields( k->oldTopSep, TRUE );
}

/////////////////////////////////////////////////////////////////////////
//
// Called by the SoBaseKit::write() method. Calls setDefault(TRUE)
// on the topSeparator. Note that this may be overriden later by basekit
// if, for example, topSeparator lies on a path that is being written out.
//
/////////////////////////////////////////////////////////////////////////
void
SoSeparatorKit::setDefaultOnNonWritingFields()
{
    topSeparator.setDefault(TRUE);

    // Call the base class...
    SoBaseKit::setDefaultOnNonWritingFields();
}
