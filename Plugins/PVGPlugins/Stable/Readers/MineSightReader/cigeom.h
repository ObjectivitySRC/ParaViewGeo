/*
 * Copyright 2007 Mintec, Inc.
 * All Rights Reserved.
 *
 * Minesight: cigeom.h
 *
 * Description: 
 *    header for the geometry module in libmsci
 */
#ifndef CIGEOM_HEADER
#define CIGEOM_HEADER

#include <msextern_c.h>

#include "ci.h"
#include "ciha.h"

msBEGIN_EXTERN_C

typedef enum {
   shownNONE          = 0,
   shownARROWS        = 0x0001,
   shownNODES         = 0x0002,
   shownLINES         = 0x0004,
   shownFACES         = 0x0008,
   shownTEXT          = 0x0010,
   shownLINE_LABELS   = 0x0020,
   shownNODE_LABELS   = 0x0040
};

typedef enum {
   resUNKNOWN,
   resPOLYLINE,
   resPOLYGON,
   resMARKER,
   resSHELLPTS,
   resSHELLFACES,
   resTLABEL,
   resLABEL,
   resLARGEPTS,
   resLARGEFACES
};

typedef enum {
   verifyOK                     = 0,
   verifyNULL                   = 0x0001,
   verifyBAD_TYPE               = 0x0002,
   verifyBAD_PLANE              = 0x0004,
   verifyELEMENT_TOO_FEW_POINTS = 0x0008,
   verifyELEMENT_NO_POINTS      = 0x0010,
   verifyELEMENT_NO_FACES       = 0x0040,
   verifyLABEL_NO_TEXT          = 0x0100,
   verifyLABEL_TOO_MUCH_TEXT    = 0x0200,
   verifyLABEL_NO_MATRIX        = 0x0400,
   verifyNAME_RESTRICTED        = 0x1000,
   verifyMATERIAL_BAD_NAME      = 0x4000,
};

typedef enum {
   elemUNKNOWN,
   elemPOLYLINE,
   elemPOLYGON,
   elemMARKER,
   elemSHELL,
   elemCOUNT
};

typedef enum {
   labelUNKNOWN,
   labelTRANS,      /* fixed */
   labelSEMITRANS,  /* forward */
   labelNONTRANS,   /* upright */
   labelCOUNT
};

typedef struct {
   int i;
   int j;
   int k;
} FACE;

typedef enum {
   dimANY,
   dim2,
   dim3
};

typedef struct {
   int           order;
   int           type;
   const Sname  *name;
   const Sname  *material;
   int           shown;
   PLANE         plane;
   int           npt;
   XYZ          *pts;
   int           nface;
   FACE         *faces;
} ELEMENT;

typedef struct {
   int           order;
   int           type;
   const Sname  *atname;
   const Sname  *name;
   const Sname  *material;
   int           shown;
   PLANE         plane;
   XYZ           pos[3];
   int           ntext;
   const Sname **text;
   float        *matrix;
} LABEL;

typedef struct {
   XYZ          origin;
   Snametable  *nt;
   const Sname *path;
   const Sname *name;
   const Sname *defmat;
   ciDict      *elemdict;
   ciDict      *labeldict;
   ciDict      *hadict;
} GEOM;

#define PLANEIS2D(p)     ((p) ? (p)->a!=0 || (p)->b!=0 || (p)->c!=0 || (p)->d!=0 : FALSE)

#define MIN_POLYLINE  2
#define MIN_POLYGON   2 
#define MIN_SHELL     3
#define MIN_MARKER    1
#define MIN_FACE      1
#define ORDER_UNSET  -1
#define DUMP_NO_POINTS   -1
#define DUMP_MANY_POINTS  1000


/* GEOM -- GENERAL */


LIBMSCI_API void         ciGEOMStartUp(int argc, char **argv);

            /* object info */

LIBMSCI_API GEOM        *ciGEOMObjectLoad(char *path);
LIBMSCI_API void         ciGEOMObjectFree(GEOM *geom);
            const char  *ciGEOMObjectGetPath(GEOM *geom);
            const char  *ciGEOMObjectGetName(GEOM *geom);
            const char  *ciGEOMObjectGetMaterial(GEOM *geom);
            XYZ         *ciGEOMObjectGetOrigin(GEOM *geom);
LIBMSCI_API HA          *ciGEOMObjectGetHA(GEOM *geom);

            /* dump function */

LIBMSCI_API void         ciGEOMDumpSummary(GEOM *geom, FILE *fp);
LIBMSCI_API void         ciGEOMDumpElements(GEOM *geom, FILE *fp, int maxpts);
LIBMSCI_API void         ciGEOMDumpLabels(GEOM *geom, FILE *fp);
LIBMSCI_API void         ciGEOMDumpHAs(GEOM *geom, FILE *fp);
LIBMSCI_API void         ciGEOMLabelDump(LABEL *label, FILE *fp);
LIBMSCI_API void         ciGEOMElementDump(ELEMENT *elem, FILE *fp, int maxpts);


            /* verification functions */

LIBMSCI_API int          ciGEOMVerifyName(const char *chr);
LIBMSCI_API int          ciGEOMVerifyMaterial(const char *chr);
LIBMSCI_API int          ciGEOMVerifyPlane(PLANE *plane);
LIBMSCI_API const char  *ciGEOMVerifyMessage(GEOM *geom, int status);


            /*geometry element info */

            ciDict      *ciGEOMGetElementDict(GEOM *geom);
LIBMSCI_API int          ciGEOMGetElementCount(GEOM *geom);
LIBMSCI_API int          ciGEOMGetElementCountByType(GEOM *geom, int type, int dim);
            int          ciGEOMGetElementPolylineCount(GEOM *geom);
            int          ciGEOMGetElementPolygonCount(GEOM *geom);
            int          ciGEOMGetElementShellCount(GEOM *geom);
            int          ciGEOMGetElementMarkerCount(GEOM *geom);
            int          ciGEOMGetElement2DPolylineCount(GEOM *geom);
            int          ciGEOMGetElement2DPolygonCount(GEOM *geom);
            int          ciGEOMGetElement2DShellCount(GEOM *geom);
            int          ciGEOMGetElement2DMarkerCount(GEOM *geom);
            int          ciGEOMGetElement3DPolylineCount(GEOM *geom);
            int          ciGEOMGetElement3DPolygonCount(GEOM *geom);
            int          ciGEOMGetElement3DShellCount(GEOM *geom);
            int          ciGEOMGetElement3DMarkerCount(GEOM *geom);

            /* label element info */

            ciDict      *ciGEOMGetLabelDict(GEOM *geom);
LIBMSCI_API int          ciGEOMGetLabelCount(GEOM *geom);
LIBMSCI_API int          ciGEOMGetLabelCountByType(GEOM *geom, int type, int dim);
            int          ciGEOMGetLabelFixedCount(GEOM *geom);
            int          ciGEOMGetLabelForwardCount(GEOM *geom);
            int          ciGEOMGetLabelUprightCount(GEOM *geom);
            int          ciGEOMGetLabel2DFixedCount(GEOM *geom);
            int          ciGEOMGetLabel2DForwardCount(GEOM *geom);
            int          ciGEOMGetLabel2DUprightCount(GEOM *geom);
            int          ciGEOMGetLabel3DFixedCount(GEOM *geom);
            int          ciGEOMGetLabel3DForwardCount(GEOM *geom);
            int          ciGEOMGetLabel3DUprightCount(GEOM *geom);

            /* HA info */

LIBMSCI_API int          ciGEOMGetHACount(GEOM *geom);
LIBMSCI_API HA          *ciGEOMGetHA(GEOM *geom, const char *material);
            ciDict      *ciGEOMGetHADict(GEOM *geom);


#define ciGEOMObjectGetPath(g)     ((g) ? CHRSTR((g)->path) : NULL)
#define ciGEOMObjectGetName(g)     ((g) ? CHRSTR((g)->name) : NULL)
#define ciGEOMObjectGetMaterial(g) ((g) ? CHRSTR((g)->defmat) : NULL)
#define ciGEOMObjectGetOrigin(g)   ((g) ? &((g)->origin) : NULL)
#define ciGEOMGetElementDict(g)    ((g) ? &((g)->elemdict) : NULL)
#define ciGEOMGetLabelDict(g)      ((g) ? &((g)->labeldict) : NULL)
#define ciGEOMGetHADict(g)         ((g) ? &((g)->hadict) : NULL)

#define ciGEOMGetElementPolylineCount(g)   ((g) ? ciGEOMGetElementCountByType((g), elemPOLYLINE, dimANY) : 0)
#define ciGEOMGetElementPolygonCount(g)    ((g) ? ciGEOMGetElementCountByType((g), elemPOLYGON, dimANY) : 0)
#define ciGEOMGetElementShellCount(g)      ((g) ? ciGEOMGetElementCountByType((g), elemSHELL, dimANY) : 0)
#define ciGEOMGetElementMarkerCount(g)     ((g) ? ciGEOMGetElementCountByType((g), elemMARKER, dimANY) : 0)
#define ciGEOMGetElement2DPolylineCount(g) ((g) ? ciGEOMGetElementCountByType((g), elemPOLYLINE, dim2) : 0)
#define ciGEOMGetElement2DPolygonCount(g)  ((g) ? ciGEOMGetElementCountByType((g), elemPOLYGON, dim2) : 0)
#define ciGEOMGetElement2DShellCount(g)    ((g) ? ciGEOMGetElementCountByType((g), elemSHELL, dim2) : 0)
#define ciGEOMGetElement2DMarkerCount(g)   ((g) ? ciGEOMGetElementCountByType((g), elemMARKER, dim2) : 0)
#define ciGEOMGetElement3DPolylineCount(g) ((g) ? ciGEOMGetElementCountByType((g), elemPOLYLINE, dim3) : 0)
#define ciGEOMGetElement3DPolygonCount(g)  ((g) ? ciGEOMGetElementCountByType((g), elemPOLYGON, dim3) : 0)
#define ciGEOMGetElement3DShellCount(g)    ((g) ? ciGEOMGetElementCountByType((g), elemSHELL, dim3) : 0)
#define ciGEOMGetElement3DMarkerCount(g)   ((g) ? ciGEOMGetElementCountByType((g), elemMARKER, dim3) : 0)

#define ciGEOMGetLabelFixedCount(g)        ((g) ? ciGEOMGetLabelCountByType((g), labelTRANS, dimANY) : 0)
#define ciGEOMGetLabelForwardCount(g)      ((g) ? ciGEOMGetLabelCountByType((g), labelSEMITRANS, dimANY) : 0)
#define ciGEOMGetLabelUprightCount(g)      ((g) ? ciGEOMGetLabelCountByType((g), labelNONTRANS, dimANY) : 0)
#define ciGEOMGetLabel2DFixedCount(g)      ((g) ? ciGEOMGetLabelCountByType((g), labelTRANS, dim2) : 0)
#define ciGEOMGetLabel2DForwardCount(g)    ((g) ? ciGEOMGetLabelCountByType((g), labelSEMITRANS, dim2) : 0)
#define ciGEOMGetLabel2DUprightCount(g)    ((g) ? ciGEOMGetLabelCountByType((g), labelNONTRANS, dim2) : 0)
#define ciGEOMGetLabel3DFixedCount(g)      ((g) ? ciGEOMGetLabelCountByType((g), labelTRANS, dim3) : 0)
#define ciGEOMGetLabel3DForwardCount(g)    ((g) ? ciGEOMGetLabelCountByType((g), labelSEMITRANS, dim3) : 0)
#define ciGEOMGetLabel3DUprightCount(g)    ((g) ? ciGEOMGetLabelCountByType((g), labelNONTRANS, dim3) : 0)


/* ELEMENT -- INDIVIDUAL */


            FACE        *ciGEOMElementGetFaces(ELEMENT *elem);
            int          ciGEOMElementGetFaceCount(ELEMENT *elem);
            HA          *ciGEOMElementGetHA(GEOM *geom, ELEMENT *elem);
            const char  *ciGEOMElementGetMaterial(ELEMENT *elem);
            const char  *ciGEOMElementGetName(ELEMENT *elem);
            PLANE       *ciGEOMElementGetPlane(ELEMENT *elem);
            int          ciGEOMElementGetPointCount(ELEMENT *elem);
            XYZ         *ciGEOMElementGetPoints(ELEMENT *elem);
            int          ciGEOMElementGetType(ELEMENT *elem);
            int          ciGEOMElementIsMarker(ELEMENT *elem);
            int          ciGEOMElementIsPolyline(ELEMENT *elem);
            int          ciGEOMElementIsPolygon(ELEMENT *elem);
            int          ciGEOMElementIsShell(ELEMENT *elem);
            int          ciGEOMElementIs2D(ELEMENT *elem);
LIBMSCI_API int          ciGEOMElementVerify(ELEMENT *elem);
            int          ciGEOMElementNodesAreShown(ELEMENT *elem);
            int          ciGEOMElementLinesAreShown(ELEMENT *elem);
            int          ciGEOMElementFacesAreShown(ELEMENT *elem);
            int          ciGEOMElementArrowsAreShown(ELEMENT *elem);
            int          ciGEOMElementLineLabelsAreShown(ELEMENT *elem);
            int          ciGEOMElementNodeLabelsAreShown(ELEMENT *elem);


#define ciGEOMElementIs2D(e)          ((e) ? PLANEIS2D(&(e)->plane) : FALSE)
#define ciGEOMElementIsPolyline(e)    ((e) ? (e)->type==elemPOLYLINE : FALSE)
#define ciGEOMElementIsPolygon(e)     ((e) ? (e)->type==elemPOLYGON : FALSE)
#define ciGEOMElementIsShell(e)       ((e) ? (e)->type==elemSHELL : FALSE)
#define ciGEOMElementIsMarker(e)      ((e) ? (e)->type==elemMARKER : FALSE)
#define ciGEOMElementGetType(e)       ((e) ? (e)->type : elemUNKNOWN)
#define ciGEOMElementGetName(e)       ((e) ? CHRSTR((e)->name) : NULL)
#define ciGEOMElementGetMaterial(e)   ((e) ? CHRSTR((e)->material) : NULL)
#define ciGEOMElementGetPlane(e)      ((e) ? &((e)->plane) : NULL)
#define ciGEOMElementGetPointCount(e) ((e) ? (e)->npt : 0)
#define ciGEOMElementGetPoints(e)     ((e) ? (e)->pts : NULL)
#define ciGEOMElementGetFaceCount(e)  ((e) ? (e)->nface : 0)
#define ciGEOMElementGetFaces(e)      ((e) ? (e)->faces : NULL)
#define ciGEOMElementGetHA(g,e)       ((g) && (e) ? ciGEOMGetHA((g), ciGEOMElementGetMaterial((e))) : NULL)
#define ciGEOMElementNodesAreShown(e)       ((e) ? (e)->shown & shownNODES : FALSE)
#define ciGEOMElementLinesAreShown(e)       ((e) ? (e)->shown & shownLINES : FALSE)
#define ciGEOMElementFacesAreShown(e)       ((e) ? (e)->shown & shownFACES : FALSE)
#define ciGEOMElementArrowsAreShown(e)      ((e) ? (e)->shown & shownARROWS : FALSE)
#define ciGEOMElementLineLabelsAreShown(e)  ((e) ? (e)->shown & shownLINE_LABELS : FALSE)
#define ciGEOMElementNodeLabelsAreShown(e)  ((e) ? (e)->shown & shownNODE_LABELS : FALSE)



/* ELEMENT BY INDEX */



LIBMSCI_API ELEMENT     *ciGEOMElementGetAt(GEOM *geom, int idx);
            FACE        *ciGEOMElementGetFacesAt(GEOM *geom, int idx);
            int          ciGEOMElementGetFaceCountAt(GEOM *geom, int idx);
            HA          *ciGEOMElementGetHAAt(GEOM *geom, int idx);
            const char  *ciGEOMElementGetMaterialAt(GEOM *geom, int idx);
            const char  *ciGEOMElementGetNameAt(GEOM *geom, int idx);
            PLANE       *ciGEOMElementGetPlaneAt(GEOM *geom, int idx);
            int          ciGEOMElementGetPointCountAt(GEOM *geom, int idx);
            XYZ         *ciGEOMElementGetPointsAt(GEOM *geom, int idx);
            int          ciGEOMElementGetTypeAt(GEOM *geom, int idx);
            int          ciGEOMElementIsMarkerAt(GEOM *geom, int idx);
            int          ciGEOMElementIsPolylineAt(GEOM *geom, int idx);
            int          ciGEOMElementIsPolygonAt(GEOM *geom, int idx);
            int          ciGEOMElementIsShellAt(GEOM *geom, int idx);
            int          ciGEOMElementIs2DAt(GEOM *geom, int idx);
            int          ciGEOMElementVerifyAt(GEOM *geom, int idx);
            int          ciGEOMElementNodesAreShownAt(GEOM *geom, int idx);
            int          ciGEOMElementLinesAreShownAt(GEOM *geom, int idx); 
            int          ciGEOMElementFacesAreShownAt(GEOM *geom, int idx); 
            int          ciGEOMElementArrowsAreShownAt(GEOM *geom, int idx); 
            int          ciGEOMElementLineLabelsAreShownAt(GEOM *geom, int idx); 
            int          ciGEOMElementNodeLabelsAreShownAt(GEOM *geom, int idx); 

#define ciGEOMElementIsPolylineAt(g,i)     ((g) ? ciGEOMElementIsPolyline(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementIsPolygonAt(g,i)      ((g) ? ciGEOMElementIsPolygon(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementIsShellAt(g,i)        ((g) ? ciGEOMElementIsShell(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementIsMarkerAt(g,i)       ((g) ? ciGEOMElementIsMarker(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementIs2DAt(g,i)           ((g) ? ciGEOMElementIs2D(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementGetTypeAt(g,i)        ((g) ? ciGEOMElementGetType(ciGEOMElementGetAt((g),(i))) : elemUNKNOWN)
#define ciGEOMElementGetNameAt(g,i)        ((g) ? ciGEOMElementGetName(ciGEOMElementGetAt((g),(i))) : NULL)
#define ciGEOMElementGetMaterialAt(g,i)    ((g) ? ciGEOMElementGetMaterial(ciGEOMElementGetAt((g),(i))) : NULL)
#define ciGEOMElementGetPlaneAt(g,i)       ((g) ? ciGEOMElementGetPlane(ciGEOMElementGetAt((g),(i))) : NULL)
#define ciGEOMElementGetPointCountAt(g,i)  ((g) ? ciGEOMElementGetPointCount(ciGEOMElementGetAt((g),(i))) : 0)
#define ciGEOMElementGetPointsAt(g,i)      ((g) ? ciGEOMElementGetPoints(ciGEOMElementGetAt((g),(i))) : NULL)
#define ciGEOMElementGetFaceCountAt(g,i)   ((g) ? ciGEOMElementGetFaceCount(ciGEOMElementGetAt((g),(i))) : 0)
#define ciGEOMElementGetFacesAt(g,i)       ((g) ? ciGEOMElementGetFaces(ciGEOMElementGetAt((g),(i))) : NULL)
#define ciGEOMElementVerifyAt(g,i)         ((g) ? ciGEOMElementVerify(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementGetHAAt(g,i)          ((g) ? ciGEOMGetHA((g), ciGEOMElementGetMaterialAt((g),(i))) : NULL)
#define ciGEOMElementNodesAreShownAt(g,i)       ((g) ? ciGEOMElementNodesAreShown(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementLinesAreShownAt(g,i)       ((g) ? ciGEOMElementLinesAreShown(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementFacesAreShownAt(g,i)       ((g) ? ciGEOMElementFacesAreShown(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementArrowsAreShownAt(g,i)      ((g) ? ciGEOMElementArrowsAreShown(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementLineLabelsAreShownAt(g,i)  ((g) ? ciGEOMElementLineLabelsAreShown(ciGEOMElementGetAt((g),(i))) : FALSE)
#define ciGEOMElementNodeLabelsAreShownAt(g,i)  ((g) ? ciGEOMElementNodeLabelsAreShown(ciGEOMElementGetAt((g),(i))) : FALSE)


/* LABEL -- INDIVIDUAL */


            HA          *ciGEOMLabelGetHA(GEOM *geom, LABEL *label);
            const char  *ciGEOMLabelGetMaterial(LABEL *label);
            float       *ciGEOMLabelGetMatrix(LABEL *label);
            const char  *ciGEOMLabelGetName(LABEL *label);
            PLANE       *ciGEOMLabelGetPlane(LABEL *label);
            XYZ         *ciGEOMLabelGetPosition(LABEL *label);
            int         *ciGEOMLabelGetTextCount(LABEL *label);
            const char  *ciGEOMLabelGetTextLine(LABEL *label, int textidx);
            int          ciGEOMLabelGetType(LABEL *label);
            int          ciGEOMLabelIsUpright(LABEL *label);
            int          ciGEOMLabelIsForward(LABEL *label);
            int          ciGEOMLabelIsFixed(LABEL *label);
            int          ciGEOMLabelIs2D(LABEL *label);
LIBMSCI_API int          ciGEOMLabelVerify(LABEL *label);
            int          ciGEOMLabelTextIsShown(LABEL *label);

#define ciGEOMLabelIsFixed(l)           ((l) ? (l)->type==labelTRANS : FALSE)
#define ciGEOMLabelIsForward(l)         ((l) ? (l)->type==labelSEMITRANS : FALSE)
#define ciGEOMLabelIsUpright(l)         ((l) ? (l)->type==labelNONTRANS : FALSE)
#define ciGEOMLabelIs2D(l)              ((l) ? PLANEIS2D(&(l)->plane) : FALSE)
#define ciGEOMLabelGetType(l)           ((l) ? (l)->type : labelUNKNOWN)
#define ciGEOMLabelGetName(l)           ((l) ? CHRSTR((l)->name) : NULL)
#define ciGEOMLabelGetMaterial(l)       ((l) ? CHRSTR((l)->material) : NULL)
#define ciGEOMLabelGetPlane(l)          ((l) ? &((l)->plane) : NULL)
#define ciGEOMLabelGetPosition(l)       ((l) ? (l)->pos : NULL)
#define ciGEOMLabelGetTextCount(l)      ((l) ? (l)->ntext : 0)
#define ciGEOMLabelGetTextLine(l,t)     ((l) && (t)>=0 && (t)<(l)->ntext ? CHRSTR((l)->text[(t)]) : NULL)
#define ciGEOMLabelGetMatrix(l)         ((l) ? (l)->matrix : NULL)
#define ciGEOMLabelGetHA(g,l)           ((g) && (l) ? ciGEOMGetHA((g), ciGEOMLabelGetMaterial((l))) : NULL)
#define ciGEOMLabelTextIsShown(l)       ((l) ? (l)->shown & shownTEXT : FALSE)


/* LABEL -- BY INDEX */


LIBMSCI_API LABEL       *ciGEOMLabelGetAt(GEOM *geom, int idx);
            HA          *ciGEOMLabelGetHAAt(GEOM *geom, int idx);
            const char  *ciGEOMLabelGetMaterialAt(GEOM *geom, int idx);
            float       *ciGEOMLabelGetMatrixAt(GEOM *geom, int idx);
            const char  *ciGEOMLabelGetNameAt(GEOM *geom, int idx);
            PLANE       *ciGEOMLabelGetPlaneAt(GEOM *geom, int idx);
            XYZ         *ciGEOMLabelGetPositionAt(GEOM *geom, int idx);
            int         *ciGEOMLabelGetTextCountAt(GEOM *geom, int idx);
            const char  *ciGEOMLabelGetTextLineAt(GEOM *geom, int idx, int textidx);
            int          ciGEOMLabelGetTypeAt(GEOM *geom, int idx);
            int          ciGEOMLabelIsUprightAt(GEOM *geom, int idx);
            int          ciGEOMLabelIsForwardAt(GEOM *geom, int idx);
            int          ciGEOMLabelIsFixedAt(GEOM *geom, int idx);
            int          ciGEOMLabelIs2DAt(GEOM *geom, int idx);
            int          ciGEOMLabelVerifyAt(GEOM *geom, int idx);
            int          ciGEOMLabelTextIsShownAt(GEOM *geom, int idx);

#define ciGEOMLabelIsFixedAt(g,i)           ((g) ? ciGEOMLabelIsFixed(ciGEOMLabelGetAt((g),(i))) : FALSE)
#define ciGEOMLabelIsForwardAt(g,i)         ((g) ? ciGEOMLabelIsForward(ciGEOMLabelGetAt((g),(i))) : FALSE)
#define ciGEOMLabelIsUprightAt(g,i)         ((g) ? ciGEOMLabelIsUpright(ciGEOMLabelGetAt((g),(i))) : FALSE)
#define ciGEOMLabelIs2DAt(g,i)              ((g) ? ciGEOMLabelIs2D(ciGEOMLabelGetAt((g),(i))) : FALSE)
#define ciGEOMLabelGetTypeAt(g,i)           ((g) ? ciGEOMLabelGetType(ciGEOMLabelGetAt((g),(i))) : labelUNKNOWN)
#define ciGEOMLabelGetNameAt(g,i)           ((g) ? ciGEOMLabelGetName(ciGEOMLabelGetAt((g),(i))) : NULL)
#define ciGEOMLabelGetPlaneAt(g,i)          ((g) ? ciGEOMLabelGetPlane(ciGEOMLabelGetAt((g),(i))) : NULL)
#define ciGEOMLabelGetMaterialAt(g,i)       ((g) ? ciGEOMLabelGetMaterial(ciGEOMLabelGetAt((g),(i))) : NULL)
#define ciGEOMLabelGetPositionAt(g,i)       ((g) ? ciGEOMLabelGetPosition(ciGEOMLabelGetAt((g),(i))) : NULL)
#define ciGEOMLabelGetTextCountAt(g,i)      ((g) ? ciGEOMLabelGetTextCount(ciGEOMLabelGetAt((g),(i))) : 0)
#define ciGEOMLabelGetTextLineAt(g,i,t)     ((g) ? ciGEOMLabelGetTextLine(ciGEOMLabelGetAt((g),(i)), (t)) : NULL)
#define ciGEOMLabelGetMatrixAt(g,i)         ((g) ? ciGEOMLabelGetMatrix(ciGEOMLabelGetAt((g),(i))) : NULL)
#define ciGEOMLabelVerifyAt(g,i)            ((g) ? ciGEOMLabelVerify(ciGEOMLabelGetAt((g),(i))) : FALSE)
#define ciGEOMLabelGetHAAt(g,i)             ((g) ? ciGEOMGetHA((g), ciGEOMLabelGetMaterialAt((g),(i))) : NULL)
#define ciGEOMLabelTextIsShownAt(g,i)       ((g) ? ciGEOMLabelTextIsShown(ciGEOMLabelGetAt((g),(i))) : FALSE)

msEND_EXTERN_C
#endif
