/*
 * AlglibArray2ImageConverter.h
 *
 *  Created on: 15.09.2011
 *      Author: kfritscher
 */

#ifndef AlglibArray2ImageConverter_H_
#define AlglibArray2ImageConverter_H_

#include "itkLightObject.h"
#include "itkImage.h"

#include "ap.h"

#include "itkImageRegionIteratorWithIndex.h"

/** \class SampleExtractor
 *
 * \brief
 *
 * \author K. Fritscher
 **/
template<class InputImageType,class OutputImageType, class ArrayType>
  class AlglibArray2ImageConverter : public itk::LightObject
  {
  public:
    typedef AlglibArray2ImageConverter<InputImageType,OutputImageType, ArrayType> Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;
    typedef itk::WeakPointer<const Self> ConstWeakPointer;


    typedef typename InputImageType::Pointer ReferenceImagePointer;

    typedef typename OutputImageType::Pointer OutputImagePointer;
    typedef typename OutputImageType::PixelType OutputPixelType;
    typedef typename OutputImageType::PointType OutputImagePointType;
    typedef typename OutputImageType::SizeType OutputImageSizeType;
    typedef typename OutputImageType::IndexType OutputImageIndexType;
    typedef typename OutputImageType::SpacingType OutputImageSpacingType;
    typedef typename OutputImageType::RegionType OutputImageRegionType;
    typedef ArrayType InputArrayType;

    typedef itk::ImageRegionIteratorWithIndex<OutputImageType> IteratorType;

    itkNewMacro(Self)
    ;itkTypeMacro(Self, Superclass)
    ;
    AlglibArray2ImageConverter();
    ~AlglibArray2ImageConverter(void);

    OutputImagePointer
    doFilter(ReferenceImagePointer referenceImage, alglib::real_2d_array pointsCoords, InputArrayType inputArray);
    OutputImagePointer
    doFilter(OutputImageSizeType imageSize, alglib::real_2d_array pointsCoords, InputArrayType inputArray);

  protected:

    OutputImagePointer m_ResultImage;

  private:
    AlglibArray2ImageConverter(const Self&); //purposely not implemented
    void
    operator=(const Self&); //purposely not implemented

  };

#ifndef ITK_MANUAL_INSTANTIATION
#include "AlglibArray2ImageConverter.txx"
#endif

#endif /* AlglibArray2ImageConverter_H_ */
