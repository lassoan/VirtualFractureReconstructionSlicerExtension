/*
 * BoundingBoxCalulator.cpp
 *
 *  Created on: 01.12.2011
 *      Author: kfritscher
 */

#include "BoundingBoxCalculator.h"

BoundingBoxCalculator::BoundingBoxCalculator()
{
  m_ImageMask = MaskType::New();

}

BoundingBoxCalculator::~BoundingBoxCalculator()
{
  // TODO Auto-generated destructor stub
}

BoundingBoxCalculator::OutputRegionType BoundingBoxCalculator::CalculateBoundingBoxRegion(InputImagePointer inputImage){
  m_ImageMask->SetImage(inputImage);
  m_ImageMask->Update();
  m_OutputRegion=m_ImageMask->GetAxisAlignedBoundingBoxRegion();
  return m_OutputRegion;
}

