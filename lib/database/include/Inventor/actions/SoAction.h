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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	Defines the SoAction class and related classes.
 |
 |   Author(s)		: Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ACTION_
#define  _SO_ACTION_

#include <Inventor/misc/SoBasic.h>
#include <Inventor/misc/SoTempPath.h>
#include <Inventor/SoPath.h>

// This is the type of a method function pointer in the table
class SoAction;
class SoCompactPathList;
class SoNode;
class SoState;
class SoLightPath;

typedef void	(*SoActionMethod)(SoAction *, SoNode *);


//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoActionMethodList
//
//  Internal class.  A list of routines to call, one for each node type.
//
//////////////////////////////////////////////////////////////////////////////

#if _COMPILER_VERSION>=710
#  pragma set woff 1375
#endif

SoINTERNAL class SoActionMethodList : public SbPList {

  public:
    // Constructor.  Pass in list from parent action.
    SoActionMethodList(SoActionMethodList *parentList);

    // Operator used to get and set methods.  The list will grow
    // dynamically as we access items off the end of the list, and
    // entries will be initialized to NULL.
    SoActionMethod &	operator [](int i) const
	{ return ((SoActionMethod &)((*(const SbPList *)this)[i])); }

    // Add a method to the appropriate place in the list.
    void		addMethod(SoType nodeType, SoActionMethod method);
    
    // This MUST be called before using the list.  It fills in NULL
    // entries with their parents' method.
    void		setUp();

  private:
    SoActionMethod	parentMethod(SoType);	// Method from parent node
    SoActionMethodList	*parent;
    int			numValidTypes;   // Stores the number of types at most
                                         // recent setup

    // Dummy action used internally
    static void		dummyAction(SoAction *, SoNode *);

};

#if _COMPILER_VERSION>=710
#  pragma reset woff 1375
#endif

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoEnabledElementsList
//
//  Internal class.  A list of elements enabled for a given action
//  class. Lists of elements can be merged, allowing an action class
//  to inherit enabled elements from its parent class.
//
//////////////////////////////////////////////////////////////////////////////

SoINTERNAL class SoEnabledElementsList {

  public:
    // Constructor
    SoEnabledElementsList(SoEnabledElementsList *parentList);

    // Returns list of elements.  This automatically merges the
    // elements with elements enabled in the parentList.
    const SoTypeList &	getElements() const;

    // Adds an element to the list if it's not already in it
    void		enable(SoType elementType, int stackIndex);

    // Enables all elements from the given list that are not already
    // enabled in this one
    void		merge(const SoEnabledElementsList &list);

    // Returns the current setting of the global counter used to
    // determine when lists are out of date.
    static int		getCounter()		{ return counter; }

  private:
    // This maintains a global counter used to determine when lists
    // are out of date. It is incremented whenever a new element is
    // added to a list.
    static int		counter;

    // And a per-instance counter so we don't merge enabled elements
    // with the parent list unnecessarily.
    int			setUpCounter;

    // This list holds type id's of enabled elements, indexed by the
    // stack index of the elements.
    SoTypeList		elements;

    // Pointer to parent's list of enabled elements (NULL for
    // SoAction).
    SoEnabledElementsList	*parent;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoAction
//
//  This is the parent class for all traversal methods.  When
//  traversing, lookup is done based on the node type and the action
//  type.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract
// C-api: prefix=SoAct
class SoAction {

  public:
    // Destructor
    // C-api: expose
    virtual ~SoAction();

    // Returns type identifier for SoAction class
    // C-api: expose
    static SoType	getClassTypeId()
			    { return classTypeId; }

    // Returns type identifier for action
    // C-api: expose
    virtual SoType	getTypeId() const = 0;

    // Returns TRUE if action is of given type or is derived from it
    SbBool		isOfType(SoType type) const;

    // Applies action to the graph rooted by a node
    // C-api: expose
    virtual void	apply(SoNode *node);

    // Applies action to the graph defined by a path
    // C-api: expose
    // C-api: name=ApplyPath
    virtual void	apply(SoPath *path);

    // Applies action to the graphs defined by a path list. The
    // "obeysRules" flag can be set to TRUE if the given path list has
    // the following 4 properties:
    //    1 - All paths have the same head node
    //    2 - Paths are sorted in traversal order
    //    3 - If one path ends at node A, no other path continues through A
    //	  4 - No two paths are the same
    // These rules will be obeyed by path lists returned by searches
    // for non-group nodes.
    // C-api: expose
    // C-api: name=ApplyPathList
    virtual void	apply(const SoPathList &pathList,
			      SbBool obeysRules = FALSE);

    // Invalidate the state, forcing it to be recreated at the next apply
    // C-api: expose
    virtual void	invalidateState();
    
  SoEXTENDER public:

    // Null action method that can be stored in lookup table when desired
    static void		nullAction(SoAction *, SoNode *);

    // This enum is used to determine what the action is being applied to
    enum AppliedCode {
	NODE,			// Applied to graph rooted by a node
	PATH,			// Applied to graph defined by a path
	PATH_LIST		// Applied to graphs defined by list of paths
    };

    // This enum may be used during traversal of nodes to indicate
    // where the node is with respect to the path being traversed
    enum PathCode {
	NO_PATH,		// Not traversing a path
	IN_PATH,		// In middle of path chain (not tail node)
	BELOW_PATH,		// Tail node of path or below tail node
	OFF_PATH		// None of the above (Probably to the left)
    };

    // Returns code indicating what action is being applied to
    AppliedCode		getWhatAppliedTo() const   { return appliedTo.code; }

    // These returns a pointer to the node, path, or path list the
    // action is being applied to. Each returns NULL if the action is
    // not being applied to the appropriate class.
    SoNode *		getNodeAppliedTo() const  { return appliedTo.node; }
    SoPath *		getPathAppliedTo() const  { return appliedTo.path; }
    // A single path list may be split into several, one for each
    // different head node. These methods allow subclasses to
    // determine the current path list, the original path list, and
    // whether the current list is the last one from the original
    const SoPathList *	getPathListAppliedTo() const
	{ return appliedTo.pathList; }
    const SoPathList *	getOriginalPathListAppliedTo() const
	{ return appliedTo.origPathList; }
    SbBool		isLastPathListAppliedTo() const
	{ return appliedTo.isLastPathList; }

    // Returns path code based on where current node (the node at the
    // end of the current path) lies with respect to the path(s) the
    // action is being applied to. If this returns IN_PATH, indices is
    // set to point to an array of indices corresponding to the
    // children that continue the paths and numIndices is set to the
    // number of such children.
    PathCode	getPathCode(int &numIndices, const int *&indices)
	{   if (appliedTo.curPathCode == IN_PATH) {
    	        usePathCode(numIndices, indices);
	    }
    	    return appliedTo.curPathCode;
	}


    // Does traversal of a graph rooted by a node
    void		traverse(SoNode *node)
	{ (*traversalMethods)[
	    SoNode::getActionMethodIndex(node->getTypeId())](this, node); }

    // Returns TRUE if the traversal has reached a termination condition
    SbBool		hasTerminated() const	{ return terminated; }

    // Returns a pointer to the path accumulated during traversal,
    // i.e., the chain of nodes from the root of the traversed graph
    // to the current node being traversed.
    const SoPath *	getCurPath();

    // Get the state from the action
    SoState *		getState() const	{ return state; }

  SoINTERNAL public:
    static void		initClass();

    // Initialize ALL Inventor action classes
    static void		initClasses();

    // These methods maintain the current path accumulated so far
    // during traversal. The action needs to know whether this path is
    // a subset of the path being applied to; it saves this info in
    // the onPath variable. Before a node is pushed onto the current
    // path, call getOnPath() to determine the current setting. The
    // value of this flag should be passed in to popCurPath() so the
    // onPath variable can be restored.
    PathCode		getCurPathCode() const { return appliedTo.curPathCode;}
    void		pushCurPath(int childIndex);
    void		popCurPath(PathCode prevPathCode);
    // This is virtual, so that SoCallbackAction can use current node.
    virtual SoNode *	getCurPathTail();			

    // called by inline getPathCode:
    void		usePathCode(int &numIndices, const int *&indices);



    // Optimized versions of push/pop when we know path codes won't
    // change:
    void		pushCurPath()
    			{ curPath.append(-1); }
    void		popPushCurPath(int childIndex)
			{ curPath.setTail(childIndex);}
    void		popCurPath()
			{ curPath.pop(); }

  protected:
    // Constructor
    SoAction();

    // Returns the list of enabled elements for a given action subclass
    virtual const SoEnabledElementsList & getEnabledElements() const;

    // Begins traversal of an action at the given node. The default
    // method just calls traverse(node). This is virtual to allow
    // subclasses to do extra work before or after traversing the node.
    virtual void	beginTraversal(SoNode *node);

    // Allows subclass instance to indicate that traversal has reached
    // a termination condition
    void		setTerminated(SbBool flag)	{ terminated = flag; }

    // This method is used when applying an action to an SoPathList.
    // It returns TRUE if the action should create a compact version
    // of the path list before applying itself to it. The default
    // method returns TRUE, since the compact version is more
    // efficient. Some actions may choose to return FALSE; for
    // example, the SoWriteAction applies itself to each path
    // separately, so it doesn't need the extra overhead of compacting
    // the list.
    virtual SbBool	shouldCompactPathLists() const;

  SoINTERNAL protected:    
    SoState		*state;		// Traversal state

    // The list of what to do when.  Subclasses set this pointer to
    // their per-class instance in their constructors.
    SoActionMethodList	*traversalMethods;

    // Holds list of enabled elements for the SoAction class
    static SoEnabledElementsList *enabledElements;

    // ... and the methods
    static SoActionMethodList *methods;

  private:
    static SoType	classTypeId;		// Type identifier

    // This structure holds the necessary information about what the
    // action is being applied to. It allows this information to be
    // saved and restored when necessary.
    struct AppliedTo {
	AppliedCode		code;
	SoNode			*node;		// If code is NODE
	SoPath			*path;		// If code is PATH
	const SoPathList	*pathList;	// If code is PATH_LIST
	const SoPathList	*origPathList;
	SoCompactPathList	*compactPathList;
	int			isLastPathList;
	PathCode		curPathCode;
    }			appliedTo;

    // This is set to TRUE while the action is being applied, so we
    // can tell if the action is being applied recursively.
    SbBool		isBeingApplied;

    // This is the current path through the graph the action is being
    // applied to
    SoLightPath		curPath;

    // This holds the current path (nodes and childindices) whenever        
    // action->getCurPath is called:
    SoTempPath	*	tempPath;
 

    // Holds enabled-elements counter when state is created; used to
    // determine whether list of enabled elements is up to date.
    int			enabledElementsCounter;

    // This is TRUE if the action has reached a termination condition
    SbBool		terminated;

    // Stores next index in path being applied to so a pointer to it
    // can be returned
    int			index;

    // Creates state, based on list of elements passed in
    void		setUpState();

    // Cleans up after an action is done
    void		cleanUp();

    // Splits a sorted path list based on head nodes, then applies to each
    void		splitPathList(const SoPathList &sortedList,
				      const SoPathList &origPathList);

    // Applies action to sorted path list
    void		apply(const SoPathList &sortedList,
			      const SoPathList &origPathList,
			      SbBool isLastPathList);

friend class SoDB;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Classes can use this macro to enable a specific element class
//  within a specific action class. E.g.:
//
//	SO_ENABLE(SoGetBoundingBoxAction, SoModelMatrixElement);
//

#define SO_ENABLE(actionClass, elementClass)				\
	actionClass::enableElement(elementClass::getClassTypeId(),	\
				   elementClass::getClassStackIndex())

//
//////////////////////////////////////////////////////////////////////////////

#endif /* _SO_ACTION_ */
