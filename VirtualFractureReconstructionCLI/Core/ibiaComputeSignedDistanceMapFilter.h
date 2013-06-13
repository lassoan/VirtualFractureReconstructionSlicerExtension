#ifndef __ibiaComputeSignedDistanceMapFilter_h
#define __ibiaComputeSignedDistanceMapFilter_h

#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkLightObject.h"


namespace ibia
{

  /**
  * \class ComputeSignedDistanceMapFilter
  *
  * \brief Computes Daniellson Distance map of binary images
  *
  * \author Klaus Fritscher
  **/

  template <class TInputImage, class TOutputImage=TInputImage>
  class ComputeSignedDistanceMapFilter : public itk::LightObject
  {
    public:
    typedef ibia::ComputeSignedDistanceMapFilter <TInputImage,TOutputImage> Self;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    typedef itk::WeakPointer<const Self>  ConstWeakPointer;

    itkNewMacro(Self);
    itkTypeMacro(Self, Superclass);

    typedef TInputImage InputImageType;
    typedef TOutputImage OutputImageType;
    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename OutputImageType::Pointer OutputImagePointer;


    typedef itk::SignedMaurerDistanceMapImageFilter<InputImageType,OutputImageType> SignedDistanceCalculatorType;
    typedef typename SignedDistanceCalculatorType::Pointer SignedDistanceCalculatorPointer;

  public:

    virtual OutputImagePointer doFilter(InputImagePointer inImage);

  protected:

    ComputeSignedDistanceMapFilter();
    virtual ~ComputeSignedDistanceMapFilter(void);
    SignedDistanceCalculatorPointer m_SignedDistance;
    ComputeSignedDistanceMapFilter(ComputeSignedDistanceMapFilter&); // no copy constructor
  };

} // namespace
#ifndef ITK_MANUAL_INSTANTIATION
#include "ibiaComputeSignedDistanceMapFilter.txx"
#endif
#endif // __ComputeSignedDistanceMapFilter_h
