#ifndef __ImageRegistrationApp_h
#define __ImageRegistrationApp_h

#pragma once

#include "itkImage.h"

#include "MomentRegistrator.h"
#include "RigidRegistrator.h"

#include "itkAffineTransform.h"

#include "itkVersor.h"

// including all the type of transform that could possibly be used
// as templated type.

namespace itk
{

template< class TImage >
class ImageRegistrationApp : public Object
  {
  public:
    typedef ImageRegistrationApp Self;
    typedef Object Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(ImageRegistrationApp, Object);

    typedef TImage                        ImageType;
    typedef typename ImageType::Pointer   ImagePointer;
    typedef typename TImage::PixelType    ImagePixelType ;
    typedef typename TImage::RegionType   RegionType ;
    typedef typename TImage::OffsetType   OffsetType ;

    typedef MomentRegistrator< TImage >               NoneRegistratorType;
    typedef typename NoneRegistratorType::TransformType
                                                      NoneRegTransformType;

    typedef MomentRegistrator< TImage >               CenterRegistratorType;
    typedef typename CenterRegistratorType::TransformType
                                                      CenterRegTransformType;

    typedef MomentRegistrator< TImage >               MassRegistratorType;
    typedef typename MassRegistratorType::TransformType
                                                      MassRegTransformType;

    typedef MomentRegistrator< TImage >               MomentRegistratorType;
    typedef typename MomentRegistratorType::TransformType
                                                      MomentRegTransformType;

    typedef AffineTransform<double, 3>                LoadedRegTransformType;

    typedef RigidRegistrator< TImage >                    RigidRegistratorType;
    typedef typename RigidRegistratorType::ParametersType RigidParametersType;
    typedef typename RigidRegistratorType::ScalesType     RigidScalesType;
    typedef typename RigidRegistratorType::TransformType  RigidRegTransformType;

//    typedef AffineRegistrator< TImage >                 AffineRegistratorType;
//    typedef typename AffineRegistratorType::ParametersType
//                                                        AffineParametersType;
//    typedef typename AffineRegistratorType::ScalesType  AffineScalesType;
//    typedef typename AffineRegistratorType::TransformType
//                                                        AffineRegTransformType;

    typedef AffineTransform<double, 3>                  AffineTransformType;

    void SetOptimizerToOnePlusOne();

    void SetOptimizerToGradient();

    void SetOptimizerToOnePlusOnePlusGradient();

    void RegisterUsingNone();

    void RegisterUsingCenters();

    void RegisterUsingMass();

    void RegisterUsingMoments();

    void SetLoadedTransform(const LoadedRegTransformType & tfm);
    void CompositeLoadedTransform(const LoadedRegTransformType & tfm);
    void RegisterUsingLoadedTransform();


    void RegisterUsingRigid() ;

    void SetFixedImage(TImage* image);

    void SetMovingImage(TImage* image);

    void SetUseAlternativeMetricForRigid(bool use){m_UseAlternativeMetricForRigid=use;}

    itkGetObjectMacro(NoneRegTransform, NoneRegTransformType) ;
    itkGetObjectMacro(CenterRegTransform, CenterRegTransformType) ;
    itkGetObjectMacro(MassRegTransform, MassRegTransformType) ;
    itkGetObjectMacro(MomentRegTransform, MomentRegTransformType) ;

    itkGetObjectMacro(LoadedRegTransform, LoadedRegTransformType) ;
    itkGetObjectMacro(RigidRegTransform, RigidRegTransformType) ;

    itkGetObjectMacro(RigidAffineTransform, AffineTransformType) ;

    itkGetObjectMacro(FinalTransform, AffineTransformType);

    itkSetMacro(RigidNumberOfIterations, unsigned int) ;
    itkGetConstMacro(RigidNumberOfIterations, unsigned int) ;
    itkSetMacro(RigidFixedImageStandardDeviation, double) ;
    itkGetConstMacro(RigidFixedImageStandardDeviation, double) ;
    itkSetMacro(RigidMovingImageStandardDeviation, double) ;
    itkGetConstMacro(RigidMovingImageStandardDeviation, double) ;
    itkSetMacro(RigidNumberOfSpatialSamples, unsigned int) ;
    itkGetConstMacro(RigidNumberOfSpatialSamples, unsigned int) ;
    itkSetMacro(RigidScales, RigidScalesType) ;
    itkGetConstMacro(RigidScales, RigidScalesType) ;

    itkSetMacro(MovingImageRegion, RegionType) ;
    itkGetConstMacro(MovingImageRegion, RegionType) ;

    itkSetMacro(HighInterpolationQuality, bool) ;
    itkSetMacro(DefaultBackgroundPixelValue, ImagePixelType) ;

    ImagePointer GetNoneRegisteredMovingImage();
    ImagePointer GetCenterRegisteredMovingImage();
    ImagePointer GetMassRegisteredMovingImage();
    ImagePointer GetMomentRegisteredMovingImage();

    ImagePointer GetLoadedRegisteredMovingImage();
    ImagePointer GetRigidRegisteredMovingImage();

    ImagePointer GetFinalRegisteredMovingImage();

    itkGetMacro(RigidMetricValue, double);


  protected:
    ImageRegistrationApp() ;
    virtual ~ImageRegistrationApp() ;

    void InitRigidParameters(RigidParametersType & p,
                             Point<double, 3> & center);

    void PrintUncaughtError() ;

    void PrintError(ExceptionObject &e) ;

  private:
    typedef enum { NONE, CENTER, MASS, MOMENT, LOADED, RIGID}
                 PriorRegistrationMethodType;

    typedef enum { ONEPLUSONE,
                   GRADIENT,
                   ONEPLUSONEPLUSGRADIENT
                 } OptimizerMethodType;

    typename NoneRegTransformType::Pointer       m_NoneRegTransform ;
    typename AffineTransformType::Pointer        m_NoneAffineTransform ;
    typename CenterRegTransformType::Pointer     m_CenterRegTransform ;
    typename AffineTransformType::Pointer        m_CenterAffineTransform ;
    typename MassRegTransformType::Pointer       m_MassRegTransform ;
    typename AffineTransformType::Pointer        m_MassAffineTransform ;
    typename MomentRegTransformType::Pointer     m_MomentRegTransform ;
    typename AffineTransformType::Pointer        m_MomentAffineTransform ;
//    typename LandmarkRegTransformType::Pointer   m_LandmarkRegTransform ;
//    typename AffineTransformType::Pointer        m_LandmarkAffineTransform ;
    typename LoadedRegTransformType::Pointer     m_LoadedRegTransform ;
    typename AffineTransformType::Pointer        m_LoadedAffineTransform ;
    typename RigidRegTransformType::Pointer      m_RigidRegTransform ;
    typename AffineTransformType::Pointer        m_RigidAffineTransform ;
//    typename AffineRegTransformType::Pointer     m_AffineRegTransform ;
    typename AffineTransformType::Pointer        m_AffineAffineTransform ;
    typename AffineTransformType::Pointer        m_FinalTransform;

    double m_RigidMetricValue;
    double m_AffineMetricValue;

    TImage*             m_FixedImage ;
    TImage*             m_MovingImage ;

    RegionType          m_MovingImageRegion ;

    PriorRegistrationMethodType   m_PriorRegistrationMethod;

    OptimizerMethodType        m_OptimizerMethod;

    bool                m_NoneRegValid;

    bool                m_CenterRegValid;

    bool                m_MassRegValid;

    bool                m_MomentRegValid;
    bool m_UseAlternativeMetricForRigid;
//    bool m_UseAlternativeMetricForAffine;

//    unsigned int        m_LandmarkNumberOfIterations ;
//    LandmarkScalesType  m_LandmarkScales ;
//    bool                m_LandmarkRegValid;

    bool                m_LoadedRegValid;

    unsigned int        m_RigidNumberOfIterations ;
    double              m_RigidFixedImageStandardDeviation ;
    double              m_RigidMovingImageStandardDeviation ;
    unsigned int        m_RigidNumberOfSpatialSamples ;
    RigidScalesType     m_RigidScales ;
    bool                m_RigidRegValid;

    unsigned int        m_AffineNumberOfIterations ;
    double              m_AffineFixedImageStandardDeviation ;
    double              m_AffineMovingImageStandardDeviation ;
    unsigned int        m_AffineNumberOfSpatialSamples ;
//    AffineScalesType    m_AffineScales ;
//    bool                m_AffineRegValid;
    bool                m_HighInterpolationQuality;

    ImagePointer        ResampleUsingTransform(
                                        AffineTransformType * transform,
                                        ImageType * input,
                                        ImageType * output);
    ImagePixelType m_DefaultBackgroundPixelValue;
  }; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "ImageRegistrationApp.txx"
#endif

#endif //__ImageRegistrationApp_h

