/*
 * Copyright 2007 Mintec, Inc.
 * All Rights Reserved.
 *
 * Minesight: cimsr.h
 *
 * Description: 
 *    header for accessing MS3D resource files in libmsci
 */
#ifndef CIMSR_HEADER
#define CIMSR_HEADER

#include "ci.h"

msBEGIN_EXTERN_C

#define chrMSRESOURCES "\\_msresources"
#define chrMATERIALS   "materials\\"
#define chrDOTMSR      ".msr"


#define chrCLASS      "Class"
#define chrGEOMSEG    "geomseg"
#define chrGRIDSET    "gridset"

#define chrHCOMP      "HComp"
#define chr3D         "##3d"
#define chrPEQ        "##pln_eqn:"
#define chrHATTR      "HAttr"
#define chrHATTRPTR   "*HAttr"
#define chrTEXT       "text"
#define chrTEXTMATRIX "textmatrix"
#define chrTEXTPOS    "textpos"
#define chrTEXTSTRING "textstring"
#define chrMATERIAL   "Material"
#define chrOPTIONS    "options"
#define chrLABELEQ    "label="
#define lenLABELEQ    strlen(chrLABELEQ)
#define chrORIGIN     "origin"
#define chrARCH       "architecture"
#define chrNONTRANS   "non transformed"
#define chrPLIST      "plist"
#define chrAT         "@"
#define chrNTEXT      "ntext"
#define chrLARGEFACES "*Large_Surface_Face_List"
#define chrLARGEPTS   "*Large_Surface_Point_List"
#define chrMARKER     "markers"
#define chrPOLYLINE   "polyline"
#define chrPOLYGON    "polygon"
#define chrSHELLFACES "shellfaces"
#define chrSHELLPTS   "shellpoints"

#define chrAZIMUTH     "azimuth"
#define chrBASEVIS     "baseVisibility"
#define chrCELLXCOUNT  "cellXCount"
#define chrCELLXWIDTH  "cellXWidth"
#define chrCELLYCOUNT  "cellYCount"
#define chrCELLYWIDTH  "cellYWidth"
#define chrDIP         "dip"
#define chrMULTORIENT  "multipleOrientations"
#define chrORIENT      "orientation"
#define chrSORTEDNAMES "SortedNames"
#define chrXBASE       "xBase"
#define chrYBASE       "yBase"
#define chrGRIDS       "grids"
#define chrGRIDCOUNTS  "gridcounts"
#define chrGRIDPOINTS  "gridpoints"

#define chrARROWNODECOUNT      "arrownodecount"
#define chrARROWSIZE           "arrowsize"
#define chrARROWSTYLE          "arrowstyle"
#define chrCONTRASTCOLOR       "contrastcolor"
#define chrCUTPLANEVIS         "cutplaneVis"
#define chrEDGEVIS             "edgevis"
#define chrEDGEHARDVIS         "edgehardvis"
#define chrDECIMALPRECISION    "decimal precision"
#define chrFACECOLOR           "facecolor"
#define chrFACEPATTERN         "facepattern"
#define chrFACEPATTERNSTRING   "face pattern string"
#define chrGLOBALTEXTVIS       "global text visibility"
#define chrLIGHTINTERP         "lightinterp"
#define chrLINELABEL0          "line label 0"
#define chrLINELABEL1          "line label 1"
#define chrLINELABEL2          "line label 2"
#define chrLINELABEL3          "line label 3"
#define chrLINELABEL4          "line label 4"
#define chrLINELABEL5          "line label 5"
#define chrLINELABELVIS0       "line label 0 vis"
#define chrLINELABELVIS1       "line label 1 vis"
#define chrLINELABELVIS2       "line label 2 vis"
#define chrLINELABELVIS3       "line label 3 vis"
#define chrLINELABELVIS4       "line label 4 vis"
#define chrLINELABELVIS5       "line label 5 vis"
#define chrLINELABELDIST       "line label distance"
#define chrLINELABELDISTFIRST  "line label distance first"
#define chrLINELABELOFFSET     "line label offset"
#define chrLINECOLOR           "linecolor"
#define chrLINEPATTERN         "linepattern"
#define chrLINEVIS             "linevis"
#define chrLINEWIDTH           "linewidth"
#define chrMARKERVIS           "markervis"
#define chrNODELABEL0          "node label 0"
#define chrNODELABEL1          "node label 1"
#define chrNODELABEL2          "node label 2"
#define chrNODELABEL3          "node label 3"
#define chrNODELABEL4          "node label 4"
#define chrNODELABEL5          "node label 5"
#define chrNODELABEL6          "node label 6"
#define chrNODELABEL7          "node label 7"
#define chrNODELABEL8          "node label 8"
#define chrNODELABEL9          "node label 9"
#define chrNODELABEL10         "node label 10"
#define chrNODELABEL11         "node label 11"
#define chrNODELABELVIS0       "node label 0 vis"
#define chrNODELABELVIS1       "node label 1 vis"
#define chrNODELABELVIS2       "node label 2 vis"
#define chrNODELABELVIS3       "node label 3 vis"
#define chrNODELABELVIS4       "node label 4 vis"
#define chrNODELABELVIS5       "node label 5 vis"
#define chrNODELABELVIS6       "node label 6 vis"
#define chrNODELABELVIS7       "node label 7 vis"
#define chrNODELABELVIS8       "node label 8 vis"
#define chrNODELABELVIS9       "node label 9 vis"
#define chrNODELABELVIS10      "node label 10 vis"
#define chrNODELABELVIS11      "node label 11 vis"
#define chrNODECOLOR           "nodecolor"
#define chrNODELABELCOMVIS     "node label com vis"
#define chrNODELABELPREFIXVIS  "node label prefix vis"
#define chrNODESIZE            "nodesize"
#define chrNODESYMBOL          "nodesymbol"
#define chrNODEVIS             "nodevis"
#define chrOPERABLE            "operable"
#define chrPOLYLINEVIS         "polyline visibility"
#define chrPOLYLINEARROWVIS    "polylinearrowvis"
#define chrPOLYLINEFACEVIS     "polylinefacevis"
#define chrPOLYLINENODEVIS     "polylinenodevis"
#define chrQUADVIS             "quadvis"
#define chrSELECTABLE          "selectable"
#define chrSURFACEVIS          "surface visibility"
#define chrSURFACEFACEVIS      "surfacefacevis"
#define chrTEXTSTROKEWIDTH     "text stroke width"
#define chrTEXTALIGN           "textalign"
#define chrTEXTCOLOR           "textcolor"
#define chrTEXTFONT            "textfont"
#define chrTEXTSIZE            "textsize"
#define chrTEXTSPACING         "textspacing"
#define chrTEXTVIS             "textvis"
#define chrTRANSMISSION        "transmission"
#define chrTRANSON             "TransOn"

#define chrDATA                "Data"
#define chrATTRIBUTED          "Attributed"
#define chrCODEPRIORITY        "Codepriority"
#define chrDTMCODE             "Dtmcode"
#define chrMODELCODE           "Modelcode"
#define chrSURVCODE            "Survcode"
#define chrVBMCODE             "Vmbcode"

typedef struct {
   double x;
   double y;
   double z;
} XYZ;
    
typedef struct {
   float a;
   float b;
   float c;
   float d;
} PLANE;

typedef struct {
   double r;
   double g;
   double b;
} RGB;


/* Prototypes */
LIBMSCI_API const Sname *ciGetObjNameFromPath(Snametable *nt, const Sname *path);
LIBMSCI_API void         ciPlaneFromString(XYZ *origin, const Sname *sname, PLANE *plane);

msEND_EXTERN_C
#endif

