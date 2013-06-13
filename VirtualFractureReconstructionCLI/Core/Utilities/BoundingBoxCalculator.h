/*
 * BoundingBoxCalculator.h
 *
 *  Created on: 01.12.2011
 *      Author: kfritscher
 */

#ifndef BOUNDINGBOXCALCULATOR_H_
#define BOUNDINGBOXCALCULATOR_H_

#include "itkImageMaskSpatialObject.h"

class BoundingBoxCalculator
{
public:
  typedef itk::Image<unsigned char, 3> InputImageType;
  typedef InputImageType::Pointer InputImagePointer;
  typedef InputImageType::RegionType OutputRegionType;
  //**Typedef for spatial object mask converter*/
  typedef itk::ImageMaskSpatialObject< 3 > MaskType;
  typedef MaskType::Pointer MaskTypePointer;

  BoundingBoxCalculator();

  OutputRegionType CalculateBoundingBoxRegion(InputImagePointer inputImage);
  virtual
  ~BoundingBoxCalculator();
private:
  MaskTypePointer m_ImageMask;
  OutputRegionType m_OutputRegion;
};

#endif /* BOUNDINGBOXCALULATOR_H_ */
