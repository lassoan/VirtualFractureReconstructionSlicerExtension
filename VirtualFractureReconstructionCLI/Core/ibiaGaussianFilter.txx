#ifndef __ibiaGaussianFilter_txx
#define __ibiaGaussianFilter_txx

#include "ibiaGaussianFilter.h"

namespace ibia
{


/**
 * \brief Standard constructor
 *
 * \author K. Fritscher
 **/
template <class TInputImage, class TOutputImage>
GaussianFilter<TInputImage, TOutputImage>::GaussianFilter()
{
    m_GaussianFilterX = GaussianFilterType::New();
    m_GaussianFilterY = GaussianFilterType::New();
    m_GaussianFilterZ = GaussianFilterType::New();
}



/**
* \brief Destructor
*
* \author Jan Schreiber
**/
template <class TInputImage, class TOutputImage>
GaussianFilter<TInputImage, TOutputImage>::~GaussianFilter(void)
{
    m_GaussianFilterX = NULL;
    m_GaussianFilterY = NULL;
    m_GaussianFilterZ = NULL;
}


/**
  * \brief This smoothing filter is recursively convulting an image with a gaussian kernel
  *
 * \param inImage - InputImagePointer, pointer to image data to be filtered
  *
 * \return OutputImagePointerPointer to filter output(=image)
    */
template <class TInputImage, class TOutputImage>
typename GaussianFilter<TInputImage, TOutputImage>::OutputImagePointer
GaussianFilter<TInputImage, TOutputImage>::doFilter(InputImagePointer inImage)
{

    m_GaussianFilterX->SetInput(inImage);
    m_GaussianFilterX->SetDirection( 0 );
    m_GaussianFilterX->SetOrder( GaussianFilterType::ZeroOrder );
    m_GaussianFilterX->SetNormalizeAcrossScale( false );
    m_GaussianFilterX->SetSigma(m_SigmaVal);
    m_GaussianFilterX->Update();

    m_GaussianFilterY->SetInput( m_GaussianFilterX->GetOutput() );
    m_GaussianFilterY->SetDirection( 1 );
    m_GaussianFilterY->SetNormalizeAcrossScale( false );
    m_GaussianFilterY->SetOrder( GaussianFilterType::ZeroOrder );
    m_GaussianFilterY->SetSigma(m_SigmaVal);
    m_GaussianFilterY->Update();
    if(inImage->GetLargestPossibleRegion().GetSize()[2]>=3)
    {
        m_GaussianFilterZ->SetInput( m_GaussianFilterY->GetOutput() );
        m_GaussianFilterZ->SetOrder( GaussianFilterType::ZeroOrder );
        m_GaussianFilterZ->SetDirection( 2 );
        m_GaussianFilterZ->SetNormalizeAcrossScale( false );
        m_GaussianFilterZ->SetSigma(m_SigmaVal);
        m_GaussianFilterZ->Update();
        return m_GaussianFilterZ->GetOutput();
    }
    else
    {
        return m_GaussianFilterY->GetOutput();
    }
}
}
#endif
