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

#include <Inventor/SbString.h>
#include <Inventor/SoType.h>

enum Language {
    ALL,
    C,
    C_PLUS_PLUS
};

class Reader {
  public:
    static SbBool	open(const char *pathName);
    static SbBool	open(const char *dirName, const char *fileName);
    static void		error(const char *what);
    static SbBool	readName(SbName &name);
    static SbBool	readString(SbString &string);
    static SbBool	readBracketed(SbString &string);
    static SbBool	readUpTo(int endChar, SbString &string);
    static void		skipWhiteSpace();
    static int		get();
    static void		putBack(int c);
    // Removes tabs, newlines, and extra spaces
    static void		cleanString(SbString &string, SbBool inCode = FALSE,
				SbBool changeSbName = TRUE);
  private:
    static FILE		*fp;
    static int		lineNum;
    static char		*fullName;
    static SbBool	readBuf(char *buf, SbBool isName);
};

class Writer {
  public:
    enum Font {
	TEXT_ROMAN,	// Fonts for text descriptions
	TEXT_ITALIC,
	TEXT_BOLD,
	CODE_ROMAN,	// Fonts for code (method, field, ...) specifications
	CODE_BOLD,
	FIXED_ROMAN,	// Fonts for fixed-width code
	FIXED_BOLD,
	PREVIOUS	// Previous font
    };
    static void		setPointer(FILE *filePointer) { fp = filePointer; }
    static void		setSourceFileName(char *f) { sourceFileName = strdup(f); }
    static SbBool	open(const char *, const char *fileName);
    static void		setInSynopsis(SbBool flag) { inSynopsis = flag; }
    static SbBool	getInSynopsis()		   { return inSynopsis; }
    static void		setLanguage(Language lang) { language = lang; }
    static Language	getLanguage()		   { return language; }
    static void		setForBook(SbBool flag)	   { forBook = flag; }
    static SbBool	isForBook()		   { return forBook; }
    static void		writePreface(const SbName &className);
    static void		startSection(const char *name);
    static void		setFont(Font newFont);
    static void		changeSize(float diff);
    static void		setBaseIndent(float indInInches);
    static void		setIndent(int numChars, int firstLineChars);
    static void		setTab(int numTabChars);
    static void		indentForTab();
    static void		tab();
    static void		setFill(SbBool flag);
    static void		breakLine();
    static void		space(float amt = 1.0);
    static void		write(const char *string);
    static void		write(const SbName &name);
    static void		write(const SbString &string);
    static void		writeBracketed(const SbString &string);
    static void		writeDescription(const SbString &string);
    static void		finish();
  private:
    static FILE		*fp;
    static char		*fullName;
    static char		*sourceFileName;
    static SbBool	inSynopsis;
    static Language	language;
    static SbBool	forBook;
    static float	baseIndent;
    static int		numTabChars;
    static SbBool	indentedForTab;

    static const char *	writeStuff(const char *string,
				   SbBool compressSpace, int blockChar);
    static const char *	startBlock(const char *string, SbBool compressSpace);
    static const char *	endBlock(const char *string, int blockChar);
    static const char *	skipBlock(const char *string);
};

class Item {
  public:
    Item 		*next;
    Language		language;
    SbBool		hasNextToWrite();
    static void		setCurLanguage(Language lang) { curLanguage = lang; }
    virtual void	write(SbBool runIn) = 0;
    virtual SbBool	read()  = 0;
    virtual int		getNumPreTabChars() const;
  protected:
    Item();
  private:
    static Language	curLanguage;
};

class ItemList {
  public:
    Item		*first, *last;
    ItemList()		{ first = last = NULL; }
    SbBool		isEmpty() const;
    int			getNum() const;
    // if append is FALSE, add at start of list; else add to end
    void		add(Item *item, SbBool append = TRUE);
    void		write(SbBool runIn = FALSE);
};

class Inherit : public Item {
  public:
    Inherit(const SbName &name);
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
  private:
    SbName		className;
};

class Include : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
  private:
    SbString		file;
};

class Typedef : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
    virtual int		getNumPreTabChars() const;
  private:
    SbString		type, extra;
    SbName		name;
friend class ClassDef;
};

class Define : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
  private:
    SbString		definition;
};

class EnumDef : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
    virtual int		getNumPreTabChars() const;
  private:
    SbName		name;
    SbString		description;
};

class Enum : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
  private:
    SbName		name;
    ItemList		defs;
};

class Field : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
    virtual int		getNumPreTabChars() const;
  protected:
    SbName		name, type;
    SbString		description;
    SbBool		readStuff();
friend class ClassDef;
};

class Input : public Field {
  public:
    Input();
    virtual SbBool	read();
};

class Output : public Field {
  public:
    Output();
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
    virtual int		getNumPreTabChars() const;
};

class Part : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
    virtual int		getNumPreTabChars() const;
  private:
    SbName		name, type;
    SbString		description;
friend class ClassDef;
};

class Method : public Item {
  public:
    Method();
    ~Method();
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
    virtual int		getNumPreTabChars() const;
  private:
    enum Type {
	CONSTRUCTOR,
	DESTRUCTOR,
	OPERATOR,
	REGULAR
    };

    SbString		full, description;
    SbName		name, cName;
    SbString		pre, post;
    Type		type;
    SbBool		skipThis;
    SbBool		isHidden;
    SbBool		isStatic;
    SbBool		isVirtual;
    SbBool		isFriend;
    SbBool		isConst;
    SbBool		parse();
    void		writeName();
friend class ClassDef;
};

class Macro : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
  private:
    SbName		name;
    SbString		arguments, description;
};

class ActionName : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
  private:
    SbName		name;
};

class Action : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
  private:
    ItemList		names;
    SbString		description;
};

class Misc : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
  private:
    SbName		name;
    SbString		description;
};

class Also : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
  private:
    SbString		list;
};

class DraggerResourcePartDef : public Item {
  public:
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
  private:
    SbName		name;
friend class DraggerResource;
};

class DraggerResource : public Item {
  public:
    DraggerResource();
    virtual SbBool	read();
    virtual void	write(SbBool runIn);
    virtual int		getNumPreTabChars() const;
    int                 getNumPartDefs() const { return parts.getNum(); }
    const SbName        &getPartDefName( int which ) const;
    const SbName        &getResourceName() const { return name; }
  private:
    SbName		name;
    ItemList		parts;
    SbString		description;
    SbString		defaultGeom;
};

class ClassDef {

  public:
    ClassDef();
    ~ClassDef();

    const SbName &	getName() const		{ return name; }
    const SbName &	getCPrefix() const	{ return cPrefix; }
    const SbName &	getCEnumPrefix() const	{ return cEnumPrefix; }

    void		addInherit(ClassDef *def);
    void		setParent(ClassDef *parent) { parentClass = parent; }
    SbBool		read();
    SbBool		setUp(const ClassDef *mainDef);
    void		cullMethods();
    void		write(SbBool runIn);

    static const ClassDef *	getCurClass()	{ return curClass; }

  private:
    enum StatementType {
	NAME,
	DESCRIPTION,
	INCLUDE,
	TYPEDEF,
	DEFINE,
	ENUM,
	FIELD,
	INPUT,
	OUTPUT,
	PART,
	METHOD,
	MACRO,
	ACTION,
	MISC,
	ALSO,
	BEGIN,
	END,
	DRAGGER_RESOURCE,
	UNKNOWN
    };

    static const ClassDef *curClass;

    ClassDef		*parentClass;
    SoType		type;
    SbBool		isAbstract;
    void		*instance;

    SbName		name, cPrefix, cEnumPrefix;
    SbString		shortDescription, longDescription;
    SbString		catalog;
    SbBool		hasFileFormat;
    SbString		fileFormat;
    int			fileFormatTabLength;

    ItemList		inherits;
    ItemList		includes;
    ItemList		typedefs;
    ItemList		defines;
    ItemList		enums;
    ItemList		fields;
    ItemList		inputs;
    ItemList		outputs;
    ItemList		parts;
    ItemList		methods;
    ItemList		macros;
    ItemList		actions;
    ItemList		miscs;
    ItemList		alsos;
    ItemList		draggerResources;

    SbBool		readItem(Item *item, ItemList *list);
    static StatementType getStatementType(const SbName &keyword);
    SbBool		setUpTypedefs();
    SbBool		setUpFields();
    SbBool		setUpInputs();
    SbBool		setUpOutputs();
    SbBool		setUpParts();
    SbBool		setUpCatalog();
    SbBool		setUpFileFormat();
    void *		getInstance();
    void		writeForBook();
    void		writeForNonBook();
    void		writeName();
    void		writeSynopsis();
    void		writeFileFormat();
    void		writeFromClass(ItemList *list, const char *whats,
				       SbBool runIn = FALSE);
};

#define QUOTE_CHAR	'"'
#define OPEN_CHAR	'{'
#define CLOSE_CHAR	'}'
#define COMMENT_CHAR	'#'
#define SEP_CHAR	','
