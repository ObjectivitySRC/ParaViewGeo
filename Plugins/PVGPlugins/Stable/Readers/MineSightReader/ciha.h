/*
 * Copyright 2007 Mintec, Inc.
 * All Rights Reserved.
 *
 * Minesight: ciha.h
 *
 * Description: 
 *    header for the material resource module in libmsci
 */
#ifndef CIHA_HEADER
#define CIHA_HEADER

#include <msextern_c.h>

#include "ci.h"
#include "cimsr.h"

msBEGIN_EXTERN_C


#define MAX_LINE_LABEL 6
#define MAX_NODE_LABEL 12

typedef enum {
   alignLEFT, 
   alignCENTER,
   alignRIGHT
};

typedef enum {
   alignTOP, 
   alignMID,
   alignBOTTOM
};

#define HORZALIGN(a)     ((a) / 3)
#define VERTALIGN(a)     ((a) % 3) 
#define LABEL_UNSET  "-"

typedef enum {
   linenodeUNSET,
   linenodeOBJECT_NAME,
   linenodeOBJECT_MATERIAL,
   linenodeELEMENT_NAME,
   linenodeELEMENT_MATERIAL,
   linenodeX_VALUE,
   linenodeY_VALUE,
   linenodeZ_VALUE,
   linenodeCOUNT
};

typedef struct {
   Sstring *name;
   Sstring *path;
   int      arrownodecount;
   double   arrowsize;
   int      arrowstyle;
   RGB      contrastcolor;
   int      cutplanevis;
   int      edgevis;
   int      edgehardvis;
   int      decimalprecision; 
   RGB      facecolor;
   int      facepattern;
   Sstring *facepatternstring;
   int      globaltextvis;
   int      lightinterp;
   Sstring *linelabel[MAX_LINE_LABEL];
   int      linelabelvis[MAX_LINE_LABEL];
   double   linelabeldist;
   double   linelabeldistfirst;
   double   linelabeloffset;
   RGB      linecolor;
   int      linepattern;
   int      linevis;
   double   linewidth;
   int      markervis;
   Sstring *nodelabel[MAX_NODE_LABEL];
   int      nodelabelvis[MAX_NODE_LABEL];
   int      nodelabelcomvis;   
   int      nodelabelprefixvis;
   RGB      nodecolor;
   double   nodesize;
   int      nodesymbol;
   int      nodevis;
   int      operable;
   int      polylinevis;
   int      polylinearrowvis;
   int      polylinefacevis;
   int      polylinenodevis;
   int      quadvis;
   int      selectable;
   int      surfacevis;
   int      surfacefacevis;
   int      textstrokewidth;
   int      textalign;
   RGB      textcolor;
   Sstring *textfont;
   double   textsize;
   double   textspacing;
   int      textvis;
   double   transmission;
   int      transon;
} HA;

/* HA -- GENERAL */


LIBMSCI_API HA         *ciHAInitialize(const char *matname);
LIBMSCI_API void        ciHAFree(HA *ha);
LIBMSCI_API HA         *ciHALoadFromMaterialName(const char *path, const char *matname);
LIBMSCI_API void        ciHALoadFromHAttr(void *MSRptr, HA *ha);
            const char *ciHAGetName(HA *ha);                      
            const char *ciHAGetPath(HA *ha);
LIBMSCI_API int         ciHALineOrNodeLabelGetTypeAt(HA *ha, int idx, int isNode);
LIBMSCI_API const char *ciHALineOrNodeLabelGetTypeAsCharAt(HA *ha, int idx, int isNode);
LIBMSCI_API void        ciHADump(HA *ha, FILE *fp);


#define ciHAGetName(h)  ((h) ? CHRSTR((h)->name) : NULL)
#define ciHAGetPath(h)  ((h) ? CHRSTR((h)->path) : NULL)


/* HA -- MARKER */


int         ciHAMarkerGetGlobalVis(HA *ha);
RGB        *ciHAMarkerGetNodeColor(HA *ha);                 
double      ciHAMarkerGetNodeSize(HA *ha);                  
int         ciHAMarkerGetNodeStyle(HA *ha);                
int         ciHAMarkerGetNodeVis(HA *ha);
RGB        *ciHAMarkerGetNodeLabelColor(HA *ha);
int         ciHAMarkerGetNodeLabelDecimalCount(HA *ha);
const char *ciHAMarkerGetNodeLabelFont(HA *ha);
int         ciHAMarkerGetNodeLabelPrefixVis(HA *ha);
double      ciHAMarkerGetNodeLabelSize(HA *ha);
int         ciHAMarkerGetNodeLabelStrokeWidth(HA *ha);
int        *ciHAMarkerGetNodeLabelTypeAt(HA *ha,int idx);
const char *ciHAMarkerGetNodeLabelTypeAsCharAt(HA *ha, int idx);
int         ciHAMarkerGetNodeLabelVisAt(HA *ha, int idx);


#define ciHAMarkerGetGlobalVis(h)  ((h) ?  (h)->markervis : TRUE)
#define ciHAMarkerGetNodeColor(h)  ((h) ? &(h)->nodecolor : NULL)
#define ciHAMarkerGetNodeSize(h)   ((h) ?  (h)->nodesize : 1)
#define ciHAMarkerGetNodeStyle(h)  ((h) ?  (h)->nodesymbol : 2)
#define ciHAMarkerGetNodeVis(h)    ((h) ?  (h)->markervis : TRUE)
#define ciHAMarkerGetNodeLabelColor(h)          ((h) ? &(h)->textcolor : NULL)
#define ciHAMarkerGetNodeLabelDecimalCount(h)   ((h) ?  (h)->decimalprecision : 0)
#define ciHAMarkerGetNodeLabelFont(h)           ((h) ? CHRSTR((h)->textfont) : NULL)
#define ciHAMarkerGetNodeLabelPrefixVis(h)      ((h) ?  (h)->nodelabelprefixvis : FALSE)
#define ciHAMarkerGetNodeLabelSize(h)           ((h) ?  (h)->textsize : 1)
#define ciHAMarkerGetNodeLabelStrokeWidth(h)    ((h) ?  (h)->textstrokewidth : 1)
#define ciHAMarkerGetNodeLabelTypeAt(h,i)       (ciHALineOrNodeLabelGetTypeAt((h), (i), TRUE))
#define ciHAMarkerGetNodeLabelTypeAsCharAt(h,i) (ciHALineOrNodeLabelGetTypeAsCharAt((h), (i), TRUE))
#define ciHAMarkerGetNodeLabelVisAt(h,i)        ((h) && (i)>=0 && (i)<MAX_NODE_LABEL ? (h)->nodelabelvis[(i)] : FALSE)


/* HA -- POLYLINE and POLYGON */



int         ciHAPolyGetArrowNodeCount(HA *ha);
double      ciHAPolyGetArrowSize(HA *ha);
int         ciHAPolyGetArrowStyle(HA *ha);
int         ciHAPolyGetArrowVis(HA *ha);
RGB        *ciHAPolyGetFaceColor(HA *ha);
RGB        *ciHAPolyGetFacePatternColor(HA *ha);
const char *ciHAPolyGetFacePatternString(HA *ha);
int         ciHAPolyGetFaceVis(HA *ha);
int         ciHAPolyGetGlobalVis(HA *ha);
RGB        *ciHAPolyGetLineColor(HA *ha);
int         ciHAPolyGetLineStyle(HA *ha);
int         ciHAPolyGetLineVis(HA *ha);
int         ciHAPolyGetLineSize(HA *ha);
RGB        *ciHAPolyGetNodeColor(HA *ha);                 
double      ciHAPolyGetNodeSize(HA *ha);                  
int         ciHAPolyGetNodeStyle(HA *ha);                
int         ciHAPolyGetNodeVis(HA *ha);
RGB        *ciHAPolyGetLineLabelColor(HA *ha);
int         ciHAPolyGetLineLabelDecimalCount(HA *ha);
double      ciHAPolyGetLineLabelDist(HA *ha);
double      ciHAPolyGetLineLabelDistFirst(HA *ha);
const char *ciHAPolyGetLineLabelFont(HA *ha);
double      ciHAPolyGetLineLabelOffset(HA *ha);
int         ciHAPolyGetLineLabelPrefixVis(HA *ha);
double      ciHAPolyGetLineLabelSize(HA *ha);
int         ciHAPolyGetLineLabelStrokeWidth(HA *ha);
int         ciHAPolyGetLineLabelTypeAt(HA *ha, int idx);
const char *ciHAPolyGetLineLabelTypeAsCharAt(HA *ha, int idx);
int         ciHAPolyGetLineLabelVisAt(HA *ha, int idx);
RGB        *ciHAPolyGetNodeLabelColor(HA *ha);
int         ciHAPolyGetNodeLabelDecimalCount(HA *ha);
const char *ciHAPolyGetNodeLabelFont(HA *ha);
int         ciHAPolyGetNodeLabelPrefixVis(HA *ha);
double      ciHAPolyGetNodeLabelSize(HA *ha);
int         ciHAPolyGetNodeLabelStrokeWidth(HA *ha);
int        *ciHAPolyGetNodeLabelTypeAt(HA *ha,int idx);
int        *ciHAPolyGetNodeLabelTypeAsCharAt(HA *ha,int idx);
int         ciHAPolyGetNodeLabelVisAt(HA *ha, int idx);


#define ciHAPolyGetArrowNodeCount(h)    ((h) ?  (h)->arrownodecount : 0)
#define ciHAPolyGetArrowSize(h)         ((h) ?  (h)->arrowsize : 1)
#define ciHAPolyGetArrowStyle(h)        ((h) ?  (h)->arrowstyle : 0)
#define ciHAPolyGetArrowVis(h)          ((h) ?  (h)->polylinearrowvis : FALSE)
#define ciHAPolyGetFaceColor(h)         ((h) ? &(h)->facecolor : NULL)
#define ciHAPolyGetFacePatternColor(h)  ((h) ? &(h)->contrastcolor : NULL)
#define ciHAPolyGetFacePatternString(h) ((h) ? CHRSTR((h)->facepatternstring) : NULL)
#define ciHAPolyGetFaceVis(h)           ((h) ?  (h)->polylinefacevis : FALSE)
#define ciHAPolyGetGlobalVis(h)         ((h) ?  (h)->polylinevis : TRUE)
#define ciHAPolyGetLineColor(h)         ((h) ? &(h)->linecolor : NULL)
#define ciHAPolyGetLineStyle(h)         ((h) ?  (h)->linepattern : 0)
#define ciHAPolyGetLineVis(h)           ((h) ?  (h)->linevis : FALSE)
#define ciHAPolyGetLineSize(h)          ((h) ?  (int) ((h)->linewidth + .45) : 1)
#define ciHAPolyGetNodeColor(h)         ((h) ? &(h)->nodecolor : NULL)
#define ciHAPolyGetNodeSize(h)          ((h) ?  (h)->nodesize : 1)
#define ciHAPolyGetNodeStyle(h)         ((h) ?  (h)->nodesymbol : 2)
#define ciHAPolyGetNodeVis(h)           ((h) ?  (h)->polylinenodevis : FALSE)
#define ciHAPolyGetLineLabelColor(h)          ((h) ? &(h)->textcolor : NULL)
#define ciHAPolyGetLineLabelDecimalCount(h)   ((h) ?  (h)->decimalprecision : 0)
#define ciHAPolyGetLineLabelDist(h)           ((h) ?  (h)->linelabeldist : 200)
#define ciHAPolyGetLineLabelDistFirst(h)      ((h) ?  (h)->linelabeldistfirst : 1)
#define ciHAPolyGetLineLabelFont(h)           ((h) ? CHRSTR((h)->textfont) : NULL)
#define ciHAPolyGetLineLabelOffset(h)         ((h) ?  (h)->linelabeloffset : .5)
#define ciHAPolyGetLineLabelPrefixVis(h)      ((h) ?  (h)->nodelabelprefixvis : FALSE)
#define ciHAPolyGetLineLabelSize(h)           ((h) ?  (h)->textsize : 1)
#define ciHAPolyGetLineLabelStrokeWidth(h)    ((h) ?  (h)->textstrokewidth : 1)
#define ciHAPolyGetLineLabelTypeAt(h,i)       (ciHALineOrNodeLabelGetTypeAt((h), (i), FALSE))
#define ciHAPolyGetLineLabelTypeAsCharAt(h,i) (ciHALineOrNodeLabelGetTypeAsCharAt((h), (i), FALSE))
#define ciHAPolyGetLineLabelVisAt(h,i)        ((h) && (i)>=0 && (i)<MAX_LINE_LABEL ? (h)->linelabelvis[(i)] : FALSE)
#define ciHAPolyGetNodeLabelColor(h)          ((h) ? &(h)->textcolor : NULL)
#define ciHAPolyGetNodeLabelDecimalCount(h)   ((h) ?  (h)->decimalprecision : 0)
#define ciHAPolyGetNodeLabelFont(h)           ((h) ? CHRSTR((h)->textfont) : NULL)
#define ciHAPolyGetNodeLabelPrefixVis(h)      ((h) ?  (h)->nodelabelprefixvis : FALSE)
#define ciHAPolyGetNodeLabelSize(h)           ((h) ?  (h)->textsize : 1)
#define ciHAPolyGetNodeLabelStrokeWidth(h)    ((h) ?  (h)->textstrokewidth : 1)
#define ciHAPolyGetNodeLabelTypeAt(h,i)       (ciHALineOrNodeLabelGetTypeAt((h), (i), TRUE))
#define ciHAPolyGetNodeLabelTypeAsCharAt(h,i) (ciHALineOrNodeLabelGetTypeAsCharAt((h), (i), TRUE))
#define ciHAPolyGetNodeLabelVisAt(h,i)        ((h) && (i)>=0 && (i)<MAX_NODE_LABEL ? (h)->nodelabelvis[(i)] : FALSE)


/* HA -- SHELL */



RGB        *ciHAShellGetFaceColor(HA *ha);
RGB        *ciHAShellGetFacePatternColor(HA *ha);
const char *ciHAShellGetFacePatternString(HA *ha);
int         ciHAShellGetFaceVis(HA *ha);
int         ciHAShellGetGlobalVis(HA *ha);
RGB        *ciHAShellGetLineColor(HA *ha);
int         ciHAShellGetLineStyle(HA *ha);
int         ciHAShellGetLineVis(HA *ha);
int         ciHAShellGetLineSize(HA *ha);
RGB        *ciHAShellGetNodeColor(HA *ha);                 
double      ciHAShellGetNodeSize(HA *ha);                  
int         ciHAShellGetNodeStyle(HA *ha);                
int         ciHAShellGetNodeVis(HA *ha);
RGB        *ciHAShellGetNodeLabelColor(HA *ha);
int         ciHAShellGetNodeLabelDecimalCount(HA *ha);
const char *ciHAShellGetNodeLabelFont(HA *ha);
int         ciHAShellGetNodeLabelPrefixVis(HA *ha);
double      ciHAShellGetNodeLabelSize(HA *ha);
int         ciHAShellGetNodeLabelStrokeWidth(HA *ha);
int        *ciHAShellGetNodeLabelTypeAt(HA *ha,int idx);
int        *ciHAShellGetNodeLabelTypeAsCharAt(HA *ha,int idx);
int         ciHAShellGetNodeLabelVisAt(HA *ha, int idx);


#define ciHAShellGetFaceColor(h)         ((h) ? &(h)->facecolor : NULL)
#define ciHAShellGetFacePatternColor(h)  ((h) ? &(h)->contrastcolor : NULL)
#define ciHAShellGetFacePatternString(h) ((h) ? CHRSTR((h)->facepatternstring) : NULL)
#define ciHAShellGetFaceVis(h)           ((h) ?  (h)->surfacefacevis : FALSE)
#define ciHAShellGetGlobalVis(h)         ((h) ?  (h)->surfacevis : TRUE)
#define ciHAShellGetLineColor(h)         ((h) ? &(h)->linecolor : NULL)
#define ciHAShellGetLineStyle(h)         ((h) ?  (h)->linepattern : 0)
#define ciHAShellGetLineVis(h)           ((h) ?  (h)->edgevis : TRUE)
#define ciHAShellGetLineSize(h)          ((h) ?  (int) ((h)->linewidth + .45) : 1)
#define ciHAShellGetNodeColor(h)         ((h) ? &(h)->nodecolor : NULL)
#define ciHAShellGetNodeSize(h)          ((h) ?  (h)->nodesize : 1)
#define ciHAShellGetNodeStyle(h)         ((h) ?  (h)->nodesymbol : 2)
#define ciHAShellGetNodeVis(h)           ((h) ?  (h)->nodevis : FALSE)
#define ciHAShellGetNodeLabelColor(h)          ((h) ? &(h)->textcolor : NULL)
#define ciHAShellGetNodeLabelDecimalCount(h)   ((h) ?  (h)->decimalprecision : 0)
#define ciHAShellGetNodeLabelFont(h)           ((h) ? CHRSTR((h)->textfont) : NULL)
#define ciHAShellGetNodeLabelPrefixVis(h)      ((h) ?  (h)->nodelabelprefixvis : FALSE)
#define ciHAShellGetNodeLabelSize(h)           ((h) ?  (h)->textsize : 1)
#define ciHAShellGetNodeLabelStrokeWidth(h)    ((h) ?  (h)->textstrokewidth : 1)
#define ciHAShellGetNodeLabelTypeAt(h,i)       (ciHALineOrNodeLabelGetTypeAt((h), (i), TRUE))
#define ciHAShellGetNodeLabelTypeAsCharAt(h,i) (ciHALineOrNodeLabelGetTypeAsCharAt((h), (i), TRUE))
#define ciHAShellGetNodeLabelVisAt(h,i)        ((h) && (i)>=0 && (i)<MAX_NODE_LABEL ? (h)->nodelabelvis[(i)] : FALSE)


/* HA -- LABEL */

int         ciHALabelGetAlign(HA *ha);
int         ciHALabelGetAlignHorz(HA *ha);
int         ciHALabelGetAlignVert(HA *ha);
RGB        *ciHALabelGetColor(HA *ha);
const char *ciHALabelGetFont(HA *ha);
int         ciHALabelGetGlobalVis(HA *ha);
double      ciHALabelGetSize(HA *ha);
int         ciHALabelGetStrokeWidth(HA *ha);
int         ciHALabelGetVis(HA *ha);

#define ciHALabelGetAlign(h)       ((h) ?  (h)->textalign : 4)
#define ciHALabelGetAlignHorz(h)   (HORZALIGN(ciHALabelGetAlign((h))))
#define ciHALabelGetAlignVert(h)   (VERTALIGN(ciHALabelGetAlign((h))))
#define ciHALabelGetColor(h)       ((h) ? &(h)->textcolor : NULL)
#define ciHALabelGetFont(h)        ((h) ? CHRSTR((h)->textfont) : NULL)
#define ciHALabelGetGlobalVis(h)   ((h) ?  (h)->globaltextvis : TRUE)
#define ciHALabelGetSize(h)        ((h) ?  (h)->textsize : 1)
#define ciHALabelGetStrokeWidth(h) ((h) ?  (h)->textstrokewidth : 1)
#define ciHALabelGetVis(h)         ((h) ?  (h)->textvis : TRUE)

/* HA -- Line and Node Label if you don't like specifying          */
/*       the element type in the function name. for example:       */
/*                                                                 */
/*       ciHAGetNodeLabelColor == ciHAMarkerGetNodeLabelColor ==   */
/*       ciHAPolyGetNodeLabelColor, etc.                           */


RGB        *ciHAGetLineLabelColor(HA *ha);
int         ciHAGetLineLabelDecimalCount(HA *ha);
double      ciHAGetLineLabelDist(HA *ha);
double      ciHAGetLineLabelDistFirst(HA *ha);
const char *ciHAGetLineLabelFont(HA *ha);
double      ciHAGetLineLabelOffset(HA *ha);
int         ciHAGetLineLabelPrefixVis(HA *ha);
double      ciHAGetLineLabelSize(HA *ha);
int         ciHAGetLineLabelStrokeWidth(HA *ha);
int         ciHAGetLineLabelTypeAt(HA *ha, int idx);
const char *ciHAGetLineLabelTypeAsCharAt(HA *ha, int idx);
int         ciHAGetLineLabelVisAt(HA *ha, int idx);
RGB        *ciHAGetNodeLabelColor(HA *ha);
int         ciHAGetNodeLabelDecimalCount(HA *ha);
const char *ciHAGetNodeLabelFont(HA *ha);
int         ciHAGetNodeLabelPrefixVis(HA *ha);
double      ciHAGetNodeLabelSize(HA *ha);
int         ciHAGetNodeLabelStrokeWidth(HA *ha);
int        *ciHAGetNodeLabelTypeAt(HA *ha,int idx);
const char *ciHAGetNodeLabelTypeAsCharAt(HA *ha, int idx);
int         ciHAGetNodeLabelVisAt(HA *ha, int idx);

#define ciHAGetLineLabelColor(h)          ((h) ? &(h)->textcolor : NULL)
#define ciHAGetLineLabelDecimalCount(h)   ((h) ?  (h)->decimalprecision : 0)
#define ciHAGetLineLabelDist(h)           ((h) ?  (h)->linelabeldist : 200)
#define ciHAGetLineLabelDistFirst(h)      ((h) ?  (h)->linelabeldistfirst : 1)
#define ciHAGetLineLabelFont(h)           ((h) ? CHRSTR((h)->textfont) : NULL)
#define ciHAGetLineLabelOffset(h)         ((h) ?  (h)->linelabeloffset : .5)
#define ciHAGetLineLabelPrefixVis(h)      ((h) ?  (h)->nodelabelprefixvis : FALSE)
#define ciHAGetLineLabelSize(h)           ((h) ?  (h)->textsize : 1)
#define ciHAGetLineLabelStrokeWidth(h)    ((h) ?  (h)->textstrokewidth : 1)
#define ciHAGetLineLabelTypeAt(h,i)       (ciHALineOrNodeLabelGetTypeAt((h), (i), FALSE))
#define ciHAGetLineLabelTypeAsCharAt(h,i) (ciHALineOrNodeLabelGetTypeAsCharAt((h), (i), FALSE))
#define ciHAGetLineLabelVisAt(h,i)        ((h) && (i)>=0 && (i)<MAX_LINE_LABEL ? (h)->linelabelvis[(i)] : FALSE)
#define ciHAGetNodeLabelColor(h)          ((h) ? &(h)->textcolor : NULL)
#define ciHAGetNodeLabelDecimalCount(h)   ((h) ?  (h)->decimalprecision : 0)
#define ciHAGetNodeLabelFont(h)           ((h) ? CHRSTR((h)->textfont) : NULL)
#define ciHAGetNodeLabelPrefixVis(h)      ((h) ?  (h)->nodelabelprefixvis : FALSE)
#define ciHAGetNodeLabelSize(h)           ((h) ?  (h)->textsize : 1)
#define ciHAGetNodeLabelStrokeWidth(h)    ((h) ?  (h)->textstrokewidth : 1)
#define ciHAGetNodeLabelTypeAt(h,i)       (ciHALineOrNodeLabelGetTypeAt((h), (i), TRUE))
#define ciHAGetNodeLabelTypeAsCharAt(h,i) (ciHALineOrNodeLabelGetTypeAsCharAt((h), (i), TRUE))
#define ciHAGetNodeLabelVisAt(h,i)        ((h) && (i)>=0 && (i)<MAX_NODE_LABEL ? (h)->nodelabelvis[(i)] : FALSE)
msEND_EXTERN_C
#endif
