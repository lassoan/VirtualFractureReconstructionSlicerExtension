#ifndef __ibiaGaussianFilter_h
#define __ibiaGaussianFilter_h

#include "itkLightObject.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

namespace ibia
{

  /**
  * \class GaussianFilter
  *
  * \brief
  *
  * \author Klaus Fritscher, Jan Schreiber
  * \todo Provide class description
  **/
  template <class TInputImage, class TOutputImage>
  class GaussianFilter : public itk::LightObject
  {
    public:
    typedef ibia::GaussianFilter <TInputImage,TOutputImage> Self;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    typedef itk::WeakPointer<const Self>  ConstWeakPointer;
    typedef itk::LightObject Superclass;

    itkNewMacro(Self);
    itkTypeMacro(Self, Superclass);

    typedef TInputImage InputImageType;
    typedef TOutputImage OutputImageType;
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename OutputImageType::Pointer OutputImagePointer;

    ///Typedef for a gaussian filter
    typedef itk::RecursiveGaussianImageFilter<InputImageType, OutputImageType >  GaussianFilterType;
    typedef typename GaussianFilterType::Pointer GaussianFilterPointer;

	///Typedef for a rescale intensity filter
	//typedef itk::RescaleIntensityImageFilter<InputImageType, OutputImageType > RescaleFilterTypeRG;
  public:


    virtual OutputImagePointer doFilter(InputImagePointer inImage);

    void SetSigma(float sigma)
    {
        m_SigmaVal=sigma;
    }


  protected:
    GaussianFilter();
    virtual ~GaussianFilter();
    double m_SigmaVal;
  private:
     // no default constructor
    GaussianFilter(GaussianFilter&); // no copy constructor
    GaussianFilterPointer m_GaussianFilterX;
    GaussianFilterPointer m_GaussianFilterY;
    GaussianFilterPointer m_GaussianFilterZ;
  };

} // namespace
#ifndef ITK_MANUAL_INSTANTIATION
#include "ibiaGaussianFilter.txx"
#endif
#endif // __ibiaGaussianFilter_h
