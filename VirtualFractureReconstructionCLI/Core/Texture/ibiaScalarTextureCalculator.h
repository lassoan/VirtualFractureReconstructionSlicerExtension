#ifndef __ibiaScalarTextureCalculator_h
#define __ibiaScalarTextureCalculator_h

#include <fstream>
#include <iostream>

#include "itkScalarImageToTextureFeaturesFilter.h"
#include "itkCastImageFilter.h"

namespace ibia
{



  /** \class ScalarTextureCalculator
  *
  * \brief Calculates texture features (refer to Haralick) of a scalar image
  *
  * \author Klaus Fritscher
  **/
template<class InputImageType>
  class ScalarTextureCalculator : public itk::LightObject
  {
   public:
    typedef ibia::ScalarTextureCalculator<InputImageType > Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    typedef itk::WeakPointer<const Self>  ConstWeakPointer;

    typedef typename InputImageType::Pointer InputImagePointer;

    typedef itk::Statistics::ScalarImageToTextureFeaturesFilter<InputImageType> TextureCalculatorType;
    typedef typename TextureCalculatorType::Pointer TextureCalculatorPointer;

    itkNewMacro(Self);
    itkTypeMacro(Self, Superclass);

    ScalarTextureCalculator();
    ~ScalarTextureCalculator(void);
    std::vector<double> doFilter(InputImagePointer image);
    std::vector<double> doFilter(InputImagePointer image,InputImagePointer mask);

    unsigned int GetNumberOfRequestedFeatures()
    {
        return this->m_NumberOfRequestedFeatures;
    }

    void SetNumberOfBinsPerAxis(unsigned int bins)
    {
        this->m_NumberOfBinsPerAxis=bins;
    }

    protected:
        TextureCalculatorPointer m_TextureCalculator;

        typename TextureCalculatorType::FeatureNameVectorPointer m_RequestedFeatures;
        unsigned int m_NumberOfRequestedFeatures;
        unsigned int m_NumberOfBinsPerAxis;
        //std::vector<double> m_TextureVector;
        InputImagePointer m_Mask;

  private:
    ScalarTextureCalculator(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
  };
} // namespace
#ifndef ITK_MANUAL_INSTANTIATION
#include "ibiaScalarTextureCalculator.txx"
#endif
#endif // __ibiaScalarTextureCalculator_h
