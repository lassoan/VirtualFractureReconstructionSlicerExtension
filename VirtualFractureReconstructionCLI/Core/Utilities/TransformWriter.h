/*
 * TransformWriter.h
 *
 *  Created on: 28.11.2011
 *      Author: kfritscher
 */

#ifndef TRANSFORMWRITER_H_
#define TRANSFORMWRITER_H_

#include "itkSpatialObjectWriter.h"
#include "itkGroupSpatialObject.h"
#include "itkScalableAffineTransform.h"

#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkTransformPolyDataFilter.h"
#include"vtkTransform.h"

class TransformWriter
{

    typedef itk::SpatialObjectWriter<> SPWriterType;
    typedef itk::GroupSpatialObject<> GroupType;
    typedef itk::ScalableAffineTransform<double, 3> SPTransformType;

    typedef itk::ScalableAffineTransform<double, 3> ITKTransformType;
    typedef ITKTransformType::Pointer ITKTransformPointer;
    typedef ITKTransformType::MatrixType ITKMatrixType;
    typedef ITKTransformType::OffsetType ITKTransformOffsetType;


public:
  static void WriteTransform(SPTransformType::Pointer transform, std::string name);
  static vtkPolyData* TransformPolyData(vtkSmartPointer<vtkPolyData> polyData, ITKTransformPointer transform, bool invert);
};


#endif /* TRANSFORMWRITER_H_ */
