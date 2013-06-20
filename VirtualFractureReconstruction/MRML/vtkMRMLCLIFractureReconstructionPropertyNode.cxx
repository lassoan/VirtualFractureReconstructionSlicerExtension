/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLCLIFractureReconstructionPropertyNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/


// Slicer includes

// VTK includes
#include <vtkCommand.h>
#include <vtkObjectFactory.h>

// MRML includes
#include "vtkMRMLVolumeNode.h"


// FractureRecontructionMRML includes
#include "vtkMRMLCLIFractureReconstructionPropertyNode.h"

// AnnotationModuleMRML includes
#include "vtkMRMLAnnotationROINode.h"

// STD includes
#include <string>

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLCLIFractureReconstructionPropertyNode);

//----------------------------------------------------------------------------
vtkMRMLCLIFractureReconstructionPropertyNode::vtkMRMLCLIFractureReconstructionPropertyNode()
{
    this->SaveWithSceneOff();

    UseICP=false;
    UseCoordinates=false;
    UseTexture=false;
    UseNormals=true;
    UseCurvature=true;

    StartAdjLevel=1;
    NumberOfAdjLevels=1;
    MaxSmoothingIts=5;
    MaxDist=5;
    StepSize=2;

    UseReferenceModel=false;
    FineTuningOnly=false;

    InputReferenceModelID=NULL;
    OutputReferencePolyDataID=NULL;

    InputReferenceFileName=NULL;
    ModifiedCandidatePolyDataFileName=NULL;
    CommaSeparatedTransformIDs=NULL;
    CommaSeparatedInputTransformIDs=NULL;
    CommaSeparatedModelFileNames=NULL;

    ModifiedCandidatePolyDataID=NULL;
    OutputLabelMapNodeID=NULL;
    TempPath=NULL;

    SigmaInf=1.0;
    SigmaFactor=0.99;

    CylinderRadius=2;
    CylinderLength=4;

    NumberOfHistogramBins=150;

    EMPointPercentage=50;
    DecimationFactor=0.4;

    MaxICPIterations=101;

    MaxPointDistanceMulti=5;
    CrestCurvatureValueMulti=-0.1;

}


//----------------------------------------------------------------------------
vtkMRMLCLIFractureReconstructionPropertyNode::~vtkMRMLCLIFractureReconstructionPropertyNode()
{

}


//----------------------------------------------------------------------------
void vtkMRMLCLIFractureReconstructionPropertyNode::ReadXMLAttributes(const char** atts)
{
    std::cerr << "Reading CropVolume param node!" << std::endl;
    Superclass::ReadXMLAttributes(atts);

    const char* attName;
    const char* attValue;
    while (*atts != NULL)
    {
        attName = *(atts++);
        attValue = *(atts++);

        if (!strcmp(attName, "InputReferenceModelID"))
        {
            this->SetInputReferenceModelID(attValue);
            continue;
        }
        if (!strcmp(attName, "OutputReferencePolyDataID"))
        {
            this->SetOutputReferencePolyDataID(attValue);
            continue;
        }
        if (!strcmp(attName, "InputReferenceFileName"))
        {
            this->SetInputReferenceFileName(attValue);
            continue;
        }
        if (!strcmp(attName, "ModifiedCandidatePolyDataFileName"))
        {
            this->SetModifiedCandidatePolyDataFileName(attValue);
            continue;
        }
        if (!strcmp(attName, "ModifiedCandidatePolyDataID"))
        {
            this->SetModifiedCandidatePolyDataID(attValue);
            continue;
        }

        if (!strcmp(attName, "CommaSeparatedTransformIDs"))
        {
            this->SetCommaSeparatedTransformIDs(attValue);
            continue;
        }
        if (!strcmp(attName, "CommaSeparatedInputTransformIDs"))
        {
            this->SetCommaSeparatedInputTransformIDs(attValue);
            continue;
        }
        if (!strcmp(attName, "CommaSeparatedModelFileNames"))
        {
            this->SetCommaSeparatedModelFileNames(attValue);
            continue;
        }

        if (!strcmp(attName, "OutputLabelMapNodeID"))
        {
            this->SetOutputLabelMapNodeID(attValue);
            continue;
        }

        if (!strcmp(attName, "useICP"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->UseICP;
            continue;
        }

        if (!strcmp(attName, "SigmaInf"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->SigmaInf;
            continue;
        }
        if (!strcmp(attName, "SigmaFactor"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->SigmaFactor;
            continue;
        }
        if (!strcmp(attName, "NumberOfHistogramBins"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->NumberOfHistogramBins;
            continue;
        }
        if (!strcmp(attName, "EMPointPercentage"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->EMPointPercentage;
            continue;
        }
        if (!strcmp(attName, "DecimationFactor"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->DecimationFactor;
            continue;
        }
        if (!strcmp(attName, "CylinderLength"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->CylinderLength;
            continue;
        }
        if (!strcmp(attName, "CylinderRadius"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->CylinderRadius;
            continue;
        }

        if (!strcmp(attName, "UseCoordinates"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->UseCoordinates;
            continue;
        }
        if (!strcmp(attName, "UseTexture"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->UseTexture;
            continue;
        }
        if (!strcmp(attName, "UseNormals"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->UseNormals;
            continue;
        }
        if (!strcmp(attName, "UseCurvature"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->UseCurvature;
            continue;
        }
        if (!strcmp(attName, "StartAdjLevel"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->StartAdjLevel;
            continue;
        }
        if (!strcmp(attName, "NumberOfAdjLevels"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->NumberOfAdjLevels;
            continue;
        }
        if (!strcmp(attName, "MaxSmoothingIts"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->MaxSmoothingIts;
            continue;
        }

        if (!strcmp(attName, "MaxICPIterations"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->MaxICPIterations;
            continue;
        }

        if (!strcmp(attName, "MaxDist"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->MaxDist;
            continue;
        }
        if (!strcmp(attName, "StepSize"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->StepSize;
            continue;
        }

        if (!strcmp(attName, "MaxPointDistanceMulti"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->MaxPointDistanceMulti;
            continue;
        }

        if (!strcmp(attName, "CrestCurvatureValueMulti"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->CrestCurvatureValueMulti;
            continue;
        }

        if (!strcmp(attName, "UseReferenceModel"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->UseReferenceModel;
            continue;
        }

        if (!strcmp(attName, "FineTuningOnly"))
        {
            std::stringstream ss;
            ss << attValue;
            ss >> this->FineTuningOnly;
            continue;
        }

    }

    this->WriteXML(std::cout,1);
}

//----------------------------------------------------------------------------
void vtkMRMLCLIFractureReconstructionPropertyNode::WriteXML(ostream& os, int nIndent)
{
    Superclass::WriteXML(os, nIndent);

    vtkIndent indent(nIndent);

    os << indent << " UseICP=\"" << this->UseICP  << "\"";
    os << indent << " SigmaInf=\"" << this->SigmaInf  << "\"";
    os << indent << " SigmaFactor=\"" << this->SigmaFactor << "\"";
    os << indent << " CylinderRadius=\"" << this->CylinderRadius  << "\"";
    os << indent << " CylinderLength=\"" << this->CylinderLength  << "\"";
    os << indent << " NumberOfHistogramBins=\"" << this->NumberOfHistogramBins  << "\"";
    os << indent << " EMPointPercentage=\"" << this->EMPointPercentage  << "\"";
    os << indent << " UseCoordinates=\"" << this->UseCoordinates  << "\"";
    os << indent << " UseTexture=\"" << this->UseTexture  << "\"";
    os << indent << " UseNormals=\"" << this->UseNormals  << "\"";
    os << indent << " UseCurvature=\"" << this->UseCurvature  << "\"";
    os << indent << " StartAdjLevel=\"" << this->StartAdjLevel  << "\"";
    os << indent << " NumberOfAdjLevels=\"" << this->NumberOfAdjLevels  << "\"";
    os << indent << " MaxSmoothingIts=\"" << this->MaxSmoothingIts  << "\"";
    os << indent << " MaxDist=\"" << this->MaxDist  << "\"";
    os << indent << " StepSize=\"" << this->StepSize  << "\"";
    os << indent << " UseReferenceModel=\"" << this->UseReferenceModel  << "\"";
    os << indent << " FineTuningOnly=\"" << this->FineTuningOnly << "\"";
    os << indent << " InputReferenceModelID=\"" <<(this->InputReferenceModelID ? this->InputReferenceModelID : "NULL") << "\"";
    os << indent << " OutputReferencePolyDataID=\""<<(this->OutputReferencePolyDataID ? this->OutputReferencePolyDataID : "NULL") << "\"";
    os << indent << " InputReferenceFileName=\""<<(this->InputReferenceFileName ? this->InputReferenceFileName : "NULL") << "\"";
    os << indent << " ModifiedCandidatePolyDataFileName=\""<<(this->ModifiedCandidatePolyDataFileName ? this->ModifiedCandidatePolyDataFileName : "NULL") << "\"";
    os << indent << " CommaSeparatedTransformIDs=\""<<(this->CommaSeparatedTransformIDs ? this->CommaSeparatedTransformIDs : "NULL") << "\"";
    os << indent << " CommaSeparatedInputTransformIDs=\""<<(this->CommaSeparatedInputTransformIDs ? this->CommaSeparatedInputTransformIDs : "NULL") << "\"";
    os << indent << " CommaSeparatedModelFileNames=\""<<(this->CommaSeparatedModelFileNames ? this->CommaSeparatedModelFileNames : "NULL") << "\"";
    os << indent << " ModifiedCandidatePolyDataID=\""<<(this->ModifiedCandidatePolyDataID ? this->ModifiedCandidatePolyDataID : "NULL") << "\"";
    os << indent << " OutputLabelMapNodeID=\""<<(this->OutputLabelMapNodeID? this->OutputLabelMapNodeID : "NULL") << "\"";

}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.

void vtkMRMLCLIFractureReconstructionPropertyNode::Copy(vtkMRMLNode *anode)
{
    Superclass::Copy(anode);
    vtkMRMLCLIFractureReconstructionPropertyNode *node = vtkMRMLCLIFractureReconstructionPropertyNode::SafeDownCast(anode);
    this->DisableModifiedEventOn();

    this->SetOutputLabelMapNodeID(node->GetOutputLabelMapNodeID());
    this->SetModifiedCandidatePolyDataID(node->GetModifiedCandidatePolyDataID());
    this->SetCommaSeparatedModelFileNames(node->GetCommaSeparatedModelFileNames());
    this->SetCommaSeparatedInputTransformIDs(node->GetCommaSeparatedInputTransformIDs());
    this->SetCommaSeparatedTransformIDs(node->GetCommaSeparatedTransformIDs());
    this->SetModifiedCandidatePolyDataFileName(node->GetModifiedCandidatePolyDataFileName());
    this->SetInputReferenceFileName(node->GetInputReferenceFileName());
    this->SetOutputReferencePolyDataID(node->GetOutputReferencePolyDataID());
    this->SetInputReferenceModelID(node->GetInputReferenceModelID());
    this->SetFineTuningOnly(node->GetFineTuningOnly());
    this->SetUseReferenceModel(node->GetUseReferenceModel());
    this->SetStepSize(node->GetStepSize());
    this->SetMaxDist(node->GetMaxDist());
    this->SetMaxSmoothingIts(node->GetMaxSmoothingIts());
    this->SetNumberOfAdjLevels(node->GetNumberOfAdjLevels());
    this->SetStartAdjLevel(node->GetStartAdjLevel());
    this->SetUseCurvature(node->GetUseCurvature());
    this->SetUseNormals(node->GetUseNormals());
    this->SetUseTexture(node->GetUseTexture());
    this->SetUseCoordinates(node->GetUseCoordinates());
    this->SetEMPointPercentage(node->GetEMPointPercentage());
    this->SetNumberOfHistogramBins(node->GetNumberOfHistogramBins());
    this->SetCylinderLength(node->GetCylinderLength());
    this->SetCylinderRadius(node->GetCylinderRadius());
    this->SetSigmaFactor(node->GetSigmaFactor());
    this->SetSigmaInf(node->GetSigmaInf());
    this->SetMaxICPIterations(node->GetMaxICPIterations());
    this->SetMaxPointDistanceMulti(node->GetMaxPointDistanceMulti());

    this->SetCrestCurvatureValueMulti(node->GetCrestCurvatureValueMulti());
    this->DisableModifiedEventOff();
    this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLCLIFractureReconstructionPropertyNode::PrintSelf(ostream& os, vtkIndent indent)
{
    Superclass::PrintSelf(os,indent);


    os << indent << " UseICP=\"" << this->UseICP  << "\"";
    os << indent << " SigmaInf=\"" << this->SigmaInf  << "\"";
    os << indent << " SigmaFactor=\"" << this->SigmaFactor << "\"";
    os << indent << " CylinderRadius=\"" << this->CylinderRadius  << "\"";
    os << indent << " CylinderLength=\"" << this->CylinderLength  << "\"";
    os << indent << " NumberosHistogramBins=\"" << this->NumberOfHistogramBins  << "\"";
    os << indent << " EMPointPercentage=\"" << this->EMPointPercentage  << "\"";
    os << indent << " UseCoordinates=\"" << this->UseCoordinates  << "\"";
    os << indent << " UseTexture=\"" << this->UseTexture  << "\"";
    os << indent << " UseNormals=\"" << this->UseNormals  << "\"";
    os << indent << " UseCurvature=\"" << this->UseCurvature  << "\"";
    os << indent << " StartAdjLevel=\"" << this->StartAdjLevel  << "\"";
    os << indent << " NumberOfAdjLevels=\"" << this->NumberOfAdjLevels  << "\"";
    os << indent << " MaxSmoothingIts=\"" << this->MaxSmoothingIts  << "\"";
    os << indent << " MaxDist=\"" << this->MaxDist  << "\"";
    os << indent << " StepSize=\"" << this->StepSize  << "\"";
    os << indent << " UseReferenceModel=\"" << this->UseReferenceModel  << "\"";
    os << indent << " FineTuningOnly=\"" << this->FineTuningOnly << "\"";
    os << indent << " InputReferenceModelID=\"" <<(this->InputReferenceModelID ? this->InputReferenceModelID : "NULL") << "\"";
    os << indent << " OutputReferencePolyDataID=\""<<(this->OutputReferencePolyDataID ? this->OutputReferencePolyDataID : "NULL") << "\"";
    os << indent << " InputReferenceFileName=\""<<(this->InputReferenceFileName ? this->InputReferenceFileName : "NULL") << "\"";
    os << indent << " ModifiedCandidatePolyDataFileName=\""<<(this->ModifiedCandidatePolyDataFileName ? this->ModifiedCandidatePolyDataFileName : "NULL") << "\"";
    os << indent << " CommaSeparatedTransformIDs=\""<<(this->CommaSeparatedTransformIDs ? this->CommaSeparatedTransformIDs : "NULL") << "\"";
    os << indent << " CommaSeparatedInputTransformIDs=\""<<(this->CommaSeparatedInputTransformIDs ? this->CommaSeparatedInputTransformIDs : "NULL") << "\"";
    os << indent << " CommaSeparatedModelFileNames=\""<<(this->CommaSeparatedModelFileNames ? this->CommaSeparatedModelFileNames : "NULL") << "\"";
    os << indent << " ModifiedCandidatePolyDataID=\""<<(this->ModifiedCandidatePolyDataID ? this->ModifiedCandidatePolyDataID : "NULL") << "\"";
    os << indent << " OutputLabelMapNodeID=\""<<(this->OutputLabelMapNodeID? this->OutputLabelMapNodeID : "NULL") << "\"";


}

// End
