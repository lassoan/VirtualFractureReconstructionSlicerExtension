/*
 * EMICPWrapper.h
 *
 *  Created on: 22.02.2012
 *      Author: kfritscher
 */

#ifndef EMICPWRAPPER_H_
#define EMICPWRAPPER_H_

#include "itkTransform.h"
#include "itkMatrix.h"
#include "vtkPolyData.h"

#include "itkScalableAffineTransform.h"
#include "itkPointSet.h"
#include "vtkPoints.h"
#include "itkDefaultStaticMeshTraits.h"
#include "vtkSmartPointer.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkLandmarkTransform.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"


typedef struct {

  // for EM-ICP - > currently not used
  float sigma_p2;      // initial value for the main loop. sigma_p2 <- sigma_p2 * sigma_factor  at the end of each iteration while sigma_p2 > sigam_inf. default: 0.01
  float sigma_inf;     // minimum value of sigma_p2. default: 0.00001
  float sigma_factor;  // facfor for reducing sigma_p2. default: 0.9
  float d_02;          // values for outlier (see EM-ICP paper). default: 0.01

  // for ICP
  int maxIterations; // Number of ICP iterations. default: 30

  // misc currently not used for slicer extension
  int noviewer; // No viewer is shown. Just align point sets, and quit.
  int nostop;   // No interatction by the viewer is required.
  int notimer;  // No timer is shown.

} registrationParameters;

namespace ibia
{

class ICPWrapper
{

  vtkSmartPointer<vtkIterativeClosestPointTransform> m_ICPVTK;

  int m_NumPointsRef;
  int m_NumPointsCand;

  registrationParameters m_Parameters;


public:

  typedef itk::ScalableAffineTransform<double,3> TransformType;
  typedef TransformType::MatrixType MatrixType;
  typedef TransformType::OffsetType OffsetType;

  ICPWrapper();

  void PointsReduction(float **X, int &Xsize,float random_sampling_percentage, bool initialize_rand = true);

  void doRegistration(vtkSmartPointer<vtkPolyData> reference,vtkSmartPointer<vtkPolyData> candidate,unsigned int percentRef=100,unsigned int percentCand=100);
  void doRegistration(vtkSmartPointer<vtkPoints> reference,vtkSmartPointer<vtkPoints> candidate,unsigned int percentRef,unsigned int percentCand);
  TransformType::Pointer GetFinalTransform();

  void SetDefaultRegistrationParameters(){
    m_Parameters.maxIterations=100;
  }

  void SetRegistrationParameters(registrationParameters para)
  {
    //m_Parameters.sigma_p2=para.sigma_p2;
    //m_Parameters.sigma_inf=para.sigma_inf;
    //m_Parameters.sigma_factor=para.sigma_factor;
    //m_Parameters.d_02=para.d_02;
    m_Parameters.maxIterations=para.maxIterations;
    //m_Parameters.noviewer=para.noviewer;
  }

  void SetViewer(bool viewer)
  {
      m_Parameters.noviewer=!viewer;
  }

  virtual
  ~ICPWrapper();

private:
  MatrixType m_ResultMatrixITK;
  OffsetType m_ResultOffsetITK;
  TransformType::Pointer m_FinalTransform;


};

} /* namespace ibia */
#endif /* EMICPWRAPPER_H_ */
