#ifndef __RigidRegistrator_h
#define __RigidRegistrator_h

#include "itkImage.h"
#include "itkCommand.h"

#include "itkImageRegistrationMethod.h"
#include "itkVersorRigid3DTransform.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkImageToImageMetric.h"

#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkNormalVariateGenerator.h"
#include "itkFRPROptimizer.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkStatisticsImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"

#include "itkImageMaskSpatialObject.h"

#include "itkImageFileReader.h"

namespace itk
{

template< class TImage >
class RigidRegistrator : public ImageRegistrationMethod < TImage, TImage >
{
public:
    typedef RigidRegistrator Self;
    typedef ImageRegistrationMethod< TImage, TImage> Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkTypeMacro(RigidRegistrator, ImageRegistrationMethod);

    itkNewMacro(Self);

    typedef typename TImage::PixelType PixelType ;
    typedef typename TImage::RegionType RegionType ;

    /** preprocessing related typedefs */
    typedef VersorRigid3DTransform<double>      TransformType ;

    typedef enum { ONEPLUSONE, GRADIENT, ONEPLUSONEPLUSGRADIENT }
                 OptimizerMethodType;

    typedef OnePlusOneEvolutionaryOptimizer     OnePlusOneOptimizerType ;
    typedef FRPROptimizer                     GradientOptimizerType ;
    typedef Statistics::NormalVariateGenerator  OptimizerNormalGeneratorType;
    typedef TransformType::ParametersType       ParametersType ;
    typedef TransformType::ParametersType       ScalesType ;
    typedef LinearInterpolateImageFunction< TImage, double >   InterpolatorType ;
    typedef MattesMutualInformationImageToImageMetric< TImage, TImage >   MetricType ;
    typedef MeanSquaresImageToImageMetric < TImage, TImage >   AlternativeMetricType ;
    typedef ImageToImageMetric < TImage, TImage >   SuperMetricType ;
    typedef typename SuperMetricType::Pointer SuperMetricTypePointer;


    typedef typename Superclass::OptimizerType OptimizerType;
    typedef typename OptimizerType::Pointer OptimizerPointer;

    void Update() ;


    TransformType * GetTypedTransform(void)
    {
        return static_cast<TransformType *>(Superclass::GetTransform());
    }

    void SetOptimizerToOnePlusOne();
    void SetOptimizerToGradient();
    void SetOptimizerToOnePlusOnePlusGradient();

    itkSetMacro(OptimizerNumberOfIterations, unsigned int) ;
    itkGetConstMacro(OptimizerNumberOfIterations, unsigned int) ;
    itkSetMacro(OptimizerScales, ScalesType) ;
    itkGetConstMacro(OptimizerScales, ScalesType) ;

    SuperMetricType * GetTypedMetric(void)
    {
        return static_cast<SuperMetricType *>(Superclass::GetMetric());
    }


    void SetUseAlternativeMetric()
    {
        typename AlternativeMetricType::Pointer altMetric = AlternativeMetricType::New();
        altMetric->SetFixedImageSamplesIntensityThreshold(1);
        altMetric->SetUseFixedImageSamplesIntensityThreshold(1);
        altMetric->SetUseAllPixels(false);
        altMetric->GetDebug();
        altMetric->SetComputeGradient(false);
        this->SetMetric(altMetric);
        std::cout<<"Using mean squared metric!!"<<std::endl;

    }
    void SetUseDefaultMetric()
    {
        typename MetricType::Pointer mmMetric = MetricType::New();


        //mmMetric->SetFixedImageSamplesIntensityThreshold(1);
        //mmMetric->SetUseFixedImageSamplesIntensityThreshold(1);
        mmMetric->SetNumberOfSpatialSamples(
                    m_MetricNumberOfSpatialSamples );
        mmMetric->SetNumberOfHistogramBins(10);

        this->SetMetric(mmMetric);
        std::cout<<"Using mattes mutual Inf. metric!!"<<std::endl;


        ///For Testing purposes only!!!
        /*typedef itk::ImageMaskSpatialObject<3> MaskType;
       itk::ImageFileReader< itk::Image<unsigned char,3> >::Pointer reader = itk::ImageFileReader< itk::Image<unsigned char,3> >::New();
       reader->SetFileName("SelectiveDM-mask.mhd");
       reader->Update();
       MaskType::Pointer mask = MaskType::New();
       mask->SetImage(reader->GetOutput());
       mask->Update();*/
        //mmMetric->SetMovingImageMask(mask);
    }

    itkSetMacro(MetricNumberOfSpatialSamples, unsigned int) ;
    itkGetConstMacro(MetricNumberOfSpatialSamples, unsigned int) ;

    itkSetObjectMacro(Observer, Command);

protected:
    RigidRegistrator() ;
    virtual ~RigidRegistrator() ;

    virtual void Initialize() throw(ExceptionObject);

    void PrintUncaughtError() ;

    void PrintError(ExceptionObject &e) ;

    itkSetObjectMacro(SecondaryOptimizer, OptimizerType);
    itkGetObjectMacro(SecondaryOptimizer, OptimizerType);


private:

    Command::Pointer        m_Observer;

    OptimizerMethodType     m_OptimizerMethod;
    unsigned int            m_OptimizerNumberOfIterations;
    ScalesType              m_OptimizerScales;

    unsigned int            m_MetricNumberOfSpatialSamples;
    bool                           m_MetricType;

    OptimizerPointer        m_SecondaryOptimizer;



} ; // end of class


#ifndef ITK_MANUAL_INSTANTIATION
#include "RigidRegistrator.txx"
#endif

} // end namespace itk

#endif //__RigidRegistrator_H

