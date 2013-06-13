#ifndef __ibiaComputeSignedDistanceMapFilter_txx
#define __ibiaComputeSignedDistanceMapFilter_txx

#include "ibiaComputeSignedDistanceMapFilter.h"


namespace ibia
{

/**
* \brief Standard constructor
*
* \author Karl Fritscher
**/

template <class TInputImage, class TOutputImage>
ComputeSignedDistanceMapFilter<TInputImage, TOutputImage>::ComputeSignedDistanceMapFilter()
{
    m_SignedDistance=SignedDistanceCalculatorType::New();
}


/**
* \brief Destructor
*
* \author K. Fritscher
**/
template <class TInputImage, class TOutputImage>
ComputeSignedDistanceMapFilter <TInputImage, TOutputImage>::~ComputeSignedDistanceMapFilter (void)
{
    m_SignedDistance=NULL;
}

template <class TInputImage, class TOutputImage>
typename   ComputeSignedDistanceMapFilter<TInputImage, TOutputImage>::OutputImagePointer
ComputeSignedDistanceMapFilter<TInputImage, TOutputImage>::doFilter(InputImagePointer inImage)
{
    m_SignedDistance->SetInput(inImage);
    m_SignedDistance->SquaredDistanceOff();
    m_SignedDistance->SetInsideIsPositive(1);
    m_SignedDistance->UpdateLargestPossibleRegion();
    return m_SignedDistance->GetOutput();
}
}//end of namespace ibia
#endif
