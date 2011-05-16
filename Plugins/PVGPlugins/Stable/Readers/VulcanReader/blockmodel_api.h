#if !defined(MTK_EXPORTED_BLOCKMODEL_API_H)
#define MTK_EXPORTED_BLOCKMODEL_API_H
/*////////////////////////////////////////////////////////////////////////////

  Name        : blockmodel_api.h
  Description : This header file declares api to the vulcan blockmodel data 
              : structure. 
              :
  Revision    : 3fa388289f70ccbac1ff3b06dfdf2704
              :
  Modified    : 20-Feb-2007 WCB DE2007020047U.1 - added FindXyz, FindWorldXys,
              :    AddMatchTriangulation, Get/SetPosition etc...
  Modified    : 19-Apr-2006 WCB DE2005080060P.9 - added 
              :    schema (extent, sizes, dimensions) and 
              :    model (origin, orientation)
  Modified    : 14-Mar-2006 WCB DE2005080060P.8 - moved and changed methods
  Creation    : 21-Feb-2006 WCB DE2005080061P.13 -

//////////////////////////////////////////////////////////////////////////////

Maptek Pty Ltd (c) 2005 - All rights reserved

/////////////////////////////////////////////////////////////////////////// */
#include <string>
#include <vector>

#define MTKBLOCKMODEL_API_DLL_EXPORT __declspec(dllexport)

// Maptek grid
namespace mtk 
{ 
namespace model 
{ 
namespace block 
{
   class Model;
   class Match;
} 
}
}

namespace mtk
{
namespace exported_blockmodel
{ 

class MTKBLOCKMODEL_API_DLL_EXPORT MTK_Position 
{
   public:
   MTK_Position(){}
   MTK_Position(const MTK_Position& position)
      { m_Position = position.GetBlockNumber(); }
   ~MTK_Position(){}

   int GetBlockNumber() const { return m_Position; }
   void SetBlockNumber(int number){ m_Position = number; }

   private:
   int m_Position;
};

class MTKBLOCKMODEL_API_DLL_EXPORT MTK_BlockModel 
{
   public:

      MTK_BlockModel();
      MTK_BlockModel(const std::string& name);
      ~MTK_BlockModel();

      bool IsOpen();
      bool Open(const std::string& name);
      void Close();

      // ------------------------
      // Generic type functions
      void GetModelExtent(double* x0, double* y0, double *z0, 
                          double* x1, double* y1, double* z1) const;

      void GetModelOrigin(double* x, double* y, double *z) const;
      void GetModelOrientation(double* x, double* y, double *z) const;

      std::vector<std::string> GetPredefinedVariableList() const;
      std::vector<std::string> GetVariableList() const;
      std::vector<std::string> GetNumericVariableList() const;
      std::vector<std::string> GetStringVariableList() const;

      bool IsNumber(const std::string& varName) const;
      bool IsString(const std::string& varName) const;

      int NBlocks() const;
      void FirstBlock() const;
      void Next() const;
      bool IsEof() const;

      double Get(const std::string& varName) const;
      std::string GetString(const std::string& varName) const;

      void GetBlockExtent(double* x0, double* y0, double* z0, 
                          double* x1, double* y1, double* z1) const;

      void ModelToWorld(double bx,  double by,  double bz,
                        double* wx, double* wy, double* wz);
      void WorldToModel(double wx,  double wy,  double wz,
                        double* mx, double* my, double* mz);

      bool FindXyz(double bx, double by, double bz);
      bool FindWorldXyz(double wx, double wy, double wz);

      MTK_Position GetPosition() const;
      bool SetPosition(MTK_Position position);

      void ClearMatch();
      void SetUseExactVolume(bool use);
      void AddMatchTriangulation(const std::string& triName);

      void SetMatchExtent(double x0, double y0, double z0,
                          double x1, double y1, double z1);
      double GetBlockMatchVolume() const;
      void GetBlockMatchExtent(double* x0, double* y0, double* z0, 
                          double* x1, double* y1, double* z1) const;


      // ------------------------
      // schema functions
      int NSchemas() const;
      bool GetSchemaExtent(const int nsch, 
                          double* x0, double* y0, double* z0,
                          double* x1, double* y1, double* z1) const;
      bool GetSchemaSizes(const int nsch, 
                         double* minx, double* miny, double* minz,
                         double* maxx, double* maxy, double* maxz) const;
      bool GetSchemaDimensions(const int nsch, int* ix, int* iy, int* iz) const;

      // ------------------------
      std::string GetError() const; 
      bool HasError() const;

   protected:
      void SetError(const std::string& error) const;
      mtk::model::block::Model* m_blockmodel;
      mtk::model::block::Match* m_blockmatch;
      mutable std::string m_error;

};

} // namespace exported_blockmodel
} // namespace mtk

#endif
// End of double inclusion guard
