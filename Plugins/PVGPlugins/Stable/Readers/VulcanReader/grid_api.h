#if !defined(MTK_EXPORTED_GRID_API_H)
#define MTK_EXPORTED_GRID_API_H
/*////////////////////////////////////////////////////////////////////////////

  Name        : grid_api.h
  Description : This header file declares api to the vulcan grid data 
              : structure. 
              :
  Revision    : 06b856a00768f56f43cfef8860392257
              :
  Modified    : 14-Mar-2006 WCB DE2005080060P.8 - moved and change methods
  Creation    : 21-Feb-2006 WCB DE2005080061P.13 -

//////////////////////////////////////////////////////////////////////////////

Maptek Pty Ltd (c) 2005 - All rights reserved

/////////////////////////////////////////////////////////////////////////// */
#include <string>
#include <vector>

#define MTKGRID_API_DLL_EXPORT __declspec(dllexport)

// Maptek grid
namespace mtk 
{ 
namespace model 
{ 
namespace grid 
{
   class Grid;
} 
}
}

namespace mtk
{
namespace exported_grid
{ 

class MTKGRID_API_DLL_EXPORT MTK_Grid 
{
   public:

      MTK_Grid();
      MTK_Grid(const std::string& name);
      ~MTK_Grid();

      bool IsValid();
      bool Open(const std::string& name);
      void Close();

      // ------------------------
      // Generic type functions
      void GetExtent(double* x0, double* y0, double* x1, double* y1) const;
      void GetSpacing(double* x, double* y) const;
      void GetNodes(int* nx, int* ny) const;
      void GetNodeValue(int i, int j,  double* val, bool* mask) const;

      std::string GetError() const; 
      bool HasError() const;

   protected:
      void SetError(const std::string& error) const;
      mtk::model::grid::Grid* m_grid;
      mutable std::string m_error;

};

} // namespace exported_grid
} // namespace mtk

#endif
// End of double inclusion guard
