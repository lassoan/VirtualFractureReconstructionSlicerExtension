/*
 * EMICPWrapper.cpp
 *
 *  Created on: 22.02.2012
 */

#include "ICPWrapper.h"
#include "vtkDecimatePro.h"
#include "vtkMaskPoints.h"
#include "vtkIterativeClosestPointTransform.h"


namespace ibia
{

ICPWrapper::ICPWrapper()
{
	// Initializing Parameters for ICP registration
	m_ICPVTK = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();

	m_ResultMatrixITK.SetIdentity();

	m_FinalTransform=TransformType::New();

	m_NumPointsRef=0;
	m_NumPointsCand=0;

	m_Parameters.noviewer=0;
	m_Parameters.nostop=0;
	m_Parameters.maxIteration=100;
}

ICPWrapper::~ICPWrapper()
{
	// TODO Auto-generated destructor stub
}

void  ICPWrapper::doRegistration(vtkSmartPointer<vtkPolyData> reference,vtkSmartPointer<vtkPolyData> candidate,unsigned int percentRef,unsigned int percentCand)
{


	// Setup ICP transform
    vtkSmartPointer<vtkMaskPoints> maskPointsC = vtkSmartPointer<vtkMaskPoints>::New();
    maskPointsC->SetInput(candidate);
    maskPointsC->SetMaximumNumberOfPoints(candidate->GetNumberOfPoints());
    maskPointsC->GenerateVerticesOn();
    maskPointsC->Update();

    vtkSmartPointer<vtkMaskPoints> maskPointsR = vtkSmartPointer<vtkMaskPoints>::New();
    maskPointsR->SetInput(reference);
    maskPointsR->SetMaximumNumberOfPoints(reference->GetNumberOfPoints());
    maskPointsR->GenerateVerticesOn();
    maskPointsR->Update();

    /*vtkSmartPointer<vtkDecimatePro> decimateR = vtkSmartPointer<vtkDecimatePro>::New();
    decimateR->SetInput(maskPointsR->GetOutput());
    decimateR->SetTargetReduction(1);
    decimateR->PreserveTopologyOn();

    vtkSmartPointer<vtkDecimatePro> decimateC = vtkSmartPointer<vtkDecimatePro>::New();
    decimateC->SetInput(maskPointsC->GetOutput());
    decimateC->SetTargetReduction(1);
    decimateC->PreserveTopologyOn();*/

    m_ICPVTK ->SetSource(maskPointsC->GetOutput());
    m_ICPVTK ->SetTarget(maskPointsR->GetOutput());
	m_ICPVTK ->GetLandmarkTransform()->SetModeToRigidBody();
	m_ICPVTK ->SetMaximumNumberOfIterations(this->m_Parameters.maxIteration);
    //m_ICPVTK ->StartByMatchingCentroidsOn();
	m_ICPVTK ->Modified();
	m_ICPVTK->Update();

	int count=0;
    for(unsigned int m=0;m<3;m++)
    {
        for(unsigned int n=0;n<3;n++)
        {
            this->m_ResultMatrixITK[m][n]=this->m_ICPVTK->GetMatrix()->GetElement(m,n);
            count++;
        }
    }

    for(unsigned int o=0;o<3;o++)
        this->m_ResultOffsetITK[o]=this->m_ICPVTK->GetMatrix()->GetElement(o,3);
}


void ICPWrapper::doRegistration(vtkSmartPointer<vtkPoints> reference,vtkSmartPointer<vtkPoints> candidate,unsigned int percentRef,unsigned int percentCand)
{
    vtkSmartPointer<vtkPolyData> tempCandidatePoly= vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyData> tempReferencePoly= vtkSmartPointer<vtkPolyData>::New();
    tempCandidatePoly->SetPoints(candidate);
    tempReferencePoly->SetPoints(reference);
    this->doRegistration(tempReferencePoly,tempCandidatePoly,percentRef,percentCand);
}


ICPWrapper::TransformType::Pointer ICPWrapper::GetFinalTransform()
{

	this->m_FinalTransform->SetMatrix(this->m_ResultMatrixITK);
	this->m_FinalTransform->SetOffset(this->m_ResultOffsetITK);

	return this->m_FinalTransform;
}

}
