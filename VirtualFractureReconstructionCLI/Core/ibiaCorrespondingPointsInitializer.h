/*
 * ibiaCorrespondingPointsInitializer.h
 *
 *  Created on: 08.03.2012
 *      Author: kfritscher
 */

#ifndef ibiaCorrespondingPointsInitializer_H_
#define ibiaCorrespondingPointsInitializer_H_

#include "itkLightObject.h"
#include "itkImage.h"

#include "ibiaComputeSignedDistanceMapFilter.h"
#include "ImageRegistrationApp.h"
#include "ibiaGaussianFilter.h"
#include "ibiaApplyTransformationFromMatrix.h"
#include "SampleExtractor.h"
#include "ICPWrapper.h"

#include "ibiaSurfacePointExtractor.h"
#include "ibiaVTKMarchingCubes.h"

#include "vtkWeakPointer.h"
#include "ActivePointSelector.h"

#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkExtractSelectedPolyDataIds.h"
#include "vtkInformation.h"
#include "vtkPolyDataConnectivityFilter.h"
#include"vtkThreshold.h"


  /** \class Corresponding points initializer
  *
  * \brief
  *
  * \author K. Fritscher
  **/
template<class InputImageType, class GreyscaleImageType>
  class CorrespondingPointsInitializer : public itk::LightObject
  {
   public:
    typedef CorrespondingPointsInitializer<InputImageType,GreyscaleImageType> Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    typedef itk::WeakPointer<const Self>  ConstWeakPointer;

    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename InputImageType::PointType InputImagePointType;

    typedef typename GreyscaleImageType::Pointer GreyscaleImagePointer;

    itkNewMacro(Self);
    itkTypeMacro(Self, Superclass);

    typedef itk::Image<short, InputImageType::ImageDimension> ShortImageType;
    typedef itk::Image<float, InputImageType::ImageDimension> DistImageType;

    typedef ibia::ComputeSignedDistanceMapFilter<InputImageType, DistImageType> SignedDistanceMapFilterType;
    typedef typename SignedDistanceMapFilterType::Pointer SignedDistanceMapFilterPointer;

    typedef ibia::GaussianFilter<InputImageType, InputImageType> GaussianFilterType;
    typedef typename GaussianFilterType::Pointer GaussianFilterPointer;

    typedef typename DistImageType::Pointer DistImagePointer;

    typedef ImageRegistrationApp<DistImageType> ImageRegistrationAppType;
    typedef typename ImageRegistrationAppType::Pointer ImageRegistrationPointer;

    typedef itk::SpatialObjectWriter<> SPWriterType;

    typedef itk::GroupSpatialObject<> SPGroupType;
    typedef typename SPGroupType::Pointer SPGroupPointer;
    typedef itk::ScalableAffineTransform<double, 3> SPTransformType;
    typedef typename SPTransformType::Pointer SPTransformPointer;

    typedef itk::ImageFileWriter<InputImageType> WriterType;
    typedef typename WriterType::Pointer WriterPointer;



    typedef itk::ScalableAffineTransform<double, 3> ITKTransformType;
    typedef ITKTransformType::Pointer ITKTransformPointer;

    typedef SurfacePointExtractor<GreyscaleImageType> SurfacePointExtractorType;
    typedef typename SurfacePointExtractorType::Pointer SurfacePointExtractorPointer;

    typedef ibia::VTKMarchingCubesFilter<InputImageType> MarchingCubesFilterType;

    typedef itk::Array<  double >    MeasurementVectorType;


    typedef vtkSmartPointer<vtkPolyData> PolyDataPointer;



    CorrespondingPointsInitializer();
    ~CorrespondingPointsInitializer(void);

    void AlignLabels(InputImagePointer referenceImage, InputImagePointer candidateImage, bool performDMReg,float mcSigma=0.5);
    ITKTransformPointer doFilter(vtkSmartPointer<vtkPolyData> referencePolyData, vtkSmartPointer<vtkPolyData> candidatePolyData, float referenceDistance=FLT_MAX);
    ITKTransformPointer AlignPolyData(vtkSmartPointer<vtkPolyData> referencePolyData, vtkSmartPointer<vtkPolyData> candidatePolyData);
    ITKTransformPointer GetInitialITKTransformDM()
    {
      return m_InitialITKTransformDM;
    }

    ITKTransformPointer GetInitialITKTransformPS()
    {
      return m_InitialITKTransformPS;
    }

    void SetParameters(MemIni* iniFile)
    {
      m_Ini=iniFile;
    }

    void SetPercentageOfReferencePoints(unsigned int perct)
    {
      m_RefPerct=perct;
    }

    void SetPercentageOfCandidatePoints(unsigned int perct)
    {
      m_CandPerct=perct;
    }


    void SetExtractReferencePoints(bool exref)
    {
      m_ExtractReferencePoints=exref;
    }

    void SetExtractCandidatePoints(bool excand)
    {
      m_ExtractCandidatePoints=excand;
    }

    void SetCandidateGreyscaleImage(GreyscaleImagePointer candgrey)
    {
      this->m_CandidateGreyscaleImage=candgrey;
    }

    void SetReferenceGreyscaleImage(GreyscaleImagePointer refgrey)
    {
      this->m_ReferenceGreyscaleImage=refgrey;
    }

    void SetAtlas(DistImagePointer atlas){
      m_Atlas=atlas;
    }

    vtkDataArray* GetMahalanobisDistance()
    {
        return m_MDistances;
    }

    void SetMaximumMahalanobisDistance(float maxMahal)
    {
        this->m_MaxMahalanobisDistance=maxMahal;
    }
   
    void SetCandidateFeatures(MeasurementVectorType candidateFeatures)
    {
        this->m_CandidateFeatures=candidateFeatures;
    }

    void SetReferenceFeatures(MeasurementVectorType referenceFeatures)
    {
        this->m_CandidateFeatures=referenceFeatures;
    }

    float GetMaximumMahalnobisDistance()
    {
        return this->m_MaxMahalanobisDistance;
    }

    bool GetRegistrationPerformed()
    {
        return  this->m_RegistrationPerformed;
    }

    void SetEstimateSigmaP2Init(bool estimate)
    {
        this->m_EstimateSigmaP2Ini=estimate;
    }


    float GetSigmaP2Init()
    {
        return this->m_SigmaP2Ini;
    }

    void SetUseActiveSetOnly(bool active)
    {
        this->m_UseOnlyActiveSet=active;
    }

    void SetUseActiveReferencePointsForMahalanobis(bool useActive)
    {
      this->m_UseActiveReferencePoints=useActive;
    }


   protected:
    unsigned int m_CandPerct,m_RefPerct;
    bool m_UseActiveReferencePoints;

    DistImagePointer m_Atlas;


    ICPWrapper* m_EMICP;

    SurfacePointExtractorPointer m_PointsExtractorCandidate,m_PointsExtractorReference;

    ITKTransformPointer m_InitialITKTransformDM;
    ITKTransformPointer m_InitialITKTransformPS;

    GreyscaleImagePointer m_CandidateGreyscaleImage;
    GreyscaleImagePointer m_ReferenceGreyscaleImage;

    MemIni* m_Ini;

    bool m_ExtractCandidatePoints, m_ExtractReferencePoints,m_ExtractCandidateCrest,m_ExtractReferenceCrest,m_RegistrationPerformed;

    MeasurementVectorType m_CandidateFeatures,m_ReferenceFeatures;

    float m_MaxMahalanobisDistance,m_SigmaP2Ini,m_D02ini;

    bool m_EstimateSigmaP2Ini,m_UseOnlyActiveSet;


    void ReadRegistrationParameters();
    void WriteReferencePointInformationToFile(std::string filename,vtkSmartPointer<vtkPolyData> referencePolyData);

    vtkSmartPointer<vtkPolyData> CorrectSurfaceClassification(vtkSmartPointer<vtkPolyData> polyInput);
    //vtkSmartPointer<vtkPolyData> ExtractPointsFromSurfaceCells(vtkSmartPointer<vtkPolyData> candidatePolyData);

  protected:
    vtkSmartPointer<vtkPolyData> m_ReferencePolyData;
    vtkSmartPointer<vtkPolyData> m_CandidatePolyData;
    ActivePointSelector* m_ActiveCandidatePointSelector;

    void ExtractPointsFromReference();
    void ExtractPointsFromCandidate();
    void ExtractPoints();
    void EstimateSigmaP2Ini();
    void SetEMICPParameters();
    void SelectSafeCandidatePointsForDistanceCalculation( float radius);

   private:
    CorrespondingPointsInitializer(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    vtkSmartPointer<vtkDoubleArray> m_MDistances;


  };

#ifndef ITK_MANUAL_INSTANTIATION
#include "ibiaCorrespondingPointsInitializer.txx"
#endif

#endif /* ibiaCorrespondingPointsInitializer_H_ */
