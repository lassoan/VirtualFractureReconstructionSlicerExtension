#ifndef __ibiaCorrespondingPointsIdentifier_h
#define __ibiaCorrespondingPointsIdentifier_h

#include "ibiaComputeSignedDistanceMapFilter.h"
#include "MeshOps.h"
#include "DeepCopy.h"
#include "TransformWriter.h"
#include "FileOutputWriter.h"
#include "ibiaMemIni.h"
#include "ibiaCorrespondingPointsInitializer.h"
#include "ActivePointSelector.h"
#include "ibiaApplyTransformationFromMatrix.h"

#include "itkPointSet.h"
#include "itkMesh.h"
#include "itkScalableAffineTransform.h"
#include "itkCastImageFilter.h"
#include "itkFlipImageFilter.h"
#include "itkEuclideanDistancePointMetric.h"
#include "itkEuler3DTransform.h"
#include "itkBinaryThresholdImageFilter.h"

#include "vtkTransformPolyDataFilter.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkComputeLocalGeometry.h"
#include "vtkTransform.h"
#include "vtkPolyDataWriter.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "itkCastImageFilter.h"
#include "itkImageRegionConstIterator.h"

#include "vtkPolyDataReader.h"


#include "SampleExtractor.h"

#include "BoundingBoxCalculator.h"

#include "ComputeMahalanobisDistance.h"


namespace ibia
{
/**
 * \class CorrespondingPointsIdentifier
 *
 * \brief
 *
 * \author
 */

class ICPWrapper;

  template<class LabelType, class InputImageType>
  class CorrespondingPointsIdentifier
  {
  public:

    typedef itk::PointSet<float, 3> PointSetType;
    typedef PointSetType::Pointer PointSetPointer;
    typedef PointSetType::PointType PointType;

    typedef typename InputImageType::Pointer InputImagePointer;

    typedef typename LabelType::Pointer LabelPointer;


    static const unsigned int PointDimension = 3;
    static const unsigned int MaxCellDimension = 2;
    typedef itk::DefaultStaticMeshTraits<double, PointDimension, MaxCellDimension, double, double> MeshTraits;
    typedef itk::Mesh<double, PointDimension, MeshTraits> MeshType;

    typedef itk::PointSet<float, 3, MeshTraits> PointSetTypeICP;
    typedef typename PointSetTypeICP::Pointer ICPPointSetPointer;
    typedef typename PointSetTypeICP::PointType PointTypeICP;

    typedef itk::Image<float, 3> DistImageType;
    typedef typename DistImageType::Pointer DistImagePointer;

    typedef ibia::ComputeSignedDistanceMapFilter<LabelType, DistImageType> SignedDistanceMapFilterType;

    typedef typename ibia::DeepCopy<LabelType> LabelImageCloneType;
    typedef typename LabelImageCloneType::Pointer LabelImageClonePointer;

    typedef typename ibia::DeepCopy<InputImageType > ImageCloneType;
    typedef typename ImageCloneType::Pointer ImageClonePointer;

    typedef itk::ScalableAffineTransform<double, 3> ITKTransformType;
    typedef ITKTransformType::Pointer ITKTransformPointer;
    typedef typename ITKTransformType::MatrixType ITKMatrixType;
    typedef typename ITKTransformType::OffsetType ITKTransformOffsetType;

    typedef itk::EuclideanDistancePointMetric<PointSetType, PointSetType> EuclideanMetricType;
    typedef typename EuclideanMetricType::Pointer EuclideanMetricPointer;

    //* Set up a Transform */
    typedef itk::Euler3DTransform<double> TransformType;
    typedef typename TransformType::Pointer TransformPointer;

    typedef CorrespondingPointsInitializer<LabelType,InputImageType> TransformInitializerType;
    typedef typename TransformInitializerType::Pointer TransformInitializerPointer;

    typedef itk::SpatialObjectWriter<> SPWriterType;

    typedef itk::GroupSpatialObject<> GroupType;
    typedef itk::ScalableAffineTransform<double, 3> SPTransformType;


    typedef itk::BinaryThresholdImageFilter<LabelType,LabelType> BinaryThresholdFilterType;
    typedef typename BinaryThresholdFilterType::Pointer BinaryThresholdFilterPointer;

    typedef itk::FixedArray<bool,3> FlipAxesArrayType;


    typedef vtkSmartPointer<vtkPolyData>  vtkSmartPolyDataPointer;

    //static Pointer
    //New(bool preRegistration, unsigned int, unsigned int, unsigned int, unsigned int, float, unsigned int, bool,bool,bool,bool,bool,bool);

    void SetSpecialSuffix(std::string ssuffix)
    {
        this->m_SpecialSuffix=ssuffix;
    }

    void SetPerformCorrespondingPointSearch(bool corespondenceSearch)
    {
        this->m_PerformCorrespondingPointSearch=corespondenceSearch;
    }

    void SetExtractReferenceFeatures(bool extractRef)
    {
        this->m_ExtractReferenceFeatures=extractRef;
    }

    void SetPrepTransform(ITKTransformPointer prepTransform)
    {
        this->m_PrepTransform=prepTransform;
    }

    void SetReferenceFileName(std::string filename)
    {
        this->m_ReferenceFileName=filename;
    }

    void SetReferencePolyData(vtkSmartPointer<vtkPolyData> referencePolyData)
    {
        this->m_ReferencePolyData=referencePolyData;
    }

    void SetCandidatePolyData(vtkSmartPointer<vtkPolyData> candidatePolyData)
    {
        this->m_CandidatePolyData=candidatePolyData;
    }

    vtkSmartPointer<vtkPolyData> GetReferencePolyData()
    {
        return this->m_ReferencePolyData;
    }

    void SetLoadReferenceDataFromFile(bool loadRef)
    {
        this->m_LoadReferencePolyDataFromFile=loadRef;
    }

    vtkSmartPointer<vtkPolyData> GetFragmentPolyData()
    {
        return this->m_CandidatePolyData;
    }

  private:

    bool m_UseGreyscaleStatistics,m_PerformCorrespondingPointSearch,m_LoadReferencePolyDataFromFile;

    std::string m_SpecialSuffix, m_ReferenceFileName;

//    DifferenceCalculatorType* m_DifferenceCalculators;

    vtkComputeLocalGeometry *m_GeometricPropCalculator;
    ibia::VTKMarchingCubesFilter<LabelType>* m_CreatePolyDataCandidate;
    ibia::VTKMarchingCubesFilter<LabelType>* m_CreatePolyDataReference;
    ibia::VTKMarchingCubesFilter<DistImageType>* m_CreatePolyDataIni;
    vtkSmartPolyDataPointer m_ReferencePolyData;
    vtkSmartPolyDataPointer m_CandidatePolyData;

//    SelectiveDDMPointer m_SDDMCreator;

    LabelPointer m_CandidateImageIni;
    LabelPointer m_PreProcessedLabel;

    float m_MaxPointDistance;
    std::vector<float> m_Length;

    unsigned int m_SmoothingIterations;
    unsigned int m_AdjacencyLevelsStart;
    unsigned int m_AdjacencyLevels;
    unsigned int m_VertIndex;
    float m_MaxDist;
    bool m_PreRegistration;
    float m_CurrentMetric;
    float m_MCSigma;

    bool m_ReferenceMode,m_ExtractReferenceFeatures;

    std::vector<std::vector<float> > m_DifferenceVector;
    std::vector<std::vector<float> > m_DifferenceVectorGrey;
    std::vector<std::vector<float> > m_BestDifferenceVectors;
    std::vector<std::vector<float> > m_BestDifferenceVectorsGrey;

    std::vector< std::vector<float> > m_CandidateCurvatureOutputVector;
    std::vector< std::vector<float> > m_ReferenceCurvatureOutputVector;

    std::vector<itk::Point<float, 3> > m_RefTwinList;

    std::vector<unsigned int> m_NumTwins;

    vtkComputeLocalGeometry *m_ReferenceGeometry;
    vtkComputeLocalGeometry *m_CandidateGeometry;
    AdjData m_ReferenceNeighbourhood;
    AdjData m_CandidateNeighbourhood;
    int m_CurrentAdjacencyLevel;
    int m_CurrentCandidatePoint;
    unsigned int m_NumberOfReferenceVerts;
    int m_CurrentCandidateIndex;
    unsigned int m_NumberOfTestPoints;
    unsigned int m_StepSize;
    bool m_CalculateCorrelation;
    bool m_PerformGreyscaleRegistration;
    bool m_UseExtendedICP;
    bool m_CalculateDifferenceVectors;

    bool m_ExtractReferencePoints;

    vtkPoints* m_IdentifiedReferencePoints;
    vtkPoints* m_IdentifiedCandidatePoints;

    vtkPoints* m_IdentifiedReferencePointsGrey;
    vtkPoints* m_IdentifiedCandidatePointsGrey;

    vtkPoints* m_ExtractedCandidateSurfacePoints;
    vtkPoints* m_ExtractedReferenceSurfacePoints;

    PointSetPointer m_CandidatePointsVector;
    PointSetPointer m_ReferencePointsVector;

    PointSetPointer m_CandidatePointsVectorGrey;
    PointSetPointer m_ReferencePointsVectorGrey;

//    SampleDifferenceCalculatorPointer m_SampleDifferenceCalculator;

    LabelImageClonePointer m_Clone;

    ImageClonePointer m_ImageClone;

    InputImagePointer m_GreyscaleCandidateImage;
    InputImagePointer m_GreyscaleReferenceImage;

    ITKTransformPointer m_InitialITKTransformDM;
    ITKTransformPointer m_InitialITKTransformPS;
    ITKTransformPointer m_CurrentTransform;
    ITKTransformPointer m_PrepTransform;

    BinaryThresholdFilterPointer m_BinaryThresholdFilter;

    vtkSmartPolyDataPointer m_ResultPolyData;

    InputImagePointer m_ResultGreyImage;

//    alglib::integer_1d_array m_KMeansResultReference;
    bool m_ReferenceSurfaceIndex;
    bool m_CandidateSurfaceIndex;

    multimap<unsigned int, std::vector<itk::Point<float, 3> > > m_CandidateTwinMap;
    multimap<unsigned int, std::vector<itk::Point<float, 3> > > m_ReferenceTwinMap;

    multimap<unsigned int, std::vector<itk::Point<float, 3> > > m_CandidateTwinMapGrey;
    multimap<unsigned int, std::vector<itk::Point<float, 3> > > m_ReferenceTwinMapGrey;

    ICPPointSetPointer m_ICPMovingPointsVector;
    ICPPointSetPointer m_ICPFixedPointsVector;

//    FragmentGreyscaleRegistrationPointer m_GreyscaleRegistrator;

    std::vector<float> m_BestDifferenceValues;
    std::vector<float> m_BestDifferenceValuesGrey;

    std::string m_OutputDirectory;
    float m_FinalMetricValue;

    ICPWrapper* m_Emicp;
    MemIni* m_Ini;
    TransformInitializerPointer m_TransformInitializer;

    LabelPointer
    PerformPreProcessing(LabelPointer referenceImage, LabelPointer candidateImage, bool flipReference, bool performDMReg=true);

//    double
//    CalculateNeighbourhoodDifferences(const int, const unsigned int numNeighbours,
//        const unsigned int currentIndex);
//    double CalculateSampleDifferences(unsigned int candidateIndex, unsigned int referenceIndex);
//    void
//    CalculateGeometricDifferenceVector(int startIndex, int endIndex, const int threadID,alglib::integer_1d_array kmeansresult, bool index);

//    static ITK_THREAD_RETURN_TYPE ThreaderCallback(void *arg);

    void ThresholdLabelInPlace(LabelPointer label, int lowerThresh=0, int upperThresh=5000,int in=1, int out=0);
     // FlipImageType FlipLabel(LabelPointer label, FlipAxesArrayType flipaxes[3]);

    void
    StartThreadedCalculation(const int ThreadId, const int ThreadNum);

//    void IdentifyBestPoints(PointSetPointer candidatePointsVector,PointSetPointer referencePointsVector,std::vector<std::vector<float> >& differenceVector,std::vector<std::vector<float> >& bestDifferenceVectors,std::vector<float>& bestDifferenceValues, bool distanceMode=true);
//    void IdentifyTwins( PointSetPointer candidatePointsVector, PointSetPointer referencePointsVector,std::vector<std::vector<float> > & bestDifferenceVector,
//        std::vector<float> &  bestDifferenceValues,
//        multimap<unsigned int, std::vector<itk::Point<float, 3> > > &  refTwinMap,
//        multimap<unsigned int, std::vector<itk::Point<float, 3> > > & candTwinMap);

    vtkSmartPolyDataPointer
    TransformPolyData(vtkSmartPolyDataPointer polyData, ITKTransformPointer transform, bool invert=0);

    void TransformCandidateObjectEnsemble(LabelPointer candidateImage,ITKTransformPointer transform, bool flip);


    InputImagePointer TransformGreyImage(InputImagePointer greyImage, ITKTransformPointer transform, bool invert,InputImagePointer referenceImage);
    LabelPointer TransformBinaryImage(LabelPointer binaryImage, ITKTransformPointer transform,bool invert,InputImagePointer referenceImage) ;
//    void ExtractPointsFromPolyData(vtkPolyData* polyData, vtkPoints* pointSet,alglib::integer_1d_array classifierArray, const int classifier=1);
    void ExtractPointsFromPolyData(vtkPolyData* polyData, vtkPoints* pointSet);
    void ExtractPointsFromPolyData(vtkPolyData* polyData, PointSetPointer pointSet);

    void SortPoints(PointSetPointer candidatePointsVector,PointSetPointer referencePointsVector,multimap<unsigned int, std::vector<itk::Point<float, 3> > > referenceTwinMap,
        multimap<unsigned int, std::vector<itk::Point<float, 3> > > candidateTwinMap, vtkPoints *referencePoints, vtkPoints* candidatePoints,std::vector<float>  bestDiff, float percentage);

    bool m_RefOutFinished;

    bool m_FlipReference;

    LabelPointer FlipLabel(LabelPointer label, FlipAxesArrayType flipaxes);

    InputImagePointer FlipImage(InputImagePointer label, FlipAxesArrayType flipaxes);

  public:


    CorrespondingPointsIdentifier(MemIni *iniFile);
    virtual ~CorrespondingPointsIdentifier();

    void
    SetGreyscaleCandidateImage(InputImagePointer greyImage)
    {
      m_GreyscaleCandidateImage = greyImage;
    }

    void
    SetGreyscaleReferenceImage(InputImagePointer greyImage)
    {
      m_GreyscaleReferenceImage = greyImage;
    }

    LabelPointer
    doFilter(LabelPointer inImage, LabelPointer overlayImage);

//    vtkSmartPolyDataPointer
//    CalculateGeometricProperties(vtkPolyData* polyDataInput, int adjacencyLevels, int smoothIts);

    vtkPoints*
    GetIdentifiedReferencePoints()
    {
      return m_IdentifiedReferencePoints;
    }

    vtkPoints*
    GetIdentifiedCandidatePoints()
    {
      return m_IdentifiedCandidatePoints;
    }

    vtkPoints*
    GetIdentifiedReferencePointsGrey()
    {
      return m_IdentifiedReferencePointsGrey;
    }

    vtkPoints*
    GetIdentifiedCandidatePointsGrey()
    {
      return m_IdentifiedCandidatePointsGrey;
    }

    vtkSmartPolyDataPointer
    GetResultPolyData()
    {
      return m_ResultPolyData;
    }

    InputImagePointer
    GetResultGreyImage()
    {
      return m_ResultGreyImage;
    }

    ITKTransformPointer
    GetInitialPSTransform()
    {
      return m_InitialITKTransformPS;
    }

    ITKTransformPointer
    GetCurrentTransform()
    {
      return m_CurrentTransform;
    }

    std::vector<float>
    GetBestDifferenceValues()
    {
      return m_BestDifferenceValues;
    }

    std::vector<float>
    GetBestDifferenceValuesGrey()
    {
      return m_BestDifferenceValuesGrey;
    }

    bool GetPreRegistration()
    {
      return m_PreRegistration;
    }

    std::vector<std::vector<float> >& GetDifferenceValues()
    {
      return m_DifferenceVector;
    }

    std::vector<std::vector<float> > GetDifferenceValuesGrey()
    {
      return m_DifferenceVectorGrey;
    }

    void SetOutputDirectory(std::string outputDir)
    {
      m_OutputDirectory=outputDir;
    }


    void SetParameterFileInstance(MemIni* ini)
    {
      m_Ini=ini;
    }

    void SetFlipReference(bool flipReference)
    {
        this->m_FlipReference=flipReference;
    }

  private:
    CorrespondingPointsIdentifier(CorrespondingPointsIdentifier&); // prevent default copy constructor



  };
// class CorrespondingPointsIdentifier



} // namespace ibia
#ifndef ITK_MANUAL_INSTANTIATION
#include "ibiaCorrespondingPointsIdentifier.txx"
#endif
#endif // __ibiaCorrespondingPointsIdentifier_h
