/*
 * ComputeMahalanobisDistance.h
 *
 *  Created on: 23.02.2012
 *      Author: kfritscher
 */

#ifndef COMPUTEMAHALANOBISDISTANCE_H_
#define COMPUTEMAHALANOBISDISTANCE_H_

#include "itkStatisticsAlgorithm.h"
#include "itkMahalanobisDistanceMetric.h"
#include "itkCovarianceSampleFilter.h"
#include "itkListSample.h"
#include "vtkDataArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkSmartPointer.h"
#include "vtkPointData.h"

namespace ibia
{

class ComputeMahalanobisDistance
{

    unsigned int   m_ReferenceVectorSize;
public:
    typedef itk::Array<  double >    MeasurementVectorType;
    typedef itk::Statistics::ListSample<  MeasurementVectorType >   SampleType;
    typedef itk::Statistics::CovarianceSampleFilter<SampleType> CovarianceCalculatorType;
    typedef itk::Statistics::MahalanobisDistanceMetric< MeasurementVectorType >   DistanceMetricType;

    double Evaluate(vtkPoints *ref, unsigned int index);
    void Evaluate(vtkSmartPointer<vtkPolyData> candidate, vtkDataArray* distances, bool useSurfaceClassArray=true,bool useActivePointsArray=true);
    void Evaluate(vtkPoints *candidate, vtkDataArray* distances);
//    void Evaluate(vtkPoints* candidate,vtkDataArray* distances,alglib::integer_1d_array indices, bool posValue);
//    void SetReference(vtkPoints* reference, alglib::integer_1d_array indices, bool posValue);
    void SetReference(vtkPoints * reference);
    void SetReference(vtkSmartPointer<vtkPolyData> reference, bool usePointCoordinates=true, bool useTexture=false, bool useCurvature=true, bool useNormals=true, bool useSurfaceClassifierArray=true);

    ComputeMahalanobisDistance();
    ~ComputeMahalanobisDistance();

    void SetUseTextureFeatures(bool useTexture)
    {
        this->m_UseTextureFeatures=useTexture;
    }

    void SetUseCurvature(bool useCurvature)
    {
        this->m_UseCurvature=useCurvature;
    }

    void SetUsePointCoordinates(bool usePointCoordinates)
    {
        this->m_UsePointCoordinates=usePointCoordinates;
    }

    void SetUseSurfaceNormals(bool useSurfaceNormals)
    {
        this->m_UseSurfaceNormals=useSurfaceNormals;
    }

private:
    CovarianceCalculatorType::Pointer m_CovarianceCalculator;
    DistanceMetricType::Pointer m_DistanceCalculator;
    SampleType::Pointer m_ReferenceData;

    bool m_UseCurvature, m_UseTextureFeatures,m_UseSurfaceNormals,m_UsePointCoordinates;


//    void CalculateMeanAndCovariance(vtkPoints *reference, alglib::integer_1d_array indices, bool posValue);
    void CalculateMeanAndCovariance(vtkPoints *reference);
    void CalculateMeanAndCovariance();


};

} /* namespace ibia */
#endif /* COMPUTEMAHALANOBISDISTANCE_H_ */
