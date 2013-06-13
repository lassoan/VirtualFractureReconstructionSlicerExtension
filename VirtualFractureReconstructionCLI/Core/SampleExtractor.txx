#include "SampleExtractor.h"

using namespace ibia;
using namespace std;

#include "itkImageFileWriter.h"

#include "itkImageRegionIterator.h"

#include "vtkMath.h"

#include "vtkPolyDataWriter.h"


/**
 * \brief Constructor, initializes member variables
 *
 * \author  K. Fritscher
 **/
template<class InputImageType, class TemplateImageType>
SampleExtractor< InputImageType, TemplateImageType>::SampleExtractor()
{
    m_LinearInterpolator = InterpolatorType::New();
    m_LinearSheetnessInterpolator = InterpolatorType::New();
    //    m_MatrixWriter = new AlglibArrayWriter();
    //    m_BasicStatisticsCalc = new AlglibBasicStatistics();
    //    m_KMeansCalculator = new AlglibKMeansCalculator();
    //    m_KMeansCalculatorIntensity = new AlglibKMeansCalculator();
    m_CurvatureCalculator = vtkComputeLocalGeometry::New();
    m_ROIFilterImage = ROIFilterType::New();
    m_ROIFilterMask = ROIFilterType::New();
    m_ScalarTextureCalc = ScalarTextureCalculatorType::New();
    m_TextureMatrix = ContainerMatrixType::New();
    m_TextureTuple= vtkSmartPointer<vtkDoubleArray>::New();
    m_MaxSheetnessClassifier=vtkSmartPointer<vtkIntArray>::New();
    m_CylinderSpFunction= CylinderSpatialFunctionType::New();
    m_RescaleFilter = RescaleIntensityFilterType::New();

    //m_SheetnessFilter = SheetnessFilterType::New();
    m_DoKMeans = true;

    m_CurvatureArray=NULL;
    m_RemoveCrest=true;
    m_ExtractCrestOnly=false;
    m_ExtractSurfacePoints=true;

    m_SheetnessImage = NULL;
    m_InputImage = NULL;
    m_TemplateImage = NULL;
    m_TextureImage=NULL;
    m_Suffix="";
    m_Thresh=0.8;
    m_CrestCurvatureValue=-0.1;
    m_NumberOfBinsForTextureFeatures=10;
    m_CylinderAxisLength=4;
    m_CylinderRadius=2;
}

/**
 * \brief Destructor
 *
 * \author K. Fritscher
 **/
template<class InputImageType, class TemplateImageType>
SampleExtractor< InputImageType, TemplateImageType>::~SampleExtractor()
{

}

/**
 * \brief Extracts samples from images along point normals of vtkPolyData
 *
 * \param
 *
 * \author K. Fritscher
 **/

template<class InputImageType, class TemplateImageType>
void
SampleExtractor< InputImageType, TemplateImageType>::ExtractFeatures(const unsigned int numPoints, const float distance)
{
    if (m_InputImage)
    {
        m_LinearInterpolator->SetInputImage(m_InputImage);

        double *pointCoord = 0;
        InputImagePointType samplePosition;
        float sheetness = 0;
        double steplength = distance / (double) (numPoints);

        m_RescaleFilter->SetInput(this->m_InputImage);
        m_RescaleFilter->SetOutputMinimum(0);
        m_RescaleFilter->SetOutputMaximum(255);
        m_RescaleFilter->Update();
        m_TextureImage=m_RescaleFilter->GetOutput();

        InputImageSizeType size;
        size[0]=10;
        size[1]=10;
        size[2]=10;

        unsigned int textureComponents=m_ScalarTextureCalc->GetNumberOfRequestedFeatures();
        m_ScalarTextureCalc->SetNumberOfBinsPerAxis(this->m_NumberOfBinsForTextureFeatures);
        unsigned int shift=1;

        m_CylinderSpFunction->SetAxisLength(this->m_CylinderAxisLength);
        m_CylinderSpFunction->SetRadius(this->m_CylinderRadius);

        if(this->m_ExtractTextureFeatures )
        {
            m_TextureTuple->SetName("HaralickFeatures");
            m_TextureTuple->SetNumberOfComponents(textureComponents);
            m_TextureTuple->SetNumberOfTuples(m_PolyData->GetPointData()->GetNumberOfTuples());
            this->m_PolyData->GetPointData()->AddArray(m_TextureTuple);
            m_TextureMatrix->Reserve(m_PolyData->GetPointData()->GetNumberOfTuples());
        }

        if(!this->m_PolyData->GetPointData()->GetArray("CrestPoints"))
        {
            this->m_CrestPointsArray =vtkSmartPointer<vtkIntArray>::New();
            this->m_CrestPointsArray->SetNumberOfComponents(1);
            this->m_CrestPointsArray->SetName("CrestPoints");
            this->m_CrestPointsArray->SetNumberOfTuples(m_PolyData->GetPointData()->GetNumberOfTuples());
            this->m_CrestPointsArray->FillComponent(0,0);
            this->m_PolyData->GetPointData()->AddArray(m_CrestPointsArray);
        }

        if(this->m_ExtractSurfacePoints && !m_PolyData->GetPointData()->GetArray("SurfacePointsClassifier"))
        {
            this->m_SampleMaxSheetnessContainer = vtkSmartPointer<vtkFloatArray>::New();
            this->m_SampleMaxSheetnessContainer->SetNumberOfComponents(1);
            this->m_SampleMaxSheetnessContainer->SetName("SampleMaxSheetness");
            this->m_SampleMaxSheetnessContainer->SetNumberOfTuples(m_PolyData->GetPointData()->GetNumberOfTuples());
            this->m_SampleMaxSheetnessContainer->FillComponent(0,0);

            //Calculate sheetness
            std::cout << "Calculating sheetness" << std::endl;
            this->CalculateSheetness();
            m_LinearSheetnessInterpolator->SetInputImage(this->m_SheetnessImage);
            itk::Image<float, 3>::Pointer testimage = itk::Image<float, 3>::New();
            testimage->CopyInformation(m_InputImage);
            testimage->SetRegions(m_InputImage->GetLargestPossibleRegion());
            testimage->Allocate();
            testimage->FillBuffer(0);
            std::cout << "sheetness image created" << std::endl;
        }



        if(this->m_ExtractSurfacePoints && !m_PolyData->GetPointData()->GetArray("SurfaceMeanCurvature"))
        {
            std::cout << "Computing surface curvature for crest extraction/removal" << std::endl;
            this->ComputeMeanSurfaceCurvature();
            std::cout << "Crest extraction/removal finished. " << std::endl;
        }

        if(this->m_ExtractTextureFeatures)
            std::cout << "Starting feature extraction"<<std::endl;

        for (unsigned int pointNumber = 0; pointNumber < m_PolyData->GetNumberOfPoints(); pointNumber++)
        {
            std::vector<double> tex;
            double normal[3];
            m_PolyData->GetPointData()->GetNormals()->GetTuple(pointNumber,&normal[0]);
            pointCoord = m_PolyData->GetPoint(pointNumber);

            if(this->m_ExtractTextureFeatures )
            {
                for (unsigned int k = 0; k < 3; k++)
                {
                    samplePosition[k] = pointCoord[k]  + (normal[k]*m_InputImage->GetSpacing()[0] * shift);
                }
                this->ExtractCubeRegionAtPosition(samplePosition,size,normal);
                tex=this->m_ScalarTextureCalc->doFilter(this->m_ROIImage,this->m_ROIImageMask);
                m_TextureMatrix->SetElement(pointNumber,tex);

                double* texArrayStatic=new double[textureComponents];

                for (unsigned int t=0;t<textureComponents;t++)
                    texArrayStatic[t]=tex[t];

                double* texArrayVTK=(double*)m_TextureTuple->GetVoidPointer(pointNumber*textureComponents);
                for (unsigned int t2=0;t2<textureComponents;t2++)
                    texArrayVTK[t2]=texArrayStatic[t2];
                delete texArrayStatic;
            }

            for (unsigned int sampleNumber = 1; sampleNumber <= numPoints+1; sampleNumber++)
            {
                for (unsigned int k = 0; k < 3; k++)
                {
                    samplePosition[k] = pointCoord[k] - ((normal[k] * floor(distance / 2.0)*m_InputImage->GetSpacing()[0])) + (normal[k]*m_InputImage->GetSpacing()[0] * steplength * (sampleNumber - 1));
                }


                if(this->m_ExtractSurfacePoints)
                {
                    sheetness = this->GetSheetnessAtPosition(samplePosition);

                    if (sheetness > m_SampleMaxSheetnessContainer->GetTuple1(pointNumber) )
                        m_SampleMaxSheetnessContainer->SetTuple1(pointNumber, sheetness);

                }

            }
        }
        std::cout << "Statistics calculated" << std::endl;

        //Add array to polydata
        if(this->m_ExtractTextureFeatures)
        {
            for (unsigned int t=0;t<this->m_PolyData->GetPointData()->GetNumberOfTuples();t++)
            {
                std::cout<<t+1<<" : ";
                double*  temp=(double*)this->m_PolyData->GetPointData()->GetArray("HaralickFeatures")->GetVoidPointer(t*textureComponents);
                for (unsigned int x=0;x<textureComponents;x++)
                    std::cout<<temp[x]<<"  ";
                std::cout<<std::endl;
            }
        }

        if(this->m_ExtractSurfacePoints && !m_PolyData->GetPointData()->GetArray("SurfacePointsClassifier"))
        {

            m_MaxSheetnessClassifier->SetName("SurfacePointsClassifier");
            m_MaxSheetnessClassifier->SetNumberOfTuples(m_SampleMaxSheetnessContainer->GetNumberOfTuples());
            m_MaxSheetnessClassifier->SetNumberOfComponents(1);

            this->ExtractPointsUsingMaxSheetness();

            this->m_PolyData->GetPointData()->AddArray(m_MaxSheetnessClassifier);

        }
    }
    else
    {
        std::cout << "Input image not set" << std::endl;
    }

}


template<class InputImageType, class TemplateImageType>
void
SampleExtractor< InputImageType, TemplateImageType>::ExtractPointsUsingMaxSheetness()
{
    float curvature=0;

    for(int i=0;i<m_SampleMaxSheetnessContainer->GetNumberOfTuples();i++)
    {
        curvature=this->m_CurvatureArray->GetTuple1(i);
        if(curvature<this->m_CrestCurvatureValue && m_SampleMaxSheetnessContainer->GetTuple1(i)>m_Thresh) this->m_CrestPointsArray->SetTuple1(i,1);
        if(this->m_ExtractCrestOnly)
        {

            if(curvature<this->m_CrestCurvatureValue)/*m_SampleMaxSheetnessContainer(i)>m_Thresh &&*/
            {
                m_MaxSheetnessClassifier->SetTuple1(i,1);
            }
            else
            {
                m_MaxSheetnessClassifier->SetTuple1(i,0);
            }
        }
        else if(!this->m_RemoveCrest)
        {
            if(m_SampleMaxSheetnessContainer->GetTuple1(i)>m_Thresh)
            {

                m_MaxSheetnessClassifier->SetTuple1(i,1);
            }
            else
            {
                m_MaxSheetnessClassifier->SetTuple1(i,0);
            }
        }
        else
        {
            curvature=this->m_CurvatureArray->GetTuple1(i);
            if(m_SampleMaxSheetnessContainer->GetTuple1(i)>m_Thresh && curvature>m_CrestCurvatureValue && curvature!=0)
            {
                m_MaxSheetnessClassifier->SetTuple1(i,1);
            }
            else
            {
                m_MaxSheetnessClassifier->SetTuple1(i,0);
            }
        }
    }
}

/**
 * \brief Extracts samples from images along point normals of vtkPolyData
 *
 * \author K. Fritscher
 **/

template<class InputImageType, class TemplateImageType>
typename SampleExtractor< InputImageType, TemplateImageType>::InputPixelType
SampleExtractor< InputImageType, TemplateImageType>::GetIntensityAtPosition(InputImagePointType& point)
{
    return m_LinearInterpolator->Evaluate(point);
}

template<class InputImageType, class TemplateImageType>
void
SampleExtractor< InputImageType, TemplateImageType>::ExtractCubeRegionAtPosition(InputImagePointType point,InputImageSizeType size,double* axis)
{

    m_CylinderSpFunction->SetCenter(point);
    m_CylinderSpFunction->SetOrientation(axis);

    InputImageIndexType sourceIndex, cornerIndex;
    this->m_InputImage->TransformPhysicalPointToIndex( point, sourceIndex );
    InputImageRegionType regionOfInterest;
    for(unsigned int i=0;i<3;i++)
    {
        cornerIndex[i]=sourceIndex[i]-size[i]/2;
        if(cornerIndex[i]<0) cornerIndex[i]=0;
        if((sourceIndex[i]+size[i]/2)>this->m_TextureImage->GetLargestPossibleRegion().GetSize()[i])
        {
            size[i]=(this->m_TextureImage->GetLargestPossibleRegion().GetSize()[i]-sourceIndex[i])*2;
            cornerIndex[i]=sourceIndex[i]-size[i]/2;
        }
    }
    regionOfInterest.SetIndex(cornerIndex);
    regionOfInterest.SetSize(size);
    m_ROIFilterImage->SetInput(this->m_TextureImage);
    m_ROIFilterImage->SetExtractionRegion(regionOfInterest);//SetRegionOfInterest(regionOfInterest);
    m_ROIFilterImage->UpdateLargestPossibleRegion();
    this->m_ROIImage=m_ROIFilterImage->GetOutput();

    m_ROIImageMask=TextureImageType::New();
    m_ROIImageMask->CopyInformation(m_ROIFilterImage->GetOutput());
    m_ROIImageMask->SetRegions(m_ROIFilterImage->GetOutput()->GetLargestPossibleRegion());
    m_ROIImageMask->Allocate();
    m_ROIImageMask->FillBuffer(0);

    ROIIteratorType roiIter(m_ROIImageMask,m_ROIImageMask->GetLargestPossibleRegion());
    TextureImageType::PointType p;
    roiIter.GoToBegin();
    int inside=0;
    while(!roiIter.IsAtEnd())
    {
        this->m_ROIImageMask->TransformIndexToPhysicalPoint(roiIter.GetIndex(),p);
        if(m_CylinderSpFunction->Evaluate(p))
        {
            roiIter.Set(1);
            inside++;
        }
        ++roiIter;
    }
}

/**
 * \brief Extracts samples from images along point normals of vtkPolyData
 *
 * \param
 *
 * \author K. Fritscher
 **/

template<class InputImageType, class TemplateImageType>
typename SampleExtractor< InputImageType, TemplateImageType>::InputPixelType
SampleExtractor< InputImageType, TemplateImageType>::GetSheetnessAtPosition(InputImagePointType& point)
{

    return m_LinearSheetnessInterpolator->Evaluate(point);
}

/**
 * \brief Extracts samples from images along point normals of vtkPolyData
 *
 * \param
 *
 * \author K. Fritscher
 **/

template<class InputImageType, class TemplateImageType>
void
SampleExtractor< InputImageType, TemplateImageType>::CalculateSheetness()
{

    if (m_InputImage)
    {
        m_SheetnessFilter = SheetnessFilterType::New();
        m_SheetnessFilter->doFilter(m_InputImage);
        m_SheetnessImage = m_SheetnessFilter->GetSheetnessImage();
    }
    else
        std::cout << "Input image not set" << std::endl;
}


///**
// * \brief Extracts samples from images along point normals of vtkPolyData
// *
// * \param
// *
// * \author K. Fritscher
// **/

//template<class InputImageType, class TemplateImageType>
//bool
//SampleExtractor< InputImageType, TemplateImageType>::GetLargerClusterIndex()
//{
//    //select larger cluster
//    int zeros = 0, ones = 0;
//    bool clusterindex = 0;
//    for (int cl = 0; cl < this->GetKMResultSheetness().length(); cl++)
//    {
//        if (this->GetKMResultSheetness()(cl) == 0)
//            zeros++;
//        else
//            ones++;
//    }

//    if ((ones > zeros))
//    {
//        clusterindex = 1;
//    }
//    else
//        clusterindex=0;
//    return clusterindex;
//}

//template<class InputImageType, class TemplateImageType>
//alglib::real_2d_array
//SampleExtractor< InputImageType, TemplateImageType>::GetLargerCluster(bool sheet)
//{
//    int indexL;
//    if(!sheet)indexL=this->GetLargerClusterIndex();
//    else
//        indexL=1;
//    int counter=0;
//    alglib::integer_1d_array classifier;
//    if(!sheet) classifier=this->GetKMResultSheetness();
//    else
//        classifier=this->m_ClassifierMaxSheetnessContainer;
//    for (int cl = 0; cl < classifier.length(); cl++)
//    {
//        if(classifier(cl)==indexL)
//        {
//            for (int i=0;i<3;i++)
//                this->m_SamplePositionsContainerLargerCluster(counter,i)=this->m_SamplePositionsContainer(cl,i);
//            counter++;
//        }
//    }
//    return this->m_SamplePositionsContainerLargerCluster;
//}

template<class InputImageType, class TemplateImageType>
void
SampleExtractor< InputImageType, TemplateImageType>::ComputeMeanSurfaceCurvature()
{

    m_CurvatureCalculator = vtkComputeLocalGeometry::New();
    m_CurvatureCalculator->SetInput(this->m_PolyData);
    m_CurvatureCalculator->Set_adj_levels(2);
    m_CurvatureCalculator->Set_H_smooth_its(5);
    m_CurvatureCalculator->UpdateWholeExtent();
    this->m_CurvatureArray=m_CurvatureCalculator->GetOutput()->GetPointData()->GetArray("SurfaceMeanCurvature");
}

