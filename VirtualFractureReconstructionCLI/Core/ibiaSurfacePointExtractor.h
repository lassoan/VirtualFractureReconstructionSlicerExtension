/*
 * ibiaSurfacePointExtractor.h
 *
 *  Created on: 08.03.2012
 *      Author: kfritscher
 */

#ifndef ibiaSurfacePointExtractor_H_
#define ibiaSurfacePointExtractor_H_

#include "itkLightObject.h"
#include "itkImage.h"

#include "SampleExtractor.h"

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkSortDataArray.h"


  /** \class SampleExtractor
  *
  * \brief
  *
  * \author K. Fritscher
  **/

template<class InputImageType>
  class SurfacePointExtractor : public itk::LightObject
  {
   public:
    typedef SurfacePointExtractor<InputImageType> Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    typedef itk::WeakPointer<const Self>  ConstWeakPointer;

    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename InputImageType::PointType InputImagePointType;

    typedef vtkSmartPointer<vtkPolyData> PolyDataPointer;

    typedef SampleExtractor<InputImageType>  SampleExtractorType;
    typedef typename SampleExtractorType::Pointer SampleExtractorPointer;

    typedef itk::Image<float, InputImageType::ImageDimension> DistImageType;
    typedef typename DistImageType::Pointer DistImagePointer;

    SampleExtractorPointer m_Sampler;

    int m_PixNum,m_SampNum;

//    bool m_UseMaxSheet;

    DistImagePointer m_Atlas;
    bool m_UseAtlas;

    PolyDataPointer doFilter(InputImagePointer inputImage, PolyDataPointer inputPolyData,std::string suffix="-cand",bool extractCrest=false );

    PolyDataPointer ExtractPointsFromPolyData(PolyDataPointer inputPolyData, bool useActivePoints);
    PolyDataPointer ExtractPointsFromPolyData(PolyDataPointer inputPolyData,vtkSmartPointer<vtkIntArray> classifierArray, const int classifier);
    PolyDataPointer ExtractPointsFromPolyDataUsingAtlas(PolyDataPointer inputPolyData,DistImagePointer reference);
    PolyDataPointer ExtractPointsFromPolyDataUsingMahalanobisDistance(PolyDataPointer inputPolyData,vtkDoubleArray* mahalanobis, float stdDev);
    PolyDataPointer ExtractNPointsWithSmallestMahalanobisDistance(PolyDataPointer inputPolyData, vtkDoubleArray* mahalanobis, int numberOfPoints, bool useActiveSet=true);

    itkNewMacro(Self);
    itkTypeMacro(Self, Superclass);
    SurfacePointExtractor();
    ~SurfacePointExtractor(void);

//    void SetUseMaxSheetnessForExtraction(bool useMaxSheet)
//    {
//      m_UseMaxSheet=useMaxSheet;
//    }

    void SetAtlas(DistImagePointer atlas){
      m_Atlas=atlas;
    }

    void SetUseAtlas(bool useAtlas)
    {
      m_UseAtlas=useAtlas;
    }

    float GetMaximumMahalnobisDistance()
    {
        return m_MaxMahalnobis;
    }



   protected:

      vtkSmartPointer<vtkPolyData> m_OutputPolyData;

       vtkSmartPointer<vtkIntArray> m_ClosestPointsMahal;

      float m_MaxMahalnobis;
    

   private:
    SurfacePointExtractor(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented


  };

#ifndef ITK_MANUAL_INSTANTIATION
#include "ibiaSurfacePointExtractor.txx"
#endif

#endif /* ibiaSurfacePointExtractor_H_ */
