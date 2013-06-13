#ifndef __ibiaApplyTransformationFromMatrix_h
#define __ibiaApplyTransformationFromMatrix_h

#include <fstream>
#include <iostream>

#include "itkLinearInterpolateImageFunction.h"
#include "itkScalableAffineTransform.h"
#include "itkResampleImageFilter.h"
#include "itkSpatialObjectReader.h"
#include "itkWindowedSincInterpolateImageFunction.h"
#include "itkConstantBoundaryCondition.h"
#include "itkCastImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

#include "itkTransformMeshFilter.h"

#include "itkMesh.h"
#include "itkMeshSpatialObject.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkTriangleMeshToBinaryImageFilter.h"
#include "itkDefaultDynamicMeshTraits.h"
#include "ibiaGaussianFilter.h"
#include "ibiaComputeSignedDistanceMapFilter.h"




namespace ibia
{

/** \class ApplyTransformationFromMatrix
  *
  * \brief Applies a transformation read from file
  *
  **/

class ApplyTransformationFromMatrix : public itk::LightObject
{
private:
    //TODO remove fixed image type and create template class
    typedef itk::Image<unsigned char,3> LabelType;
    typedef LabelType::Pointer LabelPointer;

    typedef itk::Image<float,3> InputImageType;
    typedef InputImageType::Pointer IPointer;

    typedef itk::Function::LanczosWindowFunction<4>  WindowFunctionType;

    typedef itk::ConstantBoundaryCondition< InputImageType >  BoundaryConditionType;

    typedef itk::WindowedSincInterpolateImageFunction<InputImageType,4,  WindowFunctionType,BoundaryConditionType,double  > InterpolatorType;
    typedef itk::ScalableAffineTransform<double,3>  TransformType;
    typedef TransformType::MatrixType itkMatrix;
    typedef itk::SpatialObjectReader<>      GroupReaderType;
    typedef GroupReaderType::GroupType      GroupType;

    typedef ibia::ComputeSignedDistanceMapFilter<LabelType, InputImageType> SignedDistanceMapFilterType;
    typedef SignedDistanceMapFilterType::Pointer SignedDistanceMapFilterPointer;

    typedef itk::BinaryThresholdImageFilter<InputImageType,LabelType> BinaryThresholdFilterType;
    typedef BinaryThresholdFilterType::Pointer BinaryThresholdFilterPointer;

    typedef ibia::GaussianFilter<InputImageType, InputImageType> GaussianFilterType;
    typedef GaussianFilterType::Pointer GaussianFilterPointer;

    typedef itk::ResampleImageFilter<InputImageType,InputImageType> ResFilterType;

    typedef itk::CastImageFilter<InputImageType,LabelType> ImageToLabelCastFilterType;
    typedef ImageToLabelCastFilterType::Pointer ImageToLabelCastFilterPointer;

    typedef itk::CastImageFilter<LabelType,InputImageType> LabelToImageCastFilterType;
    typedef LabelToImageCastFilterType::Pointer LabelToImageCastFilterPointer;

    short m_Quality;
    ApplyTransformationFromMatrix(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    TransformType::Pointer m_Transform;
    BinaryThresholdFilterPointer m_BinaryThresholdFilter;
    GaussianFilterPointer m_GaussianBlurFilter;

public:
    typedef ibia::ApplyTransformationFromMatrix Self;
    typedef itk::LightObject Superclass;
    typedef itk::SmartPointer< Self > Pointer;
    typedef itk::SmartPointer< const Self > ConstPointer;
    typedef itk::WeakPointer<const Self>  ConstWeakPointer;

    itkNewMacro(Self);
    itkTypeMacro(Self, Superclass);
    ApplyTransformationFromMatrix();
    ~ApplyTransformationFromMatrix(void);
    IPointer doFilter(IPointer image, std::string matrixFilePath,bool invert);
    IPointer doFilter(IPointer image,std::string matrixFile,IPointer referenceImage,bool invert,bool binary);
    IPointer doFilter(IPointer image,TransformType::Pointer transform,IPointer referenceImage,bool invert);
    LabelPointer doFilterBinary(LabelPointer image, TransformType::Pointer transform, IPointer referenceImage, bool invert);
    void SetQuality(short val){m_Quality=val;}
    TransformType::Pointer GetTransform(){ return m_Transform;}

    TransformType::Pointer GetTransformFromFile(std::string matrixFile);
};
} // namespace
#endif // __ibiaApplyTransformationFromMatrix_h
