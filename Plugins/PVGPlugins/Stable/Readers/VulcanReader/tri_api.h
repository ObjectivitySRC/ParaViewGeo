#if !defined(MTK_EXPORTED_TRI_API_H)
#define MTK_EXPORTED_TRI_API_H
/*////////////////////////////////////////////////////////////////////////////

  Name        : tri_api.h
  Description : This header file declares api to the vulcan triangulation 
              : structure. 
              :
  Revision    : b5eb951cf8ed4a1b1cae9a1e4aee96e0
              :
  Modified    : 14-Mar-2006 WCB DE2005080060P.8 - moved and changed methods
  Creation    : 21-Feb-2006 WCB DE2005080061P.13 -

//////////////////////////////////////////////////////////////////////////////

Maptek Pty Ltd (c) 2005 - All rights reserved

/////////////////////////////////////////////////////////////////////////// */
#include <string>
#include <vector>

#define MTKTRI_API_DLL_EXPORT __declspec(dllexport)

// Maptek triangulation
namespace mtk 
{ 
namespace model 
{ 
namespace tri 
{
   class Triangulation;
} 
}
}

namespace mtk
{
namespace exported_tri
{ 

class MTKTRI_API_DLL_EXPORT MTK_Triangulation 
{
   public:

      MTK_Triangulation();
      MTK_Triangulation(const std::string& name);
      ~MTK_Triangulation();

      bool IsValid();
      bool Open(const std::string& name);
      void Close();

      // ------------------------
      // Generic type functions
      void GetColour(int* r, int* g, int* b);
      int NumPoints() const;
      int NumTriangles() const;
      void GetPoint(int i, double* x, double* y, double* z) const;
      void GetTriangle(int i, int* p0, int* p1, int* p2) const;

      std::string GetError() const; 
      bool HasError() const;

   protected:
      void SetError(const std::string& error) const;
      mtk::model::tri::Triangulation* m_triangulation;
      mutable std::string m_error;

};

} // namespace exported_tri
} // namespace mtk

#endif
// End of double inclusion guard
