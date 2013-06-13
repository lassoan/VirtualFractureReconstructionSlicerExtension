#ifndef ibiaSurfacePointExtractor_txx_
#define ibiaSurfacePointExtractor_txx_

/*
 * SurfacePointExtractor.cpp
 *
 *  Created on: 08.03.2012
 *      Author: kfritscher
 */

#include "ibiaSurfacePointExtractor.h"

#include <functional>
#include <numeric>

template<class InputImageType>
SurfacePointExtractor<InputImageType>::SurfacePointExtractor() {
	// TODO Auto-generated constructor stub
  m_PixNum=3;
  m_SampNum=3;

  m_Sampler = SampleExtractor<InputImageType>::New();
  m_OutputPolyData = vtkSmartPointer<vtkPolyData>::New();
  m_UseAtlas=false;
  m_MaxMahalnobis=std::numeric_limits<float>::max();

  m_ClosestPointsMahal = vtkSmartPointer<vtkIntArray>::New();
  m_ClosestPointsMahal->SetName("ClosestPoints");
  m_ClosestPointsMahal->SetNumberOfComponents(1);
}

template<class InputImageType>
SurfacePointExtractor<InputImageType>::~SurfacePointExtractor() {
	// TODO Auto-generated destructor stub
}


template<class InputImageType>
typename SurfacePointExtractor<InputImageType>::PolyDataPointer
SurfacePointExtractor<InputImageType>::doFilter(InputImagePointer inputImage, PolyDataPointer inputPolyData,std::string suffix,bool extractCrest ) {

  m_OutputPolyData->DeepCopy(inputPolyData);
  m_Sampler->SetInputImage(inputImage);
  m_Sampler->SetInputPolyData(inputPolyData);
  m_Sampler->PerformKMeans(false);
  m_Sampler->SetSuffix(suffix);
  m_Sampler->SetExtractCrestOnly(extractCrest);
  m_Sampler->SetExtractTextureFeatures(0);
  m_Sampler->ExtractFeatures(m_PixNum, m_SampNum);

  if(m_UseAtlas && m_Atlas)
  {
    this->ExtractPointsFromPolyDataUsingAtlas(inputPolyData, m_Atlas);
  }
  else
  {
    this->m_OutputPolyData=this->ExtractPointsFromPolyData(inputPolyData, m_Sampler->GetClassifierResultMaxSheetness(),1);
  }
  return m_OutputPolyData;
}

/**
 * \brief Method to extract points specified in classifier array from Polydata
 * \author KD Fritscher
 **/
template<class InputImageType>
typename SurfacePointExtractor<InputImageType>::PolyDataPointer
SurfacePointExtractor<InputImageType>::ExtractPointsFromPolyData(PolyDataPointer inputPolyData,vtkSmartPointer<vtkIntArray> classifierArray, const int classifier)
{
  vtkSmartPointer<vtkPoints> pointSet = vtkSmartPointer<vtkPoints>::New();
  m_OutputPolyData->Reset();
  unsigned int numOfRemovedPoints=0;
  for(unsigned int point=0;point<inputPolyData->GetNumberOfPoints();point++)
  {
    if(classifierArray->GetTuple1(point)==classifier)
      {
         pointSet->InsertNextPoint(inputPolyData->GetPoint(point));
      }
      else
      {
          if(inputPolyData->GetPointData()->GetArray("HaralickFeatures"))
              inputPolyData->GetPointData()->GetArray("HaralickFeatures")->RemoveTuple(point-numOfRemovedPoints);
          if(inputPolyData->GetPointData()->GetArray("SurfaceMeanCurvature"))
              inputPolyData->GetPointData()->GetArray("SurfaceMeanCurvature")->RemoveTuple(point-numOfRemovedPoints);
          numOfRemovedPoints++;
      }
  }
  m_OutputPolyData->SetPoints(pointSet);
  if(inputPolyData->GetPointData()->GetArray("HaralickFeatures"))
      m_OutputPolyData->GetPointData()->AddArray(inputPolyData->GetPointData()->GetArray("HaralickFeatures"));
  if(inputPolyData->GetPointData()->GetArray("SurfaceMeanCurvature"))
      m_OutputPolyData->GetPointData()->AddArray(inputPolyData->GetPointData()->GetArray("SurfaceMeanCurvature"));

  return this->m_OutputPolyData;
}

/**
 * \brief Method to extract points specified in classifier array from Polydata
 * \author KD Fritscher
 **/
template<class InputImageType>
typename SurfacePointExtractor<InputImageType>::PolyDataPointer
SurfacePointExtractor<InputImageType>::ExtractPointsFromPolyDataUsingAtlas(PolyDataPointer inputPolyData, DistImagePointer reference)
{
  //this->m_OutputPolyData->Reset();
  vtkSmartPointer<vtkPoints> pointSet = vtkSmartPointer<vtkPoints>::New();
  pointSet->Reset();

  unsigned int numOfRemovedPoints=0;

  typename DistImageType::PointType itkPoint;
  typename DistImageType::IndexType itkIndex;

  std::cout<<"Extracting points using distancemap"<<std::endl;

  for(unsigned int point=0;point<inputPolyData->GetNumberOfPoints();point++)
  {
    for(int i=0;i<3;i++)
      itkPoint[i]=inputPolyData->GetPoint(point)[i];

    reference->TransformPhysicalPointToIndex(itkPoint, itkIndex);

    if(reference->GetPixel(itkIndex)==0)
      {
        pointSet->InsertNextPoint(inputPolyData->GetPoint(point));
      }
    else
    {
        if(inputPolyData->GetPointData()->GetArray("HaralickFeatures"))
            inputPolyData->GetPointData()->GetArray("HaralickFeatures")->RemoveTuple(point-numOfRemovedPoints);
        if(inputPolyData->GetPointData()->GetArray("SurfaceMeanCurvature"))
            inputPolyData->GetPointData()->GetArray("SurfaceMeanCurvature")->RemoveTuple(point-numOfRemovedPoints);
        numOfRemovedPoints++;
    }
  }

    if(inputPolyData->GetPointData()->GetArray("HaralickFeatures"))
        m_OutputPolyData->GetPointData()->AddArray(inputPolyData->GetPointData()->GetArray("HaralickFeatures"));
    if(inputPolyData->GetPointData()->GetArray("SurfaceMeanCurvature"))
        m_OutputPolyData->GetPointData()->AddArray(inputPolyData->GetPointData()->GetArray("SurfaceMeanCurvature"));
    m_OutputPolyData->SetPoints(pointSet);
  return m_OutputPolyData;
}


/**
 * \brief Method to extract points specified in classifier array from Polydata
 * \author KD Fritscher
 **/
template<class InputImageType>
typename SurfacePointExtractor<InputImageType>::PolyDataPointer
SurfacePointExtractor<InputImageType>::ExtractPointsFromPolyData(PolyDataPointer inputPolyData, bool useActivePoints)
{
  //Check if classifier array is available
  if(!inputPolyData->GetPointData()->GetArray("SurfacePointsClassifier")) return inputPolyData;
  if(useActivePoints && !inputPolyData->GetPointData()->GetArray("ActivePoints") ) return inputPolyData;

  vtkSmartPointer<vtkPoints> pointSet = vtkSmartPointer<vtkPoints>::New();
  this->m_OutputPolyData->DeepCopy(inputPolyData);

  for(unsigned int point=0;point<inputPolyData->GetNumberOfPoints();point++)
  {
    if(inputPolyData->GetPointData()->GetArray("SurfacePointsClassifier")->GetTuple1(point)>0)
     {
        if(useActivePoints && inputPolyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(point)>0 )
             pointSet->InsertNextPoint(inputPolyData->GetPoint(point));
        else if(!useActivePoints)
            pointSet->InsertNextPoint(inputPolyData->GetPoint(point));
     }
  }
  if(inputPolyData->GetPointData()->GetArray("HaralickFeatures"))
      m_OutputPolyData->GetPointData()->AddArray(inputPolyData->GetPointData()->GetArray("HaralickFeatures"));
  if(inputPolyData->GetPointData()->GetArray("SurfaceMeanCurvature"))
      m_OutputPolyData->GetPointData()->AddArray(inputPolyData->GetPointData()->GetArray("SurfaceMeanCurvature"));
  m_OutputPolyData->SetPoints(pointSet);

  return this->m_OutputPolyData;
}



/**
 * \brief Method to extract points specified in classifier array from Polydata
 * \author KD Fritscher
 **/
template<class InputImageType>
typename SurfacePointExtractor<InputImageType>::PolyDataPointer
SurfacePointExtractor<InputImageType>::ExtractNPointsWithSmallestMahalanobisDistance(PolyDataPointer inputPolyData, vtkDoubleArray* mahalanobis, int numberOfPoints,bool useActiveSet)
{
    this->m_OutputPolyData->Reset();
    if(!inputPolyData->GetPointData()->GetArray("ClosestPoints"))
    {
        m_ClosestPointsMahal->Initialize();
        m_ClosestPointsMahal->SetNumberOfTuples(inputPolyData->GetNumberOfPoints());
        inputPolyData->GetPointData()->AddArray(m_ClosestPointsMahal);
    }
    else
        this->m_ClosestPointsMahal=vtkIntArray::SafeDownCast(inputPolyData->GetPointData()->GetArray("ClosestPoints"));

    m_ClosestPointsMahal->FillComponent(0,0);
    if(!inputPolyData->GetPointData()->GetArray("ActivePoints") && useActiveSet)
    {
        std::cout<<"ActiveSet not available"<<std::endl;
        return inputPolyData;
    }
    //sort mahalanobis
    vtkSmartPointer<vtkDoubleArray> mahalanobisSort= vtkSmartPointer<vtkDoubleArray>::New();
    mahalanobisSort->DeepCopy(mahalanobis);
    vtkSortDataArray::Sort(mahalanobisSort);
    this->m_MaxMahalnobis=mahalanobisSort->GetTuple1(numberOfPoints*1.2);/////CHANGE?!?!??
    //extract number of active points
    unsigned int activePointsCounter=0;
    for(unsigned int point=0;point<inputPolyData->GetNumberOfPoints();point++)
    {
       if(inputPolyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(point)==1) activePointsCounter++;
    }
    std::cout<<"Maximum Mahalanobis distance: "<<m_MaxMahalnobis<<std::endl;

    std::cout<<"Extracting "<<numberOfPoints<< " points from reference."<<std::endl;

    vtkSmartPointer<vtkPoints> pointSet = vtkSmartPointer<vtkPoints>::New();
    pointSet->Reset();
    unsigned int pointCounter=0;
    for(unsigned int point=0;point<inputPolyData->GetNumberOfPoints();point++)
    {
      //if(inputPolyData->GetPointData()->GetArray("SurfacePointsClassifier") && inputPolyData->GetPointData()->GetArray("SurfacePointsClassifier")->GetTuple1(point)==0) continue;
      if(mahalanobis->GetTuple1(point)<=m_MaxMahalnobis)
      {

         if(useActiveSet && pointCounter>activePointsCounter) break;
         if(useActiveSet && inputPolyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(point)==0) continue;
         pointSet->InsertNextPoint(inputPolyData->GetPoint(point));
         m_ClosestPointsMahal->SetTuple1(point,1);
         pointCounter++;
      }
    }
    std::cout<<"Really extracting "<<pointCounter<< " points from reference."<<std::endl;
    m_OutputPolyData->SetPoints(pointSet);

    return m_OutputPolyData;
}

/**
 * \brief Method to extract points specified in classifier array from Polydata
 * \author KD Fritscher
 **/
template<class InputImageType>
typename SurfacePointExtractor<InputImageType>::PolyDataPointer
SurfacePointExtractor<InputImageType>::ExtractPointsFromPolyDataUsingMahalanobisDistance(PolyDataPointer inputPolyData, vtkDoubleArray* mahalanobis, float stdDev)
{

    this->m_OutputPolyData->Reset();
    std::vector <double>v(mahalanobis->GetSize());
    for (int i=0;i<mahalanobis->GetSize();i++)
        v[i]= *mahalanobis->GetTuple(i);

    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();
    std::vector<double> diff(v.size());
    std::transform(v.begin(), v.end(), diff.begin(), std::bind2nd(std::minus<double>(), mean));
    double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    double maxVal = std::sqrt(sq_sum / v.size())*stdDev;

    vtkSmartPointer<vtkPoints> pointSet = vtkSmartPointer<vtkPoints>::New();
    pointSet->Reset();
    for(unsigned int point=0;point<inputPolyData->GetNumberOfPoints();point++)
    {
      if(v[point]<=maxVal)
        {
          pointSet->InsertNextPoint(inputPolyData->GetPoint(point));
        }
    }
    m_OutputPolyData->SetPoints(pointSet);
    return m_OutputPolyData;
}

#endif
