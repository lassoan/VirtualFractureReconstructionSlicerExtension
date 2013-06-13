#ifndef __ibiaImageDifinitions_h
#define __ibiaImageDifinitions_h

#include <itkImage.h>
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

///Typedef for input pixel type
typedef float InputPixelType;
///Typedef for output pixel type
typedef float OutputPixelType;
///Typedef for overlay pixel type
typedef unsigned char	OverlayPixelType;
///Tpededef for pixel type used for images generated for principal component analysis
typedef float CalcPixelType;
///Tpededef for pixel type used for PCA calculations (usually double)
typedef double PCAPixelType;
///Tpededef for pixel type used for Level set calculations (usually double)
typedef float LevelSetPixelType;
///Defining number of Dimensions
enum{Dimension = 3};


typedef itk::Image< InputPixelType,  Dimension >   ImageType;
typedef ImageType::Pointer              ImagePointer;
///Typedef for input image typ
typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
///Typedef for input image type 2D
typedef itk::Image< InputPixelType,  2 >   InputImageType2D;
///Typedef for input image type 4D
typedef itk::Image< InputPixelType,  4 >   InputImageType4D;
///Typedef for short image type 4D
typedef itk::Image< unsigned short,  4 >   InputImageType4DShort;
///Typedef for output image type
typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;
///Typedef for calculation image type
typedef itk::Image< OverlayPixelType, Dimension >  OverlayImageType;
///Typedef for image type used for PCA
typedef itk::Image< PCAPixelType,  Dimension >   PCAImageType;
typedef PCAImageType::Pointer PCAImagePointer;
typedef std::vector<PCAImagePointer>	           ImageContainerDouble;
///Typedef for calc image type, for filters that require float or double images
typedef itk::Image< CalcPixelType, Dimension >   CalcImageType;
///Typedefs for vector image type, used for deformation fields
typedef itk::CovariantVector< LevelSetPixelType, Dimension >    CovariantVectorPixelType;
///Typedefs for gradient upwind (fast marching)
typedef itk::Image<  CovariantVectorPixelType, Dimension > CovariantVectorImageType;
///Typedefs for vector image type, used for deformation fields
typedef itk::Vector< InputPixelType, Dimension >    VectorPixelType;
typedef itk::Image<  VectorPixelType, Dimension > DeformationFieldType;
typedef std::vector<ImagePointer>	           ImageContainer;
///Typedefs for vector image type, used for deformation fields
typedef itk::Vector< InputPixelType, 2 >    VectorPixelType2D;
typedef itk::Image<  VectorPixelType, 2 > DeformationFieldType2D;
///Typedef for SmartPointer to input image
typedef InputImageType::Pointer IPointer;
///Typedef for SmartPointer to input image
typedef InputImageType2D::Pointer IPointer2D;
///Typedef for SmartPointer to calculation image (currently float, but can be changed to double)
typedef CalcImageType::Pointer CIPointer;
///Typedef for image container
typedef std::vector<IPointer> InputImageContainer;
///Typedef for 2D image container
typedef std::vector<InputImageType2D::Pointer> InputImageContainer2D;
///Typedef for image container
typedef std::vector<OverlayImageType::Pointer> OverlayImageContainer;
///Typedef for image container
typedef std::vector<PCAImageType::Pointer> PCAImageContainer;
///Typedef for image container
typedef std::vector<DeformationFieldType::Pointer> DeformationFieldContainer;


typedef InputImageType::SizeType ISizeType;
typedef InputImageType::SpacingType ISpacingType;
typedef InputImageType::IndexType IIndexType;

   ///Typedef for output image type ConstInputIteratorType
  typedef itk::ImageRegionConstIterator< InputImageType > ConstInputIteratorType;

   ///Typedef for ConstOverlayIteratorType
  typedef itk::ImageRegionConstIterator< OverlayImageType > ConstOverlayIteratorType;
   ///Typedef for ConstOverlayIteratorType
  typedef itk::ImageRegionIterator< OverlayImageType > OverlayIteratorType;

   ///Typedef for OutputIteratorType
  typedef itk::ImageRegionIterator< OutputImageType> OutputIteratorType;

#endif

