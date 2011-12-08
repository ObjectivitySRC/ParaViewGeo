/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAnalyzeWriter.cxx

  Copyright (c) Joseph Hennessey
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkAnalyzeWriter.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtknifti1_io.h"

#include "vtkUnsignedCharArray.h"
#include "vtkFieldData.h"

#define NIFTI_HEADER_ARRAY "vtkNIfTIReaderHeaderArray"

vtkStandardNewMacro(vtkAnalyzeWriter);

vtkAnalyzeWriter::vtkAnalyzeWriter()
{
    savedFlipAxis = new int[3];
      savedInPlaceFilteredAxes = new int[3];

    this->FileLowerLeft = 1;
    this->FileType = 0;
    this->FileDimensionality = 3;
}

vtkAnalyzeWriter::~vtkAnalyzeWriter()
{
  delete savedFlipAxis;
  savedFlipAxis = NULL;
  delete savedInPlaceFilteredAxes;
  savedInPlaceFilteredAxes = NULL;
}


void vtkAnalyzeWriter::SetFileType(int inValue){
  FileType = inValue;
}

int vtkAnalyzeWriter::getFileType(){
  return FileType;
}


/* return number of extensions written, or -1 on error */
static int nifti_write_extensions(znzFile fp, nifti_image *nim)
{
   nifti1_extension * list;
   char               extdr[4] = { 0, 0, 0, 0 };
   int                c, ok = 1;
   size_t size;

   if( znz_isnull(fp) || !nim || nim->num_ext < 0 ){
      return -1;
   }

   // if invalid extension list, clear num_ext 
   if( ! vtknifti1_io::valid_nifti_extensions(nim) ) nim->num_ext = 0;

   // write out extender block
   if( nim->num_ext > 0 ) extdr[0] = 1;
   if( vtknifti1_io::nifti_write_buffer(fp, extdr, 4) != 4 ){
      fprintf(stderr,"** failed to write extender\n");
      return -1;
   }

   list = nim->ext_list;
   for ( c = 0; c < nim->num_ext; c++ ){
      size = vtknifti1_io::nifti_write_buffer(fp, &list->esize, sizeof(int));
      ok = (size == (int)sizeof(int));
      if( ok ){
         size = vtknifti1_io::nifti_write_buffer(fp, &list->ecode, sizeof(int));
         ok = (size == (int)sizeof(int));
      }
      if( ok ){
         size = vtknifti1_io::nifti_write_buffer(fp, list->edata, list->esize - 8);
         ok = (((int)size) == list->esize - 8);
      }

      if( !ok ){
         fprintf(stderr,"** failed while writing extension #%d\n",c);
         return -1;
      }

      list++;
   }

   return nim->num_ext;
}

//GetExtension from uiig library.
static vtkstd::string
GetExtension( const vtkstd::string& filename ) {

  // This assumes that the final '.' in a file name is the delimiter
  // for the file's extension type
  const vtkstd::string::size_type it = filename.find_last_of( "." );

  // This determines the file's type by creating a new string
  // who's value is the extension of the input filename
  // eg. "myimage.gif" has an extension of "gif"
  vtkstd::string fileExt( filename, it+1, filename.length() );

  return( fileExt );
}

//GetRootName from uiig library.
static vtkstd::string
GetRootName( const vtkstd::string& filename )
{
  const vtkstd::string fileExt = GetExtension(filename);

  // Create a base filename
  // i.e Image.hdr --> Image
  if( fileExt.length() > 0 )
    {
    const vtkstd::string::size_type it = filename.find_last_of( fileExt );
    vtkstd::string baseName( filename, 0, it-fileExt.length() );
    return( baseName );
    }
  //Default to return same as input when the extension is nothing (Analyze)
  return( filename );
}

static vtkstd::string
GetHeaderFileName( const vtkstd::string & filename )
{
  vtkstd::string ImageFileName = GetRootName(filename);
  vtkstd::string fileExt = GetExtension(filename);
  //If file was named xxx.img.gz then remove both the gz and the img endings.
  if(!fileExt.compare("gz"))
    {
    ImageFileName=GetRootName(GetRootName(filename));
    }
  ImageFileName += ".hdr";
  return( ImageFileName );
}

//Returns the base image filename.
static vtkstd::string GetImageFileName( const vtkstd::string& filename )
{
  // Why do we add ".img" here?  Look in fileutils.h
  vtkstd::string fileExt = GetExtension(filename);
  vtkstd::string ImageFileName = GetRootName(filename);
  if(!fileExt.compare("gz"))
    {
    //First strip both extensions off
    ImageFileName=GetRootName(GetRootName(filename));
    ImageFileName += ".img.gz";
    }
  else if(!fileExt.compare("img") || !fileExt.compare("hdr") )
    {
    ImageFileName += ".img";
    }
  else
    {
    //uiig::Reporter* reporter = uiig::Reporter::getReporter();
    //vtkstd::string temp="Error, Can not determine compressed file image name. ";
    //temp+=filename;
    //reporter->setMessage( temp );
    return ("");
    }
  return( ImageFileName );
}

void vtkAnalyzeWriter::WriteFileHeader(ofstream * vtkNotUsed(file), vtkImageData *cache)
{
   
   struct nifti_1_header nhdr ;
   znzFile               fp=NULL;
   size_t                ss ;
   int                   write_data, leave_open;
   znzFile        imgfile = NULL;
   const char * opts = "wb";
   int orientPosition = 252;

   write_data = 0;
   leave_open = 0;
  
  // Find the length of the rows to write.

    // Get the information from the input
  double spacing[3];
  double origin[3];
  int wholeExtent[6];
  int numComponents = cache->GetNumberOfScalarComponents();
  imageDataType = cache->GetScalarType();
  cache->GetSpacing(spacing);
  cache->GetOrigin(origin);
  cache->GetWholeExtent(wholeExtent);
  
  if( numComponents > 1 ){
    vtkErrorMacro("cannot write data with more than 1 component yet.");
  }

  char *iname = this->GetFileName();
  vtkstd::string HeaderFileName = GetHeaderFileName( iname );

   if( ! vtknifti1_io::nifti_validfilename(HeaderFileName.c_str())  ) vtkErrorMacro("bad fname input") ;

  vtkImageData * data = cache;
  vtkFieldData *fa = data->GetFieldData();
  int headerSize = 348;

  if (!fa)
  {
    fa = vtkFieldData::New();
    data->SetFieldData(fa);
    fa->Delete();
    fa = data->GetFieldData();
  }

  vtkUnsignedCharArray *headerUnsignedCharArray = NULL;
  vtkDataArray * validAnalyzeDataArray = fa->GetArray(ANALYZE_HEADER_ARRAY);
  foundAnalayzeHeader = true;
  foundNiftiHeader = false;
  if (!validAnalyzeDataArray)
  {
  headerUnsignedCharArray = vtkUnsignedCharArray::New();
  headerUnsignedCharArray->SetName(ANALYZE_HEADER_ARRAY);
    headerUnsignedCharArray->SetNumberOfValues(headerSize);
  fa->AddArray(headerUnsignedCharArray);
  headerUnsignedCharArray->Delete();
  foundAnalayzeHeader = false;
    validAnalyzeDataArray = fa->GetArray(ANALYZE_HEADER_ARRAY);
  }
  
  vtkDataArray * validNiftiDataArray = fa->GetArray(NIFTI_HEADER_ARRAY);
  if (validNiftiDataArray){
  foundNiftiHeader = true;
  }

  if(foundAnalayzeHeader){
    headerUnsignedCharArray = vtkUnsignedCharArray::SafeDownCast(validAnalyzeDataArray);
  } else if (foundNiftiHeader){
    headerUnsignedCharArray = vtkUnsignedCharArray::SafeDownCast(validNiftiDataArray);
  }

  nifti_1_header tempNiftiHeader;
  unsigned char * headerUnsignedCharArrayPtr = (unsigned char *) &tempNiftiHeader;
  int count;

  //this->headerUnsignedCharArrayPtr = new unsigned char[this->headerSize];

  nifti_image * m_NiftiImage = NULL;

  if(foundAnalayzeHeader){
    for(count=0;count<headerSize;count++){
     headerUnsignedCharArrayPtr[count] = headerUnsignedCharArray->GetValue(count);
    }
      m_NiftiImage = vtknifti1_io::nifti_convert_nhdr2nim(tempNiftiHeader, HeaderFileName.c_str());
  } else if(foundNiftiHeader){
    for(count=0;count<headerSize;count++){
     headerUnsignedCharArrayPtr[count] = headerUnsignedCharArray->GetValue(count);
    }
      m_NiftiImage = vtknifti1_io::nifti_convert_nhdr2nim(tempNiftiHeader, HeaderFileName.c_str());

      int qform_code = m_NiftiImage->qform_code;
      int sform_code = m_NiftiImage->sform_code;

    int row,col;
      double **q;
      double **s;

    q = new double*[4];
    s = new double*[4];

    for(count=0;count<4;count++){
    q[count] = new double[4];
    s[count] = new double[4];
    }

    for (row=0;row<4;row++){
      for (col=0;col<4;col++){
      s[row][col] = m_NiftiImage->sto_xyz.m[row][col];
      q[row][col] = m_NiftiImage->qto_xyz.m[row][col];
      }
    }

    int flipAxis[3];
    int InPlaceFilteredAxes[3];

    flipAxis[0] = 0;
    flipAxis[1] = 0;
    flipAxis[2] = 0;

    InPlaceFilteredAxes[0]=0;
    InPlaceFilteredAxes[1]=1;
    InPlaceFilteredAxes[2]=2;

    if(sform_code>0){
      if(s[0][0]>=1.0){
      InPlaceFilteredAxes[0]=0;
      flipAxis[0] = 0;
      } else if (s[0][0]<=-1.0){
      InPlaceFilteredAxes[0]=0;
      flipAxis[0] = 1;
      }
      if(s[0][1]>=1.0){
      InPlaceFilteredAxes[0]=1;
      flipAxis[0] = 0;
      } else if (s[0][1]<=-1.0){
      InPlaceFilteredAxes[0]=1;
      flipAxis[0] = 1;
      }
      if(s[0][2]>=1.0){
      InPlaceFilteredAxes[0]=2;
      flipAxis[0] = 0;
      } else if (s[0][2]<=-1.0){
      InPlaceFilteredAxes[0]=2;
      flipAxis[0] = 1;
      }
      if(s[1][0]>=1.0){
      InPlaceFilteredAxes[1]=0;
      flipAxis[1] = 0;
      } else if (s[1][0]<=-1.0){
      InPlaceFilteredAxes[1]=0;
      flipAxis[1] = 1;
      }
      if(s[1][1]>=1.0){
      InPlaceFilteredAxes[1]=1;
      flipAxis[1] = 0;
      } else if (s[1][1]<=-1.0){
      InPlaceFilteredAxes[1]=1;
      flipAxis[1] = 1;
      }
      if(s[1][2]>=1.0){
      InPlaceFilteredAxes[1]=2;
      flipAxis[1] = 0;
      } else if (s[1][2]<=-1.0){
      InPlaceFilteredAxes[1]=2;
      flipAxis[1] = 1;
      }
      if(s[2][0]>=1.0){
      InPlaceFilteredAxes[2]=0;
      flipAxis[2] = 0;
      } else if (s[2][0]<=-1.0){
      InPlaceFilteredAxes[2]=0;
      flipAxis[2] = 1;
      }
      if(s[2][1]>=1.0){
      InPlaceFilteredAxes[2]=1;
      flipAxis[2] = 0;
      } else if (s[2][1]<=-1.0){
      InPlaceFilteredAxes[2]=1;
      flipAxis[2] = 1;
      }
      if(s[2][2]>=1.0){
      InPlaceFilteredAxes[2]=2;
      flipAxis[2] = 0;
      } else if (s[2][2]<=-1.0){
      InPlaceFilteredAxes[2]=2;
      flipAxis[2] = 1;
      }
    } else if(qform_code>0){
      if(q[0][0]>=1.0){
      InPlaceFilteredAxes[0]=0;
      flipAxis[0] = 0;
      } else if (q[0][0]<=-1.0){
      InPlaceFilteredAxes[0]=0;
      flipAxis[0] = 1;
      }
      if(q[0][1]>=1.0){
      InPlaceFilteredAxes[0]=1;
      flipAxis[0] = 0;
      } else if (q[0][1]<=-1.0){
      InPlaceFilteredAxes[0]=1;
      flipAxis[0] = 1;
      }
      if(q[0][2]>=1.0){
      InPlaceFilteredAxes[0]=2;
      flipAxis[0] = 0;
      } else if (q[0][2]<=-1.0){
      InPlaceFilteredAxes[0]=2;
      flipAxis[0] = 1;
      }
      if(q[1][0]>=1.0){
      InPlaceFilteredAxes[1]=0;
      flipAxis[1] = 0;
      } else if (q[1][0]<=-1.0){
      InPlaceFilteredAxes[1]=0;
      flipAxis[1] = 1;
      }
      if(q[1][1]>=1.0){
      InPlaceFilteredAxes[1]=1;
      flipAxis[1] = 0;
      } else if (q[1][1]<=-1.0){
      InPlaceFilteredAxes[1]=1;
      flipAxis[1] = 1;
      }
      if(q[1][2]>=1.0){
      InPlaceFilteredAxes[1]=2;
      flipAxis[1] = 0;
      } else if (q[1][2]<=-1.0){
      InPlaceFilteredAxes[1]=2;
      flipAxis[1] = 1;
      }
      if(q[2][0]>=1.0){
      InPlaceFilteredAxes[2]=0;
      flipAxis[2] = 0;
      } else if (q[2][0]<=-1.0){
      InPlaceFilteredAxes[2]=0;
      flipAxis[2] = 1;
      }
      if(q[2][1]>=1.0){
      InPlaceFilteredAxes[2]=1;
      flipAxis[2] = 0;
      } else if (q[2][1]<=-1.0){
      InPlaceFilteredAxes[2]=1;
      flipAxis[2] = 1;
      }
      if(q[2][2]>=1.0){
      InPlaceFilteredAxes[2]=2;
      flipAxis[2] = 0;
      } else if (q[2][2]<=-1.0){
      InPlaceFilteredAxes[2]=2;
      flipAxis[2] = 1;
      }
    }

 
    savedFlipAxis[0] = flipAxis[0];
    savedFlipAxis[1] = flipAxis[1];
    savedFlipAxis[2] = flipAxis[2];

    savedInPlaceFilteredAxes[0] = InPlaceFilteredAxes[0];
    savedInPlaceFilteredAxes[1] = InPlaceFilteredAxes[1];
    savedInPlaceFilteredAxes[2] = InPlaceFilteredAxes[2];

    for(count=0;count<4;count++){
    delete [] q[count];
    q[count] = NULL;

    delete [] s[count];
    s[count] = NULL;
    }
    delete [] q;
    delete [] s;
    q = NULL;
    s = NULL;

    for(count=0;count<orientPosition;count++){
     headerUnsignedCharArrayPtr[count] = headerUnsignedCharArray->GetValue(count);
    }
    for(count=orientPosition;count<headerSize;count++){
     headerUnsignedCharArrayPtr[count] = 0;
    }
      m_NiftiImage = vtknifti1_io::nifti_convert_nhdr2nim(tempNiftiHeader, HeaderFileName.c_str());

  } else {
    for(count=0;count<headerSize;count++){
     headerUnsignedCharArrayPtr[count] = 0;
    }
    m_NiftiImage = vtknifti1_io::nifti_simple_init_nim();
  }

   m_NiftiImage->nifti_type = 0;

   //m_NiftiImage->fname = HeaderFileName.c_str();

  int check = 0;
  int comp = 0;

  m_NiftiImage->fname = vtknifti1_io::nifti_makehdrname(HeaderFileName.c_str(),  m_NiftiImage->nifti_type, check, comp);
  m_NiftiImage->iname = vtknifti1_io::nifti_makeimgname(HeaderFileName.c_str(),  m_NiftiImage->nifti_type, check, comp);

  vtknifti1_io::nifti_set_iname_offset(m_NiftiImage);

  m_NiftiImage->qfac = 1.0 ;

  m_NiftiImage->ndim = 4;
  m_NiftiImage->dim[1] = wholeExtent[1] + 1;
  m_NiftiImage->dim[2] = wholeExtent[3] + 1;
  m_NiftiImage->dim[3] = wholeExtent[5] + 1;
  m_NiftiImage->dim[4] = 1;
  m_NiftiImage->dim[5] = 0;
  m_NiftiImage->dim[6] = 0;
  m_NiftiImage->dim[7] = 0;
  m_NiftiImage->nx =  m_NiftiImage->dim[1];
  m_NiftiImage->ny =  m_NiftiImage->dim[2];
  m_NiftiImage->nz =  m_NiftiImage->dim[3];
  m_NiftiImage->nt =  m_NiftiImage->dim[4];
  m_NiftiImage->nu =  m_NiftiImage->dim[5];
  m_NiftiImage->nv =  m_NiftiImage->dim[6];
  m_NiftiImage->nw =  m_NiftiImage->dim[7];

  nhdr.pixdim[0] = 0.0 ;
  m_NiftiImage->pixdim[1] = spacing[0];
  m_NiftiImage->pixdim[2] = spacing[1];
  m_NiftiImage->pixdim[3] = spacing[2];
  m_NiftiImage->pixdim[4] = 0;
  m_NiftiImage->pixdim[5] = 0;
  m_NiftiImage->pixdim[6] = 0;
  m_NiftiImage->pixdim[7] = 0;
  m_NiftiImage->dx = m_NiftiImage->pixdim[1];
  m_NiftiImage->dy = m_NiftiImage->pixdim[2];
  m_NiftiImage->dz = m_NiftiImage->pixdim[3];
  m_NiftiImage->dt = m_NiftiImage->pixdim[4];
  m_NiftiImage->du = m_NiftiImage->pixdim[5];
  m_NiftiImage->dv = m_NiftiImage->pixdim[6];
  m_NiftiImage->dw = m_NiftiImage->pixdim[7];

 int numberOfVoxels = m_NiftiImage->nx;
 
 if(m_NiftiImage->ny>0){
  numberOfVoxels*=m_NiftiImage->ny;
 }
 if(m_NiftiImage->nz>0){
  numberOfVoxels*=m_NiftiImage->nz;
 }
 if(m_NiftiImage->nt>0){
  numberOfVoxels*=m_NiftiImage->nt;
 }
 if(m_NiftiImage->nu>0){
  numberOfVoxels*=m_NiftiImage->nu;
 }
 if(m_NiftiImage->nv>0){
  numberOfVoxels*=m_NiftiImage->nv;
 }
 if(m_NiftiImage->nw>0){
  numberOfVoxels*=m_NiftiImage->nw;
 }

  dataTypeSize = 1.0;

  m_NiftiImage->nvox = numberOfVoxels;

      switch(imageDataType)
    {
    case VTK_BIT://DT_BINARY:
    m_NiftiImage->datatype = DT_BINARY;
    m_NiftiImage->nbyper = 0;
    dataTypeSize = 0.125;
      break;
    case VTK_CHAR://Type not supported
    vtkErrorMacro("Analyze does not support the type signed char") ;
    m_NiftiImage->datatype = DT_UNSIGNED_CHAR;
    m_NiftiImage->nbyper = 1;
    dataTypeSize = m_NiftiImage->nbyper;
      break;
    case VTK_UNSIGNED_CHAR://DT_UNSIGNED_CHAR:
    m_NiftiImage->datatype = DT_UNSIGNED_CHAR;
    m_NiftiImage->nbyper = 1;
    dataTypeSize = m_NiftiImage->nbyper;
      break;
    case VTK_SHORT://DT_SIGNED_SHORT:
    m_NiftiImage->datatype = DT_SIGNED_SHORT;
    m_NiftiImage->nbyper = 2;
    dataTypeSize = m_NiftiImage->nbyper;
      break;
    case VTK_INT://DT_SIGNED_INT:
    m_NiftiImage->datatype = DT_SIGNED_INT;
    m_NiftiImage->nbyper = 4;
    dataTypeSize = m_NiftiImage->nbyper;
      break;
    case VTK_FLOAT://DT_FLOAT:
    m_NiftiImage->datatype = DT_FLOAT;
    m_NiftiImage->nbyper = 4;
     dataTypeSize = m_NiftiImage->nbyper;
     break;
    case VTK_DOUBLE://DT_DOUBLE:
    m_NiftiImage->datatype = DT_DOUBLE;
    m_NiftiImage->nbyper = 8;
    dataTypeSize = m_NiftiImage->nbyper;
      break;
     default:
      break;
    }
  
  imageSizeInBytes = (int) (numberOfVoxels * dataTypeSize);


   nhdr.datatype = m_NiftiImage->datatype ;
   nhdr.bitpix   = 8 * m_NiftiImage->nbyper ;

   if( m_NiftiImage->cal_max > m_NiftiImage->cal_min ){
     nhdr.cal_max = m_NiftiImage->cal_max ;
     nhdr.cal_min = m_NiftiImage->cal_min ;
   }

   if( m_NiftiImage->scl_slope != 0.0 ){
     nhdr.scl_slope = m_NiftiImage->scl_slope ;
     nhdr.scl_inter = m_NiftiImage->scl_inter ;
   }

   if( m_NiftiImage->descrip[0] != '\0' ){
     memcpy(nhdr.descrip ,m_NiftiImage->descrip ,79) ; nhdr.descrip[79] = '\0' ;
   }
   if( m_NiftiImage->aux_file[0] != '\0' ){
     memcpy(nhdr.aux_file ,m_NiftiImage->aux_file ,23) ; nhdr.aux_file[23] = '\0' ;
   }

   nhdr = vtknifti1_io::nifti_convert_nim2nhdr(m_NiftiImage);    // create the nifti1_header struct 

   // if writing to 2 files, make sure iname is set and different from fname 
   if( m_NiftiImage->nifti_type != NIFTI_FTYPE_NIFTI1_1 ){
       if( m_NiftiImage->iname && strcmp(m_NiftiImage->iname,m_NiftiImage->fname) == 0 ){
         free(m_NiftiImage->iname) ; m_NiftiImage->iname = NULL ;
       }
       if( m_NiftiImage->iname == NULL ){ // then make a new one 
         m_NiftiImage->iname = vtknifti1_io::nifti_makeimgname(m_NiftiImage->fname,m_NiftiImage->nifti_type,0,0);
         if( m_NiftiImage->iname == NULL ) return;  
       }
   }

   // if we have an imgfile and will write the header there, use it 
   if( ! znz_isnull(imgfile) && m_NiftiImage->nifti_type == NIFTI_FTYPE_NIFTI1_1 ){
      fp = imgfile;
   }
   else {
      fp = vtkznzlib::znzopen( m_NiftiImage->fname , opts , vtknifti1_io::nifti_is_gzfile(m_NiftiImage->fname) ) ;
    if( znz_isnull(fp) ){
         vtkErrorMacro("cannot open output file");
         return;
      }
   }

   nhdr.extents = 16384;
   unsigned char * niftiHeaderPtr = (unsigned char *) &nhdr;
   niftiHeaderPtr[orientPosition] = headerUnsignedCharArray->GetValue(orientPosition);
   orientation = niftiHeaderPtr[orientPosition];
   // write the header and extensions 

   ss = vtkznzlib::znzwrite(&nhdr , 1 , sizeof(nhdr) , fp); // write header 
   if( ss < sizeof(nhdr) ){
      vtkErrorMacro("bad header write to output file");
    vtkznzlib::znzclose(fp); return;
   }

   // partial file exists, and errors have been printed, so ignore return 
   if( m_NiftiImage->nifti_type != NIFTI_FTYPE_ANALYZE )
      (void)nifti_write_extensions(fp,m_NiftiImage);

   // if the header is all we want, we are done
   if( ! write_data && ! leave_open ){
      vtkznzlib::znzclose(fp); return;
   }

   if( m_NiftiImage->nifti_type != NIFTI_FTYPE_NIFTI1_1 ){ // get a new file pointer
      vtkznzlib::znzclose(fp);         // first, close header file 
      if( ! znz_isnull(imgfile) ){
         fp = imgfile;
      }
      else {
         fp = vtkznzlib::znzopen( m_NiftiImage->iname , opts , vtknifti1_io::nifti_is_gzfile(m_NiftiImage->iname) ) ;
         if( znz_isnull(fp) ) vtkErrorMacro("cannot open image file") ;
      }
   }

   vtkznzlib::znzseek(fp, m_NiftiImage->iname_offset, SEEK_SET);  // in any case, seek to offset 

   if( write_data ) {
     //nifti_write_all_data(fp,m_NiftiImage,NBL);
   }
   if( ! leave_open ) vtkznzlib::znzclose(fp);
   
      return;

}


void vtkAnalyzeWriter::WriteFile(ofstream * vtkNotUsed(file), vtkImageData *data,
                             int extent[6])
{
   //struct nifti_1_header nhdr ;
   znzFile               fp=NULL;
   //size_t                ss ;
   size_t                numberOfBytes ;
   int                   write_data, leave_open;
   znzFile        imgfile = NULL;
   const char * opts = "wb";
   int   iname_offset = 0;
   char *  p = (char *)data->GetScalarPointer();


// reorient data
  unsigned char * outUnsignedCharPtr = (unsigned char *) p;
  int scalarSize = (int) dataTypeSize;
  int inIndex[3];
  int inDim[3];
  int outDim[3];
  int flipAxis[3];
  int flipIndex[3];
  int InPlaceFilteredAxes[3];
  int count;
  long inOffset;
  long charInOffset;

  flipAxis[0] = 1;
  flipAxis[1] = 1;
  flipAxis[2] = 1;

  InPlaceFilteredAxes[0]=0;
  InPlaceFilteredAxes[1]=1;
  InPlaceFilteredAxes[2]=2;

  switch(orientation){
  case 0: {
      flipAxis[0] = 1;
      flipAxis[1] = 1;
      flipAxis[2] = 1;
      }
      break;
  case 1: {
      }
      break;
  case 2: {
      }
      break;
  case 3: {
      }
      break;
  case 4: {
      }
      break;
  case 5: {
      }
      break;
  case 6: {
      }
      break;
  case 255: {
      }
      break;
  default: {
      }
      break;
  } 

  if(foundNiftiHeader){
    if ((savedFlipAxis)&&(savedInPlaceFilteredAxes)){

    flipAxis[0] = savedFlipAxis[0];
    flipAxis[1] = savedFlipAxis[1];
    flipAxis[2] = savedFlipAxis[2];

    InPlaceFilteredAxes[0]=savedInPlaceFilteredAxes[0];
    InPlaceFilteredAxes[1]=savedInPlaceFilteredAxes[1];
    InPlaceFilteredAxes[2]=savedInPlaceFilteredAxes[2];
    }
  }

  for (count=0;count<3;count++){
  inDim[count] = (extent[(count*2)+1] - extent[count*2]) + 1;
 }

  for (count=0;count<3;count++){
    outDim[count]          = inDim[InPlaceFilteredAxes[count]];
 }

  unsigned char* tempUnsignedCharData = NULL;

  tempUnsignedCharData = new unsigned char[outDim[0]*outDim[1]*outDim[2]*scalarSize];

  double tempSizeDouble;
  int tempSizeInt = 0;
  if(imageDataType==1){
  tempSizeDouble = outDim[0]*outDim[1]*outDim[2]*dataTypeSize;
  tempSizeInt = (int) tempSizeDouble;
  if (tempSizeInt!=tempSizeDouble){
    tempSizeInt++;
  }
  tempUnsignedCharData = new unsigned char[tempSizeInt];
  }
  
  int idSize;
  int idZ, idY, idX;
  long outSliceSize = outDim[0]*outDim[1]*scalarSize;
  long outRowSize   = outDim[0]*scalarSize;
  long outRowOffset;
  long outSliceOffset;
  long outOffset;
  long charOutOffset;

  
 //first flip
 
  unsigned char zeroValue = 0;

 if(imageDataType==1){
  for (count = 0; count < tempSizeInt ; count++){ 
    tempUnsignedCharData[count] = zeroValue;
    //tempUnsignedCharData[count] = outUnsignedCharPtr[count];
  } 
 }


 int outBitCount;
 int outByteCount;
 int inOffsetByte;
 int inOffsetBit;
 unsigned char inBitValue;
 unsigned char inByteValue;
 unsigned char shiftedBitValue;

 // Loop through input voxels
  count = 0;
  for ( inIndex[2] = 0 ; inIndex[2] < outDim[2] ; inIndex[2]++){  
  if(flipAxis[2]==1){
    flipIndex[2] = ((outDim[2] -1) - inIndex[2]);
  } else {
    flipIndex[2] = inIndex[2];
  }
    for ( inIndex[1] = 0; inIndex[1] < outDim[1] ; inIndex[1]++){
    if(flipAxis[1]==1){
    flipIndex[1] = ((outDim[1] -1) - inIndex[1]);
    } else {
    flipIndex[1] = inIndex[1];
    }
      for (inIndex[0] = 0; inIndex[0] < outDim[0] ; inIndex[0]++){
    if(flipAxis[0]==1){
      flipIndex[0] = ((outDim[0] -1) - inIndex[0]);
    } else {
      flipIndex[0] = inIndex[0];
    }
    if(imageDataType!=1){
      inOffset = (flipIndex[2] * outSliceSize) + (flipIndex[1] * outRowSize) + (flipIndex[0] * scalarSize);
      for (idSize = 0; idSize < scalarSize ; idSize++){ 
        charInOffset = inOffset + idSize;
        tempUnsignedCharData[count++] = outUnsignedCharPtr[charInOffset]; 
      } 
    } else {
      inOffset = (flipIndex[2] * outDim[0]*outDim[1]) + (flipIndex[1] *  outDim[0]) + flipIndex[0];

      inOffsetByte = inOffset / 8;
      inOffsetBit = inOffset % 8;

      inByteValue = outUnsignedCharPtr[inOffsetByte];
      inBitValue = (inByteValue >> (inOffsetBit)) & 0x01;

      outBitCount = count % 8;
      outByteCount = count / 8;
      shiftedBitValue = inBitValue << (outBitCount);
      tempUnsignedCharData[outByteCount] += shiftedBitValue;
      count++;
    }
      } 
    }
  }

  // Loop through output voxels
 if(imageDataType!=1){
  count = 0;
  for (idZ = 0 ; idZ < outDim[2] ; idZ++){
    outSliceOffset = idZ * outSliceSize;
    for (idY = 0; idY < outDim[1]; idY++){
      outRowOffset = idY * outRowSize;
      for (idX = 0; idX < outDim[0]  ; idX++){
        outOffset = outSliceOffset + outRowOffset + (idX * scalarSize);
        for (idSize = 0; idSize < scalarSize ; idSize++){ 
        charOutOffset = outOffset + idSize;
      outUnsignedCharPtr[charOutOffset] = tempUnsignedCharData[count++];
        } 
      } 
    }
  }
 } else if(imageDataType==1){
  for (count = 0; count < tempSizeInt ; count++){ 
    //outUnsignedCharPtr[count] = zeroValue;
    outUnsignedCharPtr[count] = tempUnsignedCharData[count];
  } 
 }

  if (imageDataType==VTK_BIT){
  unsigned char tempByte;
  unsigned char tempBit;
  unsigned char newByte;
  int bitCount;
  int byteSize = 8;

  for (count = 0; count < ((int) imageSizeInBytes) ; count++){ 
    tempByte = outUnsignedCharPtr[count];

    //swap tempByte
    newByte = 0;
    for (bitCount = 0; bitCount < byteSize ; bitCount++){
    tempBit = (tempByte >> bitCount) & 0x01; 
    newByte += tempBit << (7 - bitCount);
    }

    outUnsignedCharPtr[count] = newByte;
  } 
  }

  // then permute 

  delete tempUnsignedCharData;
  tempUnsignedCharData = NULL;

  double tempSize = outDim[0] * outDim[1];
  double tempSliceSize = tempSize * dataTypeSize;
  int tempSliceSizeInt = (int) tempSliceSize;
  if (tempSliceSizeInt<tempSliceSize){
    tempSliceSizeInt++;
  }
  int onDiskImageSizeInBytes = tempSliceSizeInt * outDim[2];

  tempUnsignedCharData = new unsigned char[onDiskImageSizeInBytes];

  if (imageDataType!=VTK_BIT){
    for (count = 0 ; count < onDiskImageSizeInBytes ; count++){
    tempUnsignedCharData[count] = outUnsignedCharPtr[count];
    }
  } else {
    // Loop through input voxels
    int totalBitCount = 0;
    int byteBitCount = 0;
    int byteCount = 0;
    int outSliceBit;
    int outSliceByte;
    int outOffsetByte;
    unsigned char tempByteValue = 0;
    unsigned char tempBitValue = 0;
    int outBitNumber = 0;

    for ( idZ = 0 ; idZ < outDim[2] ; idZ++){
    for ( idY = 0; idY < outDim[1] ; idY++){
      for (idX = 0; idX < outDim[0] ; idX++){
      outSliceOffset = tempSliceSizeInt * idZ;
      outSliceBit =  (idY * outDim[0]) + idX;
      outSliceByte = (int) (outSliceBit / 8);
      outOffsetByte = outSliceOffset + outSliceByte;
      outBitNumber = outSliceBit %8;
        byteBitCount = totalBitCount % 8;
        byteCount = (int) (totalBitCount / 8);

      tempByteValue = outUnsignedCharPtr[byteCount];
      tempBitValue = (tempByteValue >> byteBitCount) & 0x01;

      if(outBitNumber==0){
        tempUnsignedCharData[outOffsetByte] = 0;
      }
      tempUnsignedCharData[outOffsetByte]  += (tempBitValue << outBitNumber);

        totalBitCount++;
      } 
    }
    }
  }
  char * outP = (char *) (tempUnsignedCharData);

   write_data = 1;
   leave_open = 0;

  char *iname = this->GetFileName();
  vtkstd::string ImageFileName = GetImageFileName( iname );

  if( ! znz_isnull(imgfile) ){
     fp = imgfile;
  }
  else {
     fp = vtkznzlib::znzopen( ImageFileName.c_str() , opts , vtknifti1_io::nifti_is_gzfile(ImageFileName.c_str()) ) ;
     if( znz_isnull(fp) ) vtkErrorMacro("cannot open image file") ;
  }
   numberOfBytes = this->getImageSizeInBytes();

   vtkznzlib::znzseek(fp, iname_offset, SEEK_SET);  // in any case, seek to offset 
   if( write_data ) {
     vtknifti1_io::nifti_write_buffer(fp, outP, numberOfBytes);
   }
   if( ! leave_open ) vtkznzlib::znzclose(fp);

  delete tempUnsignedCharData;
  tempUnsignedCharData = NULL;

}

//----------------------------------------------------------------------------
void vtkAnalyzeWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
