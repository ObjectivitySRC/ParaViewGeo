#if !defined(MTK_EXPORTED_DGD_API_H)
#define MTK_EXPORTED_DGD_API_H
/*////////////////////////////////////////////////////////////////////////////

  Name        : dgd_api.h
  Description : This header file declares api to vulcan dgd data structure
              : classes representing polylines (like polygons but not
              : necessarily closed), arrows, text, and dimension objects.
              : There are separate classes for objects meant to be displayed
              : and ones that can be used outside of Envisage. The generic
              : Object class and the hierarchy of classes (ObjectPolyline,
              : ObjectArrow, ObjectText, and so on)
              :
  Revision    : 57e0e7d7dda78495c0165f989e0b78e7
              :
  Modified    : ---
  Modified    : 14-Mar-2006 WCB DE2005080060P.8 - changed location and methods
  Modified    : 23-Feb-2006 WCB DE2005080061P.13 - renamed some functions and
              :    cleaned up a bit.
  Modified    : 20-Feb-2006 WCB DE2005080061P.12 - change DLine,DRadius,etc..
              :    to DimensionLine, DimensionRadius, etc...
  Creation    : 01-Feb-2006 WCB DE2005080061P.8 -

//////////////////////////////////////////////////////////////////////////////

Maptek Pty Ltd (c) 2005 - All rights reserved

/////////////////////////////////////////////////////////////////////////// */
#include <string>
#include <vector>

//#include <mtk/exported_api/export.h>
#define MTKDGD_API_DLL_EXPORT __declspec(dllexport)

// Maptek database/design classes
namespace mtk
{
namespace design
{
   class Object;
   class Layer;
   class Database;
}
}


namespace mtk
{

namespace exported_dgd
{

enum MTK_ObjectType { DESIGN_NULL, DESIGN_POLYLINE, DESIGN_TEXT, DESIGN_3DTEXT, 
                  DESIGN_ARROW, DESIGN_DIMENSIONLINE, DESIGN_DIMENSIONRADIUS, 
                  DESIGN_DIMENSIONARC, DESIGN_DIMENSIONANGLE };

class MTKDGD_API_DLL_EXPORT MTK_Object 
{
   public:
      friend class MTK_Layer;

      MTK_Object();
      MTK_Object(const MTK_Object& object); // not defined...

      ~MTK_Object();

      // ---------------------------------------------------------------
      // General object type functions
      // ---------------------------------------------------------------
      MTK_ObjectType GetType() const;

      std::string GetName() const;
      std::string GetDescription() const;
      int GetColour() const;

      bool GetAttributes(std::string& group, std::string& feature,
                        std::string& primitive, double* value) const;

      bool IsPolyline() const;
      bool IsText() const;
      bool Is3DText() const;
      bool IsArrow() const;
      bool IsDimensionLine() const;
      bool IsDimensionRadius() const;
      bool IsDimensionArc() const;
      bool IsDimensionAngle() const;

      // We may need to look at what is really available for links.
      int NLinkRecords() const;
      int GetLinkType(int k) const;
      std::string GetLinkRecord(int k) const;

      // ---------------------------------------------------------------
      // Polyline type specific functions
      // ---------------------------------------------------------------
      void GetPolylineAttributes(int* linetype, int* pattern, 
                                 bool* clockwise, bool* closed) const;

      int NPoints() const;
      bool Get(int k, double* x, double* y, double* z, double* w, 
                        int* t, std::string* name) const;

      int GetPointType(int k) const;

      // ---------------------------------------------------------------
      // Text type specific functions
      // ---------------------------------------------------------------
      void GetTextAttributes(std::string& font, std::string& scale, 
           double* angle, double* height, double* size, bool* boxed) const;

      // ---------------------------------------------------------------
      // 3DText type specific functions
      // ---------------------------------------------------------------
      void Get3DTextAttributes(std::string& font, std::string& scale, 
           double* width, double* height, bool* italic) const;

      void GetDirection(double* x, double* y, double* z) const;
      int GetHorizontalPosition() const;
      int GetVerticalPosition() const;

      // --------------------------
      // Text 
      // 3DText
      // --------------------------
      int NTextRecords() const;
      std::string GetTextRecord(int k) const;

      // ---------------------------------------------------------------
      // Arrow type specific functions
      // ---------------------------------------------------------------
      void GetArrowAttributes(bool* autoscale, bool* filled, int* nfacets, 
           int* arrowtype) const;

      // The size and shape of the arrowhead:
      void GetArrowHeadLengthWidth(double* headLength,
           double* headWidth) const;

      // The length and width of the arrowhead can be set as a percentage
      // of the total length of the arrow:
      void GetArrowRelativeHeadLengthWidth(double* headLength,
           double* headWidth) const;

      // ---------------------------------------------------------------
      // Dimension Line type 
      // Dimension Radius type 
      // Dimension Arc type 
      // Dimension Angle type 
      // ---------------------------------------------------------------
      // Dimensions attributes...
      //   (display attributes, tick attributes, arrow attributes)
      void GetDimensionAttributes(double* textMapSize, std::string& label,
           double* tickOffset, double* tickGap, double* tickExtentSize,
           double* arrowLength, double* arrowWidth, int* textLocation) const;

      // --------------------------
      // Arrow 
      // Dimension Line 
      // Dimensino Radius 
      // --------------------------
      void GetStart(double* x, double* y, double* z) const;
      void GetEnd(double* x, double* y, double* z) const;

      // ------------------------
      // Dimension Arc 
      // Dimensino Angle 
      // ------------------------
      void GetRadius(double* x, double* y, double* z) const;
      void GetSweep(double* x, double* y, double* z) const;

      // ------------------------
      // Polyline
      // 3DText
      // Dimension Line 
      // Dimensino Radius 
      // Dimension Arc 
      // Dimensino Angle 
      // -------------------------
      int GetLineType() const;

      // -------------------------
      // Text
      // 3DText
      // Dimension Arc 
      // Dimensino Angle 
      // -------------------------
      void GetOrigin(double* x, double* y, double* z) const;

      // -------------------------
      // 3DText
      // Arrow
      // Dimension Line 
      // Dimensino Radius 
      // -------------------------
      void GetNormal(double* x, double* y, double* z) const;

      // -------------------------
      // Text
      // 3DText
      // Dimension Line 
      // Dimensino Radius 
      // Dimension Arc 
      // Dimensino Angle 
      // -------------------------
      void GetFontScaleHeight(std::string& font, std::string& scale, 
           double* textHeight) const;

      // -------------------------
      // 3DText
      // Dimension Line 
      // Dimensino Radius 
      // Dimension Arc 
      // Dimensino Angle 
      // -------------------------
      void IsMirrored(bool* horizontal, bool* vertical) const;

      bool HasError() const;
      std::vector<std::string> GetError() const;
      void ClearError() const;

   protected:
      void SetError(const std::string& error) const;
      mtk::design::Object* m_object;
      mutable std::string m_error;

};

// ==================================================
// Function by catagory....
// ==================================================
// General functions...
//    GetName(...)
//    GetDescription(...)
//    GetColour(...)
//    GetAttributes(...)
//    NLinkRecords(...)
//    GetLinkType(...)
//    GetLinkRecord(...)
//    HasError(...)
//    GetError(...)
//    ClearError(...)

//  * GetType(...)
//    ...................
//  * IsPolyline(...)
//  * IsText(...)
//  * Is3DText(...)
//  * IsArrow(...)
//  * IsDimensionLine(...)
//  * IsDimensionRadius(...)
//  * IsDimensionArc(...)
//  * IsDimensionAngle(...)

//---------------------------------
// Polyline functions
//    GetPolylineAttributes(...)
//    NPoints(...)
//    Get(...)
//    GetPointType(...)
//    GetLineType(...)
   
//---------------------------------
// Text
//    GetTextAttributes(...)
//    NTextRecords(...) 
//    GetTextRecord(...)
//    GetOrigin(...)
//    GetFontScaleHeight(...)

//---------------------------------
// 3DText
//    Get3DTextAttributes(...)
//    GetDirection(...)
//    GetHorizontalPosition(...)
//    GetVerticalPosition(...)
//    NTextRecords(...)
//    GetTextRecord(...)
//    GetLineType(...)
//    GetOrigin(...)
//    GetNormal(...)
//    GetFontScaleHeight(...)
//    IsMirrored(...)

//---------------------------------
// Arrow
//    GetArrowAttributes(...)
//    GetArrowHeadLengthWidth(...)
//    GetArrowRelativeHeadLengthWidth(...)
//    GetStart(...)
//    GetEnd(...)
//    GetNormal(...)

//---------------------------------
// Dimension Line
//    GetDimensionAttributes(...)
//    GetStart(...)
//    GetEnd(...)
//    GetLineType(...)
//    GetNormal(...)
//    GetFontScaleHeight(...)
//    IsMirrored(...)

//---------------------------------
// Dimension Radius
//    GetDimensionAttributes(...)
//    GetStart(...)
//    GetEnd(...)
//    GetLineType(...)
//    GetNormal(...)
//    GetFontScaleHeight(...)
//    IsMirrored(...)

//---------------------------------
// Dimension Arc
//    GetDimensionAttributes(...)
//    GetRadius(...)
//    GetSweep(...)
//    GetLineType(...)
//    GetOrigin(...)
//    GetFontScaleHeight(...)
//    IsMirrored(...)

//---------------------------------
// Dimension Angle
//    GetDimensionAttributes(...)
//    GetRadius(...)
//    GetSweep(...)
//    GetLineType(...)
//    GetOrigin(...)
//    GetFontScaleHeight(...)
//    IsMirrored(...)

///////////////////////////////////////////////////////////////
// A Layer can be used outside of Envisage.
class MTKDGD_API_DLL_EXPORT MTK_Layer
{
   public:
      friend class MTK_Database;

      MTK_Layer(const MTK_Layer& layer);
      MTK_Layer();
      ~MTK_Layer();

      std::string GetName() const;
      std::string GetDescription() const;

      int NObjects() const;
      bool GetObject(int k, MTK_Object* object) const;

      std::string GetDate() const;
      std::string GetTime() const;

      bool HasError() const;
      std::string GetError() const;

   private:
      void SetError(const std::string& error) const;
      MTK_Layer& operator =(mtk::design::Layer& layer);
      mtk::design::Layer* m_layer;
      mutable std::string m_error;
      
};

///////////////////////////////////////////////////////////////
// A Database can be used outside of Envisage.
class MTKDGD_API_DLL_EXPORT MTK_Database 
{
   public:

      MTK_Database();
      MTK_Database(const std::string& name);
      ~MTK_Database();
      bool Open(const std::string& name);
      void Close();

      bool IsOpen() const;

      std::string GetName() const;
      int GetVersion() const;

      int NLayers() const;
      //bool Get(int k, MTK_Layer* layer);
      bool Get(const std::string& name, MTK_Layer* layer);
      bool Exists(const std::string& name) const;

      std::vector<std::string> GetLayerList() const;
      bool FirstMatchingLayer(const std::string& pattern, MTK_Layer* layer);
      bool NextMatchingLayer(MTK_Layer* layer);

      bool HasError() const;
      std::vector<std::string> GetError() const;
      void ClearError() const;

   private:
      void SetError(const std::string& error) const;
      mtk::design::Database* m_database;
      mutable std::string m_error;

};

} // namespace exported_dgd
} // namespace mtk

#endif
// End of double inclusion guard
