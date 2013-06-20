/*
 * SheetnessImageCreator.h
 *
 *  Created on: 12.09.2011
 *      Author: kfritscher
 */

#ifndef SheetnessImageCreator_H_
#define SheetnessImageCreator_H_

#include "itkLightObject.h"
#include "itkImage.h"

#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkSymmetricEigenAnalysisImageFilter.h"
#include "itkDescoteauxSheetnessImageFilter.h"


  /** \class SampleExtractor
  *
  * \brief
  *
  * \author K. Fritscher
  **/
template<class InputImageType>
  class SheetnessImageCreator : public itk::LightObject
  {
   public:
    typedef SheetnessImageCreator<InputImageType> Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    typedef itk::WeakPointer<const Self>  ConstWeakPointer;

    typedef typename InputImageType::Pointer InputImagePointer;
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename InputImageType::PointType InputImagePointType;

    typedef itk::HessianRecursiveGaussianImageFilter< InputImageType >  HessianFilterType;
    typedef typename HessianFilterType::Pointer HessianFilterPointer;
     typedef typename HessianFilterType::OutputImageType                          HessianImageType;
     typedef typename HessianImageType::PixelType                                 HessianPixelType;

     typedef  itk::FixedArray< double, HessianPixelType::Dimension >     EigenValueArrayType;
     typedef  itk::Image< EigenValueArrayType, HessianPixelType::Dimension >               EigenValueImageType;

     typedef  itk::SymmetricEigenAnalysisImageFilter<
       HessianImageType, EigenValueImageType >     EigenAnalysisFilterType;
     typedef typename EigenAnalysisFilterType::Pointer EigenAnalysisFilterPointer;

     typedef  itk::DescoteauxSheetnessImageFilter< EigenValueImageType, InputImageType >   SheetnessFilterType;
     typedef typename SheetnessFilterType::Pointer SheetnessFilterPointer;


    itkNewMacro(Self);
    itkTypeMacro(Self, Superclass);


    SheetnessImageCreator();
    ~SheetnessImageCreator(void);

    void doFilter(InputImagePointer input);

    InputImagePointer GetSheetnessImage(){return m_SheetnessImage;}
   

   protected:
    
    SheetnessFilterPointer sheetnessFilter;
    HessianFilterPointer hessianFilter;
    EigenAnalysisFilterPointer eigenAnalysis;

    InputImagePointer m_SheetnessImage;

   private:
    SheetnessImageCreator(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented


  };

#ifndef ITK_MANUAL_INSTANTIATION
#include "SheetnessImageCreator.txx"
#endif

#endif /* SheetnessImageCreator_H_ */
