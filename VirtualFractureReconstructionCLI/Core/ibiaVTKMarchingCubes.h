#ifndef __ibiaVTKMarchingCubes_H
#define __ibiaVTKMarchingCubes_H

#include "vtkImageConstantPad.h"
#include "vtkContourFilter.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataNormals.h"
#include "vtkDecimatePro.h"
#include "vtkTriangleFilter.h"
#include "vtkSmartPointer.h"

#include "itkImageToVTKImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImage.h"

#pragma once

/** \class VTKMarchingCubesFilter
  *
	*\brief Class to create asurface model of label data using VTK marching cubes algorithm.
  *
  * \Author K. Fritscher
  **/
namespace ibia
{
  template <class TInputImage>
  class VTKMarchingCubesFilter
  {
    typedef TInputImage InputImageType;
    typedef typename InputImageType::Pointer InputImagePointer;

    typedef itk::Image<float,3> ImageTypeFloat;

    typedef itk::CastImageFilter<InputImageType,ImageTypeFloat> CastImageFilterType;
    typedef typename CastImageFilterType::Pointer CastFilterPointer;
    typedef itk::ImageToVTKImageFilter<ImageTypeFloat>  ITK2VTKType;
    typedef typename ITK2VTKType::Pointer ITK2VTKPointer;
    typedef vtkSmartPointer<vtkPolyData> PolyData;

    vtkSmartPointer<vtkPolyDataNormals>                       m_PolyNormals;
    CastFilterPointer                                         m_Caster;
    double m_DecimationFactor;

    public:
      VTKMarchingCubesFilter();
      ~VTKMarchingCubesFilter();
      PolyData MarchingCubesFilter(InputImagePointer inImage, float sigma=1);
      void SetDecimationFactor(double deci){m_DecimationFactor=deci;}
      float GetDecimationFactor(){return m_DecimationFactor;}
  };
}
#ifndef ITK_MANUAL_INSTANTIATION
#include "ibiaVTKMarchingCubes.txx"
#endif
#endif
