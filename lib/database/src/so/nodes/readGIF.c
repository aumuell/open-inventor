/**************************************************************************
 *
 * 		  Copyright (c)	1994 David Koblas
 * Permission to use, copy, modify, and distribute this software 
 * and its documentation for any purpose and without fee is hereby 
 * granted, provided that the above copyright notice appear in all
 * copies and that both that copyright notice and this permission 
 * notice appear in supporting documentation.  This software is 
 * provided "as is" without express or implied warranty. 
 *
 **************************************************************************
 *
 * File: readGIF.c
 *
 * Description: Routines to read GIF 87a and 89a images.
 *
 **************************************************************************/


/* $Revision: 1.3 $ */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readGIF.h"


#define LOCALCOLORMAP		0x80
#define INTERLACE		0x40

#define MAX_LWZ_BITS	12

#define CM_RED		0
#define CM_GREEN	1
#define CM_BLUE		2

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif

#define readLWZ(fd)		((sp > stack) ? *--sp : nextLWZ(fd))
#define ReadOK(file,buffer,len)	(fread(buffer, len, 1, file) != 0)
#define LM_to_uint(a,b)		(((b)<<8)|(a))
#define BitSet(byte, bit)	(((byte) & (bit)) == (bit))


static struct {
    unsigned int Width;
    unsigned int Height;
    unsigned char ColorMap[3][GIF_MAXCOLORMAPSIZE];
    unsigned int BitPixel;
    unsigned int ColorResolution;
    unsigned int Background;
    unsigned int AspectRatio;
    int xGrayScale;
} GifScreen;


static struct {
    int transparent;
    int delayTime;
    int inputFlag;
    int disposal;
} Gif89 = { -1, -1, -1, 0 };


/* Local functions */

static int readColorMap(FILE *fd, int number, unsigned char buffer[][256],
								int *gray);
static int doExtension(FILE *fd, int label);
static unsigned char* readImage(FILE *fd, int len, int height, XColor *colors,
		int cmapSize, unsigned char cmap[][GIF_MAXCOLORMAPSIZE],
		int gray, int interlace, int ignore);
static void initLWZ(int input_code_size);
static int nextLWZ(FILE *fd);
static int GetDataBlock(FILE *fd, unsigned char *buf);
static int nextCode(FILE * fd, int code_size);


/* File scope variables */

static  int curbit, lastbit, get_done, last_byte;
static  int return_clear;
static  int ZeroDataBlock = FALSE;
/*	For nextLWZ */
static int stack[(1<<(MAX_LWZ_BITS))*2], *sp;
static int code_size, set_code_size;
static int max_code, max_code_size;
static int clear_code, end_code;


unsigned char* readGIF(FILE *fd, int *w, int *h, XColor *colors, int *ncolors,
						int *bgIndex, int *errCode)
{
    unsigned char buf[16];
    unsigned char c;
    unsigned char localColorMap[3][GIF_MAXCOLORMAPSIZE];
    int grayScale;
    int useGlobalColormap;
    int bitPixel;
    int imageCount = 0;
    char version[4];
    int imageNumber = 1;
    unsigned char *image = NULL;
    int i;
    static const int scale = 65536 / GIF_MAXCOLORMAPSIZE + 1;

    /*
     * Initialize the error code
     */
    *errCode = GIF_NO_ERROR;

    /*
     * Initialize GIF89 extensions
     */
    Gif89.transparent = -1;
    Gif89.delayTime = -1;
    Gif89.inputFlag = -1;
    Gif89.disposal = 0;

    if (!ReadOK(fd, buf, 6)) {
	*errCode = GIF_BAD_HEADER;
	return (NULL);
    }
    if (strncmp((char *) buf, "GIF", 3) != 0) {
	*errCode = GIF_BAD_MAGIC;
	return (NULL);
    }
    strncpy(version, (char *) buf + 3, 3);
    version[3] = '\0';

    if ((strcmp(version, "87a") != 0) && (strcmp(version, "89a") != 0)) {
	*errCode = GIF_BAD_VERSION;	/* Version not 87a or 89a */
	return (NULL);
    }
    if (!ReadOK(fd, buf, 7)) {
	*errCode = GIF_BAD_SCRNDESC;
	return (NULL);
    }
    GifScreen.Width = LM_to_uint(buf[0], buf[1]);
    GifScreen.Height = LM_to_uint(buf[2], buf[3]);
    GifScreen.BitPixel = 2 << (buf[4] & 0x07);
    GifScreen.ColorResolution = (((buf[4] & 0x70) >> 3) + 1);
    GifScreen.Background = buf[5];
    GifScreen.AspectRatio = buf[6];

    *ncolors = GifScreen.BitPixel;

    if (BitSet(buf[4], LOCALCOLORMAP)) {	/* Global Colormap */
	if (readColorMap(fd, GifScreen.BitPixel, GifScreen.ColorMap,
			 &GifScreen.xGrayScale)) {
	    *errCode = GIF_BAD_GCOLORMAP;
	    return (NULL);
	}
	for (i = 0; i < GifScreen.BitPixel; i++) {
	    colors[i].red = GifScreen.ColorMap[0][i] * scale;
	    colors[i].green = GifScreen.ColorMap[1][i] * scale;
	    colors[i].blue = GifScreen.ColorMap[2][i] * scale;
	    colors[i].pixel = i;
	    colors[i].flags = DoRed | DoGreen | DoBlue;
	}
	for (i = GifScreen.BitPixel; i < GIF_MAXCOLORMAPSIZE; i++) {
	    colors[i].red = 0;
	    colors[i].green = 0;
	    colors[i].blue = 0;
	    colors[i].pixel = i;
	    colors[i].flags = DoRed | DoGreen | DoBlue;
	}
    }

    while (image == NULL) {
	if (!ReadOK(fd, &c, 1)) {
	    *errCode = GIF_SHORT_IMAGE;		/* EOF on read of image data */
	    return (NULL);
	}
	if (c == ';') {		/* GIF terminator */
	    if (imageCount < imageNumber) {
		*errCode = GIF_NO_IMAGE; 
		return (NULL);
	    }
	    break;
	}
	if (c == '!') {		/* Extension */
	    if (!ReadOK(fd, &c, 1)) {
		*errCode = GIF_SHORT_EXT; /* EOF/read error on exten. code */
		return (NULL);
	    }
	    doExtension(fd, c);
	    continue;
	}
	if (c != ',') {		/* Not a valid start character */
	    *errCode = GIF_BAD_CHARACTER;
	    continue;
	}
	++imageCount;

	if (!ReadOK(fd, buf, 9)) {
	    *errCode = GIF_SHORT_DIMS;	/* Short read on dimensions */
	    return (NULL);
	}
	useGlobalColormap = !BitSet(buf[8], LOCALCOLORMAP);
	bitPixel = 1 << ((buf[8] & 0x07) + 1);

	/*
	 * We only want to set width and height for the imageNumber we are
	 * requesting.
	 */
	if (imageCount == imageNumber) {
	    *w = LM_to_uint(buf[4], buf[5]);
	    *h = LM_to_uint(buf[6], buf[7]);
	}
	if (!useGlobalColormap) {
	    if (readColorMap(fd, bitPixel, localColorMap, &grayScale)) {
	        *errCode = GIF_BAD_LCOLORMAP;
		return (NULL);
	    }
	    *ncolors = bitPixel;
	    for (i = 0; i < bitPixel; i++) {
	        colors[i].red = localColorMap[0][i] * scale;
	        colors[i].green = localColorMap[1][i] * scale;
	        colors[i].blue = localColorMap[2][i] * scale;
	        colors[i].pixel = i;
	        colors[i].flags = DoRed | DoGreen | DoBlue;
	    }
	    for (i = bitPixel; i < GIF_MAXCOLORMAPSIZE; i++) {
	        colors[i].red = 0;
	        colors[i].green = 0;
	        colors[i].blue = 0;
	        colors[i].pixel = i;
	        colors[i].flags = DoRed | DoGreen | DoBlue;
	    }

	    /*
	     * We only want to set the data for the imageNumber we are
	     * requesting.
	     */
	    if (imageCount == imageNumber) {
		image = readImage(fd, LM_to_uint(buf[4], buf[5]),
				  LM_to_uint(buf[6], buf[7]), colors,
				  bitPixel, localColorMap, grayScale,
				  BitSet(buf[8], INTERLACE),
				  imageCount != imageNumber);
	    } else {
		unsigned char  *tdata;

		tdata = readImage(fd, LM_to_uint(buf[4], buf[5]),
				  LM_to_uint(buf[6], buf[7]), colors,
				  bitPixel, localColorMap, grayScale,
				  BitSet(buf[8], INTERLACE),
				  imageCount != imageNumber);
	    }
	} else {
	    /*
	     * We only want to set the data for the imageNumber we are
	     * requesting.
	     */
	    if (imageCount == imageNumber) {
		image = readImage(fd, LM_to_uint(buf[4], buf[5]),
				  LM_to_uint(buf[6], buf[7]), colors,
				  GifScreen.BitPixel, GifScreen.ColorMap,
				  GifScreen.xGrayScale,
				  BitSet(buf[8], INTERLACE),
				  imageCount != imageNumber);
	    } else {
		unsigned char  *tdata;

		tdata = readImage(fd, LM_to_uint(buf[4], buf[5]),
				  LM_to_uint(buf[6], buf[7]), colors,
				  GifScreen.BitPixel, GifScreen.ColorMap,
				  GifScreen.xGrayScale,
				  BitSet(buf[8], INTERLACE),
				  imageCount != imageNumber);
	    }
	}

    }
    *bgIndex = Gif89.transparent;

    return (image);
}


static int readColorMap(FILE *fd, int number, unsigned char buffer[][256],
								int *gray)
{
    int i;
    unsigned char rgb[3];
    int flag;

    flag = TRUE;

    for (i = 0; i < number; ++i) {
	if (!ReadOK(fd, rgb, sizeof(rgb))) {
	    return TRUE;
	}
	buffer[CM_RED][i] = rgb[0];
	buffer[CM_GREEN][i] = rgb[1];
	buffer[CM_BLUE][i] = rgb[2];

	flag &= (rgb[0] == rgb[1] && rgb[1] == rgb[2]);
    }

    *gray = flag;

    return FALSE;
}


static int doExtension(FILE *fd, int label)
{
    static char buf[256];
    char *str;
    int showComment = FALSE;

    switch (label) {
	case 0x01:		/* Plain Text Extension */
	    str = "Plain Text Extension";
#ifdef notdef
	    if (GetDataBlock(fd, (unsigned char *) buf) <= 0);

	    lpos = LM_to_uint(buf[0], buf[1]);
	    tpos = LM_to_uint(buf[2], buf[3]);
	    width = LM_to_uint(buf[4], buf[5]);
	    height = LM_to_uint(buf[6], buf[7]);
	    cellw = buf[8];
	    cellh = buf[9];
	    foreground = buf[10];
	    background = buf[11];

	    while (GetDataBlock(fd, (unsigned char *) buf) > 0) {
		PPM_ASSIGN(image[ypos][xpos],
			   cmap[CM_RED][v],
			   cmap[CM_GREEN][v],
			   cmap[CM_BLUE][v]);
		++index;
	    }

	    return FALSE;
#else
	    break;
#endif
	case 0xff:		/* Application Extension */
	    str = "Application Extension";
	    break;
	case 0xfe:		/* Comment Extension */
	    str = "Comment Extension";
	    while (GetDataBlock(fd, (unsigned char *) buf) > 0) {
		if (showComment) {
#if 0
		    fprintf(stderr, "gif comment: %s\n", buf);
#endif
		}
	    }
	    return FALSE;
	case 0xf9:		/* Graphic Control Extension */
	    str = "Graphic Control Extension";
	    (void) GetDataBlock(fd, (unsigned char *) buf);
	    Gif89.disposal = (buf[0] >> 2) & 0x7;
	    Gif89.inputFlag = (buf[0] >> 1) & 0x1;
	    Gif89.delayTime = LM_to_uint(buf[1], buf[2]);
	    if ((buf[0] & 0x1) != 0)
		Gif89.transparent = (int) ((unsigned char) buf[3]);

	    while (GetDataBlock(fd, (unsigned char *) buf) > 0)
		;
	    return FALSE;
	default:
	    str = buf;
	    sprintf(buf, "UNKNOWN (0x%02x)", label);
	    break;
    }

    /* fprintf(stderr, "got a '%s' extension\n", str); */

    while (GetDataBlock(fd, (unsigned char *) buf) > 0);

    return FALSE;
}


static unsigned char* readImage(FILE *fd, int len, int height, XColor *colors,
			int cmapSize, unsigned char cmap[][GIF_MAXCOLORMAPSIZE],
			int gray, int interlace, int ignore)
{
    unsigned char *dp, c;
    int v;
    int xpos = 0, ypos = 0;
    unsigned char *image;

    /*
     * Initialize the Compression routines
     */
    if (!ReadOK(fd, &c, 1)) {
#if 0
	fprintf(stderr, "EOF / read error on image data\n");
#endif
	return (NULL);
    }
    initLWZ(c);

    /*
     * *  If this is an "uninteresting picture" ignore it.
     */
    if (ignore) {
	while (readLWZ(fd) >= 0);
	return (NULL);
    }
    image = (unsigned char *)calloc(len * height, sizeof(char));
    if (image == NULL) {
	fprintf(stderr, "Cannot allocate space for image data\n");
	return (NULL);
    }
    for (v = 0; v < GIF_MAXCOLORMAPSIZE; v++) {
	colors[v].red = colors[v].green = colors[v].blue = 0;
	colors[v].pixel = v;
	colors[v].flags = DoRed | DoGreen | DoBlue;
    }
    for (v = 0; v < cmapSize; v++) {
	colors[v].red = cmap[CM_RED][v] * 0x101;
	colors[v].green = cmap[CM_GREEN][v] * 0x101;
	colors[v].blue = cmap[CM_BLUE][v] * 0x101;
    }

#if 0
    fprintf(stderr, "reading %d by %d%s GIF image\n",
		len, height, interlace ? " interlaced" : "");
#endif

    if (interlace) {
	int i;
	int pass = 0, step = 8;

	for (i = 0; i < height; i++) {
	    if (ypos < height) {
		dp = &image[len * ypos];
		for (xpos = 0; xpos < len; xpos++) {
		    if ((v = readLWZ(fd)) < 0)
			goto fini;

		    *dp++ = v;
		}
	    }
	    if ((ypos += step) >= height) {
		if (pass++ > 0)
		    step /= 2;
		ypos = step / 2;
	    }
	}
    } else {
	dp = image;
	for (ypos = 0; ypos < height; ypos++) {
	    for (xpos = 0; xpos < len; xpos++) {
		if ((v = readLWZ(fd)) < 0)
		    goto fini;

		*dp++ = v;
	    }
	}
    }

fini:
    if (readLWZ(fd) >= 0) {
#if 0
	INFO_MSG(("too much input data, ignoring extra..."));
#endif
    }
    return (image);
}


static void initLWZ(int input_code_size)
{
    set_code_size = input_code_size;
    code_size = set_code_size + 1;
    clear_code = 1 << set_code_size;
    end_code = clear_code + 1;
    max_code_size = 2 * clear_code;
    max_code = clear_code + 2;

    curbit = lastbit = 0;
    last_byte = 2;
    get_done = FALSE;

    return_clear = TRUE;

    sp = stack;
}


static int nextLWZ(FILE *fd)
{
    static int table[2][(1 << MAX_LWZ_BITS)];
    static int firstcode, oldcode;
    int code, incode;
    register int i;

    while ((code = nextCode(fd, code_size)) >= 0) {
	if (code == clear_code) {

	    /* corrupt GIFs can make this happen */
	    if (clear_code >= (1 << MAX_LWZ_BITS)) {
		return -2;
	    }
	    for (i = 0; i < clear_code; ++i) {
		table[0][i] = 0;
		table[1][i] = i;
	    }
	    for (; i < (1 << MAX_LWZ_BITS); ++i)
		table[0][i] = table[1][i] = 0;
	    code_size = set_code_size + 1;
	    max_code_size = 2 * clear_code;
	    max_code = clear_code + 2;
	    sp = stack;
	    do {
		firstcode = oldcode = nextCode(fd, code_size);
	    } while (firstcode == clear_code);

	    return firstcode;
	}
	if (code == end_code) {
	    int             count;
	    unsigned char   buf[260];

	    if (ZeroDataBlock)
		return -2;

	    while ((count = GetDataBlock(fd, buf)) > 0);

	    if (count != 0) {
#if 0
		INFO_MSG(("missing EOD in data stream (common occurence)"));
#endif
	    }
	    return -2;
	}
	incode = code;

	if (code >= max_code) {
	    *sp++ = firstcode;
	    code = oldcode;
	}
	while (code >= clear_code) {
	    *sp++ = table[1][code];
	    if (code == table[0][code]) {
#if 0
		ERROR("circular table entry BIG ERROR");
#endif
		return (code);
	    }
	    if ((unsigned long) sp >= ((unsigned long) stack + sizeof(stack))) {
#if 0
		ERROR("circular table STACK OVERFLOW!");
#endif
		return (code);
	    }
	    code = table[0][code];
	}

	*sp++ = firstcode = table[1][code];

	if ((code = max_code) < (1 << MAX_LWZ_BITS)) {
	    table[0][code] = oldcode;
	    table[1][code] = firstcode;
	    ++max_code;
	    if ((max_code >= max_code_size) &&
		(max_code_size < (1 << MAX_LWZ_BITS))) {
		max_code_size *= 2;
		++code_size;
	    }
	}
	oldcode = incode;

	if (sp > stack)
	    return *--sp;
    }
    return code;
}


static int GetDataBlock(FILE *fd, unsigned char *buf)
{
    unsigned char count;

    count = 0;
    if (!ReadOK(fd, &count, 1)) {
#if 0
	fprintf(stderr, "error in getting DataBlock size\n");
#endif
	return -1;
    }
    ZeroDataBlock = count == 0;

    if ((count != 0) && (!ReadOK(fd, buf, count))) {
#if 0
	fprintf(stderr, "error in reading DataBlock\n");
#endif
	return -1;
    }
    return ((int) count);
}


static int nextCode(FILE * fd, int code_size)
{
    static unsigned char buf[280];
    static int maskTbl[16] = {
	0x0000, 0x0001, 0x0003, 0x0007,
	0x000f, 0x001f, 0x003f, 0x007f,
	0x00ff, 0x01ff, 0x03ff, 0x07ff,
	0x0fff, 0x1fff, 0x3fff, 0x7fff,
    };
    int i, j, ret, end;

    if (return_clear) {
	return_clear = FALSE;
	return clear_code;
    }
    end = curbit + code_size;

    if (end >= lastbit) {
	int             count;

	if (get_done) {
	    if (curbit >= lastbit) {
#if 0
		ERROR("ran off the end of my bits");
#endif
	    }
	    return -1;
	}
	buf[0] = buf[last_byte - 2];
	buf[1] = buf[last_byte - 1];

	if ((count = GetDataBlock(fd, &buf[2])) == 0)
	    get_done = TRUE;

	last_byte = 2 + count;
	curbit = (curbit - lastbit) + 16;
	lastbit = (2 + count) * 8;

	end = curbit + code_size;
    }
    j = end / 8;
    i = curbit / 8;

    if (i == j)
	ret = (int) buf[i];
    else if (i + 1 == j)
	ret = (int) buf[i] | ((int) buf[i + 1] << 8);
    else
	ret = (int) buf[i] | ((int) buf[i + 1] << 8) | ((int) buf[i + 2] << 16);

    ret = (ret >> (curbit % 8)) & maskTbl[code_size];

    curbit += code_size;

    return ret;
}
