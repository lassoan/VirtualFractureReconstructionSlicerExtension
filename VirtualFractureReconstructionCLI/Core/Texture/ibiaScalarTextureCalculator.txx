#ifndef __ibiaScalarTextureCalculator_txx
#define __ibiaScalarTextureCalculator_txx

#include "ibiaScalarTextureCalculator.h"
#include "itkImageFileWriter.h"


using namespace ibia;

using namespace std;
/**
* \brief Constructor, initializes member variables
*
* \author  K. Fritscher
**/
template<class InputImageType>
ScalarTextureCalculator<InputImageType>::ScalarTextureCalculator()
{
    m_TextureCalculator=TextureCalculatorType::New();
    m_RequestedFeatures= TextureCalculatorType::FeatureNameVector::New();
    m_RequestedFeatures->push_back(TextureCalculatorType::TextureFeaturesFilterType::Energy);
    m_RequestedFeatures->push_back(TextureCalculatorType::TextureFeaturesFilterType::Entropy);//
    //m_RequestedFeatures->push_back(itk::Statistics::Correlation);
    m_RequestedFeatures->push_back(TextureCalculatorType::TextureFeaturesFilterType::InverseDifferenceMoment);//
    m_RequestedFeatures->push_back(TextureCalculatorType::TextureFeaturesFilterType::Inertia);//
    //m_RequestedFeatures->push_back(itk::Statistics::ClusterShade);//
    //m_RequestedFeatures->push_back(itk::Statistics::ClusterProminence);
    m_RequestedFeatures->push_back(TextureCalculatorType::TextureFeaturesFilterType::HaralickCorrelation);
    m_NumberOfRequestedFeatures=m_RequestedFeatures->Size();
    m_NumberOfBinsPerAxis=10;
}


/**
* \brief Destructor
*
* \author K. Fritscher
**/
template<class InputImageType>
ScalarTextureCalculator<InputImageType>::~ScalarTextureCalculator()
{

}


/**
* \brief Calculates scalar texture features
*
* \param IPointer - Pointer to input image
* \param OverlayImageType::Pointer - Pointer to mask, that defines regions to caluclate texutre features
*
* \return std::vector<float> - vector with average values and standard deviation of features
* \author K. Fritscher
**/
template<class InputImageType>
std::vector<double>
ScalarTextureCalculator<InputImageType>::doFilter(InputImagePointer image)
{
    std::vector<double> m_TextureVector;
    m_TextureVector.clear();


    m_TextureCalculator->SetInput(image);
    m_TextureCalculator->FastCalculationsOn();
    //m_TextureCalculator->SetNumberOfBinsPerAxis(100); //working with features 1,2,7
    m_TextureCalculator->SetNumberOfBinsPerAxis(m_NumberOfBinsPerAxis); //working with 1,2,7;
    m_TextureCalculator->SetRequestedFeatures(m_RequestedFeatures);
    m_TextureCalculator->Update();
    typename TextureCalculatorType::FeatureValueVectorPointer means;//,stdDev;
    means = m_TextureCalculator->GetFeatureMeans();
    //stdDev = m_TextureCalculator->GetFeatureStandardDeviations();

    typename TextureCalculatorType::FeatureValueVector::ConstIterator mIt;
    for (mIt = means->Begin(); mIt != means->End(); ++mIt)
        m_TextureVector.push_back(mIt.Value());

  /* typename itk::ImageFileWriter<InputImageType>::Pointer writer = itk::ImageFileWriter<InputImageType>::New();
    time_t ltime;
    time(&ltime);
    std::stringstream ss;
    ss << ltime;
     writer->SetFileName(ss.str()+"_Patch.mhd");
     writer->SetInput(image);
     writer->Update();

     writer->SetFileName(ss.str()+"_PatchMask.mhd");
     writer->SetInput(this->m_Mask);
     writer->Update();*/

    return m_TextureVector;
}

template<class InputImageType>
std::vector<double>
ScalarTextureCalculator<InputImageType>::doFilter(InputImagePointer image,InputImagePointer mask)
{
    this->m_Mask=mask;
    m_TextureCalculator->SetMaskImage(mask);
    return this->doFilter(image);
}

#endif



