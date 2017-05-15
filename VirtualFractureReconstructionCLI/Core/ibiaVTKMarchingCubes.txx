#ifndef __ibiaVTKMarchingCubes_txx
#define __ibiaVTKMarchingCubes_txx

#include <ibiaVTKMarchingCubes.h>
#include <ibiaGaussianFilter.h>



namespace ibia
{

/**
 * \brief Constructor
 *   member variables are instantiate within the constructor
 *
 **/
template <class TInputImage>
VTKMarchingCubesFilter<TInputImage>::VTKMarchingCubesFilter()
{
    m_PolyNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
    m_Caster = CastImageFilterType::New();
    m_DecimationFactor=0;
}


/**
 * \brief Destructor
 *   member variables are instantiate within the constructor
 *
 **/
template <class TInputImage>
VTKMarchingCubesFilter<TInputImage>::~VTKMarchingCubesFilter()
{

    //m_VTKImporter->Delete();
    m_PolyNormals=NULL;
}

/**
 * \brief Method to create surface from binary image using vtkContourFilter
 *        Surface gets smoothed by using decimate pro and smooth poly data filter.
 *
 * \param inImage - IPointer, smart pointer to (itk) input image
 *
 * \return VTKMarchingCubesFilter::PolyData  Surface representation (vtkPolyData)
 **/
template <class TInputImage>
typename VTKMarchingCubesFilter<TInputImage>::PolyData
VTKMarchingCubesFilter<TInputImage>::MarchingCubesFilter(InputImagePointer inImage,float sigma)
{
    ITK2VTKPointer vtkImporter = ITK2VTKType::New();
    m_Caster->SetInput(inImage);
    m_Caster->UpdateLargestPossibleRegion();
    typename ibia::GaussianFilter<ImageTypeFloat,ImageTypeFloat>::Pointer gauss = ibia::GaussianFilter<ImageTypeFloat,ImageTypeFloat>::New();
    gauss->SetSigma(sigma);
    vtkImporter->SetInput(gauss->doFilter(m_Caster->GetOutput()));
    vtkImporter->Update();

    vtkSmartPointer<vtkImageConstantPad> pad = vtkSmartPointer<vtkImageConstantPad>::New();

    pad->SetInputData(vtkImporter->GetOutput());

    vtkSmartPointer<vtkContourFilter> mc = vtkSmartPointer<vtkContourFilter>::New();
    mc->SetInputConnection(pad->GetOutputPort());
    mc->SetValue(0,0.5);
    vtkSmartPointer<vtkDecimatePro> deci = vtkSmartPointer<vtkDecimatePro>::New();
    deci->SetInputConnection(mc->GetOutputPort());
    deci->SetTargetReduction(this->m_DecimationFactor);
    deci->PreserveTopologyOn();


    m_PolyNormals->SetInputConnection(deci->GetOutputPort());
    m_PolyNormals->FlipNormalsOn();
    m_PolyNormals->Update();

    vtkImporter=NULL;
    return m_PolyNormals->GetOutput();
}
}
#endif

