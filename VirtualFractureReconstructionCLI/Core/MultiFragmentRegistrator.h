#ifndef MULTIFRAGMENTREGISTRATOR_H
#define MULTIFRAGMENTREGISTRATOR_H

#include "vtkPolyData.h"
#include "ICPWrapper.h"
#include "vtkPoints.h"
#include "vtkWeakPointer.h"
#include "vtkSmartPointer.h"
#include "vtkPointData.h"
#include "vtkComputeLocalGeometry.h"
#include "ActivePointSelector.h"
#include "ibiaSurfacePointExtractor.h"
#include "ComputeMahalanobisDistance.h"
#include "ibiaMemIni.h"

#include "vtkTransformPolyDataFilter.h"

#include "itkScalableAffineTransform.h"

class MultiFragmentRegistrator
{
    public:

        typedef itk::ScalableAffineTransform<double, 3> ITKTransformType;
        typedef ITKTransformType::Pointer ITKTransformPointer;
        typedef ITKTransformType::MatrixType ITKMatrixType;
        typedef ITKTransformType::OffsetType ITKTransformOffsetType;
        MultiFragmentRegistrator();
        ~MultiFragmentRegistrator();
        void AddFragment(vtkPolyData* fragment);
        void AddFragment(vtkPolyData* fragment,ITKTransformPointer transform);

        void SetRelevantPointsArrayName(std::string name)
        {
            this->m_RelevantPointsArrayName=name;
        }

        void StartMultiFragmentAlignment();

        void SetIniFileName(std::string iniName)
        {
            this->m_DefaultIniName=iniName;
            this->m_Ini->SetFileName(this->m_DefaultIniName);
            this->m_Ini->LoadValues();
        }

        void Initialize();

        std::vector<ITKTransformPointer> GetFinalTransformContainer()
        {
            return this->m_FinalTransformContainer;
        }

    private:

        MemIni* m_Ini;

        std::string m_DefaultIniName;

        unsigned int m_NumberOfFragmentsInActiveSet,m_CurrentlyActiveFragment;
        float m_MaxPointDistanceToRef, m_MaxPointDistanceToExtCand,m_CrestCurvatureMaxVal;
        bool m_RefineCrest;

        std::vector<std::pair<vtkPolyData*,unsigned int> > m_FragmentContainer;
        vtkSmartPointer<vtkPoints> m_ActiveReferencePointSet;
        vtkSmartPointer<vtkPolyData> m_ActiveReferencePolyData;
        vtkSmartPointer<vtkPoints> m_ActiveCandidatePointSet;
        vtkSmartPointer<vtkPolyData> m_ActiveCandidatePolyData;
        vtkSmartPointer<vtkPolyData> m_CurrentCombinedEnsemble;

        vtkSmartPointer<vtkComputeLocalGeometry> m_GeometricPropCalculator;
        ibia::ICPWrapper* m_EMICP;

        float m_ClosestPointSearchRadius;
        ActivePointSelector* m_ClosestPointFinder;
        std::string m_RelevantPointsArrayName;

        vtkPoints* InitializeActiveReferencePointSet();
        vtkPoints* UpdateActiveReferencePointSet();
        vtkPolyData* IdentifyClosestInactiveFragment();
        vtkPoints* IdentifyCandidatePointsOnInactiveFragments();
        void TransformInactiveFragments();
        void ExtractActiveCandidatePoints();
        vtkSmartPointer<vtkPoints> ExtractClosestNActivePoints(vtkSmartPointer<vtkPolyData> points);
        registrationParameters GetEMICPParameters();
        void RefineCrestPointSelection(vtkSmartPointer<vtkPolyData> fragment);


        ITKTransformPointer m_ActiveTransform;
        std::vector<ITKTransformPointer> m_FinalTransformContainer;
};

#endif // MULTIFRAGMENTREGISTRATOR_H
