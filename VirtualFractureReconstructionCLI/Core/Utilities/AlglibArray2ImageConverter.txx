#ifndef AlglibArray2ImageConverter_txx_
#define AlglibArray2ImageConverter_txx_

/*
 * AlglibArray2ImageConverter.cpp
 *
 *  Created on: 15.09.2011
 *      Author: kfritscher
 */

#include "AlglibArray2ImageConverter.h"


template<class InputImageType,class OutputImageType, class ArrayType>
AlglibArray2ImageConverter<InputImageType,OutputImageType,ArrayType>::AlglibArray2ImageConverter() {
	// TODO Auto-generated constructor stub
	m_ResultImage = OutputImageType::New();

}

template<class InputImageType,class OutputImageType, class ArrayType>
AlglibArray2ImageConverter<InputImageType,OutputImageType,ArrayType>::~AlglibArray2ImageConverter() {
	// TODO Auto-generated destructor stub
}


template<class InputImageType,class OutputImageType, class ArrayType>
typename AlglibArray2ImageConverter<InputImageType,OutputImageType, ArrayType>::OutputImagePointer
AlglibArray2ImageConverter<InputImageType,OutputImageType, ArrayType>::doFilter(ReferenceImagePointer referenceImage, alglib::real_2d_array pointsCoords,
    InputArrayType inputArray)
{
 return this->doFilter(referenceImage->GetLargestPossibleRegion().GetSize(),pointsCoords, inputArray);
}


template<class InputImageType,class OutputImageType, class ArrayType>
typename AlglibArray2ImageConverter<InputImageType,OutputImageType, ArrayType>::OutputImagePointer
AlglibArray2ImageConverter<InputImageType,OutputImageType, ArrayType>::doFilter(OutputImageSizeType imageSize,alglib::real_2d_array pointsCoords, InputArrayType inputArray)
{
  OutputImageRegionType region;
  OutputImageIndexType index;
  index[0]=0;
  index[1]=0;
  index[2]=0;

  region.SetIndex(index);
  region.SetSize(imageSize);

  OutputImageSpacingType spacing;
  spacing[0]=1;
  spacing[1]=1;
  spacing[2]=1;

  m_ResultImage->SetSpacing(spacing);
  m_ResultImage->SetRegions(region);
  m_ResultImage->Allocate();
  m_ResultImage->FillBuffer(0);

  IteratorType outputIterator(m_ResultImage, m_ResultImage->GetLargestPossibleRegion());

  typename OutputImageType::PointType point;
  typename OutputImageType::IndexType runningindex;

  for (unsigned int coord = 0; coord < pointsCoords.rows(); coord++)
  {

    for (unsigned int comp = 0; comp < 3; comp++)
      point[comp] = pointsCoords(coord, comp);
    m_ResultImage->TransformPhysicalPointToIndex(point, runningindex);
    outputIterator.SetIndex(runningindex);
    outputIterator.Set(inputArray(coord));
    ++outputIterator;
  }
  return m_ResultImage;
}

#endif
