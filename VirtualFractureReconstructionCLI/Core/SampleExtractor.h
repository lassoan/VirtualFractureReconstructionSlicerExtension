/*
 * SampleExtractor.h
 *
 *  Created on: 26.08.2011
 *      Author: kfritscher
 */

#ifndef SampleExtractor_H_
#define SampleExtractor_H_

#include "itkLightObject.h"
#include "itkImage.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkVectorContainer.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "SheetnessImageCreator.h"

#include "AlglibArrayWriter.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkSmartPointer.h"
#include "vtkComputeLocalGeometry.h"
#include "itkExtractImageFilter.h"

#include "ibiaScalarTextureCalculator.h"
#include "itkFiniteCylinderSpatialFunction.h"





namespace ibia
{

  /** \class SampleExtractor
  *
  * \brief
  *
  * \author K. Fritscher
  **/
template<class InputImageType, class TemplateImage=itk::Image<unsigned char,3 > >
  class SampleExtractor : public itk::LightObject
  {
   public:
    typedef ibia::SampleExtractor<InputImageType> Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    typedef itk::WeakPointer<const Self>  ConstWeakPointer;

    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename InputImageType::PointType InputImagePointType;
    typedef typename InputImageType::IndexType InputImageIndexType;
    typedef typename InputImageType::RegionType InputImageRegionType;
    typedef typename InputImageType::SizeType InputImageSizeType;

     typedef typename TemplateImage::Pointer TemplateImagePointer;

    typedef typename itk::LinearInterpolateImageFunction<InputImageType> InterpolatorType;
    typedef typename InterpolatorType::Pointer InterpolatorPointer;

    typedef itk::Image<unsigned char, 3> TextureImageType;
    typedef typename TextureImageType::Pointer TextureImagePointer;

    typedef SheetnessImageCreator<InputImageType> SheetnessFilterType;
    typedef typename SheetnessFilterType::Pointer SheetnessFilterPointer;


    typedef itk::ExtractImageFilter<TextureImageType,TextureImageType> ROIFilterType;
    typedef typename ROIFilterType::Pointer ROIFilterPointer;

    typedef ibia::ScalarTextureCalculator<TextureImageType> ScalarTextureCalculatorType;
    typedef typename ScalarTextureCalculatorType::Pointer ScalarTextureCalculatorPointer;

    typedef itk::VectorContainer<unsigned int, std::vector<double> > ContainerMatrixType;
    typedef typename ContainerMatrixType::Pointer ContainerMatrixPointer;

    typedef itk::ImageRegionIteratorWithIndex<TextureImageType> ROIIteratorType;

    typedef itk::FiniteCylinderSpatialFunction<3> CylinderSpatialFunctionType;
    typedef typename CylinderSpatialFunctionType::Pointer CylinderSpatialFunctionPointer;

    typedef itk::CastImageFilter<InputImageType, TextureImageType> TextureImageCasterType;
    typedef typename TextureImageCasterType::Pointer TextureImageCasterPointer;

    typedef itk::RescaleIntensityImageFilter<InputImageType,TextureImageType> RescaleIntensityFilterType;
    typedef typename RescaleIntensityFilterType::Pointer RescaleIntensityFilterPointer;

    itkNewMacro(Self);
    itkTypeMacro(Self, Superclass);

    SampleExtractor();
    ~SampleExtractor(void);
    void ExtractFeatures(const unsigned int numPoints,const float distance );
    InputPixelType GetIntensityAtPosition(InputImagePointType& point);
    InputPixelType GetSheetnessAtPosition(InputImagePointType& point);
    void CalculateSheetness();
    void SetInputImage(InputImagePointer image){ m_InputImage=image;}
    void SetTemplateImage(TemplateImagePointer image){ m_TemplateImage=image;}

    void SetInputPolyData(vtkPolyData* polyData){
      m_PolyData=polyData;
    }

    vtkSmartPointer<vtkFloatArray> GetSampleMaxSheetnessContainer(){return m_SampleMaxSheetnessContainer;}
    vtkSmartPointer<vtkIntArray> GetClassifierResultMaxSheetness(){return m_MaxSheetnessClassifier;}

//    void ExtractRowToVTKFLoatArray(alglib::real_2d_array array, vtkSmartPointer<vtkFloatArray> vtkArray,unsigned int col)
//    {
//    	for (unsigned int row=0; row<array.rows();row++)
//    		vtkArray->InsertNextTuple((float*)array(col,row));
//    }

//    void ConvertToVTKFLoatArray(alglib::integer_1d_array array, vtkSmartPointer<vtkFloatArray> vtkArray)
//    {
//    	for (unsigned int row=0; row<array.length();row++)
//    		vtkArray->InsertNextTuple((float*)array(row));
//    }

    void PerformKMeans(bool km){m_DoKMeans=km;}

    void SetSuffix(std::string s){m_Suffix=s;}
    std::string GetSuffix(){return m_Suffix;}

    void ComputeMeanSurfaceCurvature();

    void SetExtractCrestOnly(bool extract)
    {
        this->m_ExtractCrestOnly=extract;
    }

    void SetRemoveCrest(bool remove)
    {
        this->m_RemoveCrest=remove;
    }

    void SetExtractTextureFeatures(bool extract)
    {
        this->m_ExtractTextureFeatures=extract;
    }

    void SetExtractSurfacePoints(bool extractSurf)
    {
        this->m_ExtractSurfacePoints=extractSurf;
    }

    void SetNumberOfBinsForTextureFeatureCalc(unsigned int bins)
    {
        this->m_NumberOfBinsForTextureFeatures=bins;
    }

    void SetCylinderRadius(float radius)
    {
        this->m_CylinderRadius=radius;
    }

    void SetCylinderAxisLength(float aLength)
    {
        this->m_CylinderAxisLength=aLength;
    }


   protected:

    void ExtractPointsUsingMaxSheetness();

    bool m_DoKMeans;

    bool m_ExtractCrestOnly,m_RemoveCrest;
    float m_CrestCurvatureValue;

    float m_CylinderAxisLength,m_CylinderRadius;

    bool m_ExtractTextureFeatures,m_ExtractSurfacePoints;

    unsigned int m_NumberOfBinsForTextureFeatures;

    ContainerMatrixPointer m_TextureMatrix;
    vtkSmartPointer<vtkDoubleArray> m_TextureTuple;
    vtkSmartPointer<vtkIntArray> m_MaxSheetnessClassifier;

    vtkSmartPointer<vtkComputeLocalGeometry> m_CurvatureCalculator;
    vtkDataArray* m_CurvatureArray;
    vtkSmartPointer<vtkIntArray> m_CrestPointsArray;

    ROIFilterPointer m_ROIFilterImage, m_ROIFilterMask;

    ScalarTextureCalculatorPointer m_ScalarTextureCalc;

    void ExtractCubeRegionAtPosition(InputImagePointType point,InputImageSizeType size,double*);

    TextureImagePointer m_ROIImage,m_ROIImageMask;

    CylinderSpatialFunctionPointer m_CylinderSpFunction;

    RescaleIntensityFilterPointer m_RescaleFilter;

    TextureImageType::Pointer m_TextureImage;

  private:
    SampleExtractor(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    vtkSmartPointer<vtkFloatArray> m_SampleMaxSheetnessContainer;
    InterpolatorPointer m_LinearInterpolator;
    InterpolatorPointer m_LinearSheetnessInterpolator;
    InputImagePointer m_InputImage;
    TemplateImagePointer m_TemplateImage;
    vtkPolyData* m_PolyData;

    SheetnessFilterPointer m_SheetnessFilter;
    InputImagePointer m_SheetnessImage;

    std::string m_Suffix;
    float m_Thresh;

  };
} // namespace
#ifndef ITK_MANUAL_INSTANTIATION
#include "SampleExtractor.txx"
#endif

#endif /* SampleExtractor_H_ */
