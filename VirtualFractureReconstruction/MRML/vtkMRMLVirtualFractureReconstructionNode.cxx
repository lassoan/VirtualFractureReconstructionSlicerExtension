/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLVirtualFractureReconstructionNode.cxx,v $
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
#include "vtkMRMLVirtualFractureReconstructionNode.h"

// AnnotationModuleMRML includes
#include "vtkMRMLAnnotationROINode.h"

// STD includes
#include <string>

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLVirtualFractureReconstructionNode);

//----------------------------------------------------------------------------
vtkMRMLVirtualFractureReconstructionNode::vtkMRMLVirtualFractureReconstructionNode()
{
    this->HideFromEditorsOn();

    this->SaveWithSceneOff();
    this->SetName("ActiveFractureReconstructionNode");

    this->ReferenceLabelmapNodeID = NULL;
    this->FragmentLabelmapNodeID =NULL;
    this->FragmentImageNodeID = NULL;
    this->ReferenceImageNodeID = NULL;
    this->OutputLabelmapNodeID = NULL;
    this->OutputTransformNodeID=NULL;
    this->CurrentFragmentModelID=NULL;
    this->CurrentReferenceModelID=NULL;
    this->CurrentFragmentTransformNodeID=NULL;
    this->CurrentInitialTransformNodeID=NULL;
    this->CurrentModifiedReferenceModelID=NULL;
    this->ReferenceModelNodeID=NULL;
    this->ModelMakerNodeID=NULL;
    this->CLIReconstructionNodeID=NULL;
    this->CurrentModelNumber=0;

    this->CurrentFragmentModelHierarchyNodeID=NULL;
    this->RegistrationStep1HierarchyNodeID=NULL;
    this->TransformContainer = vtkSmartPointer<vtkCollection>::New();
    this->FragmentLabelmapsAndVolumes=std::map< std::string,ModelDataContainer >();
}

//----------------------------------------------------------------------------
std::string vtkMRMLVirtualFractureReconstructionNode::GetLabelmapIDForModel(std::string modelNodeID)
{
    if ( this->FragmentLabelmapsAndVolumes.find(modelNodeID) == this->FragmentLabelmapsAndVolumes.end() )
    {
        return "";
    }
    else
        return this->FragmentLabelmapsAndVolumes[modelNodeID].LabelmapID;
}

//----------------------------------------------------------------------------
std::string vtkMRMLVirtualFractureReconstructionNode::GetInitialTransformIDForModel(std::string modelNodeID)
{
    if ( this->FragmentLabelmapsAndVolumes.find(modelNodeID) == this->FragmentLabelmapsAndVolumes.end() )
    {
        return "Model ID did not change";
    }
    return this->FragmentLabelmapsAndVolumes[modelNodeID].InitialTransformID;
}

//----------------------------------------------------------------------------
std::string vtkMRMLVirtualFractureReconstructionNode::GetStep1TransformIDForModel(std::string modelNodeID)
{
    if ( this->FragmentLabelmapsAndVolumes.find(modelNodeID) == this->FragmentLabelmapsAndVolumes.end() )
    {
        return "";
    }
    else
        return this->FragmentLabelmapsAndVolumes[modelNodeID].Step1TransformID;
}
//----------------------------------------------------------------------------
std::string vtkMRMLVirtualFractureReconstructionNode::GetStep1InteractiveTransformIDForModel(std::string modelNodeID)
{
    if ( this->FragmentLabelmapsAndVolumes.find(modelNodeID) == this->FragmentLabelmapsAndVolumes.end() )
    {
        return "";
    }
    else
        return this->FragmentLabelmapsAndVolumes[modelNodeID].Step1InteractiveTransformID;
}
//----------------------------------------------------------------------------
std::string vtkMRMLVirtualFractureReconstructionNode::GetFinalTransformIDForModel(std::string modelNodeID)
{
    if ( this->FragmentLabelmapsAndVolumes.find(modelNodeID) == this->FragmentLabelmapsAndVolumes.end() )
    {
        return "";
    }
    else
        return this->FragmentLabelmapsAndVolumes[modelNodeID].FinalTransformID;
}

//----------------------------------------------------------------------------
std::string vtkMRMLVirtualFractureReconstructionNode::GetStep2TransformIDForModel(std::string modelNodeID)
{
    if ( this->FragmentLabelmapsAndVolumes.find(modelNodeID) == this->FragmentLabelmapsAndVolumes.end() )
    {
        return "";
    }
    else
        return this->FragmentLabelmapsAndVolumes[modelNodeID].Step2TransformID;
}

std::string vtkMRMLVirtualFractureReconstructionNode::GetCandidatePolyDataIDForModel(std::string modelNodeID)
{
    if ( this->FragmentLabelmapsAndVolumes.find(modelNodeID) == this->FragmentLabelmapsAndVolumes.end() )
    {
        return "";
    }
    else
        return this->FragmentLabelmapsAndVolumes[modelNodeID].CandidatePolyDataID;
}

std::string vtkMRMLVirtualFractureReconstructionNode::GetReferencePolyDataIDForModel(std::string modelNodeID)
{
    if ( this->FragmentLabelmapsAndVolumes.find(modelNodeID) == this->FragmentLabelmapsAndVolumes.end() )
    {
        return "";
    }
    else
        return this->FragmentLabelmapsAndVolumes[modelNodeID].ReferencePolyDataID;
}


std::string vtkMRMLVirtualFractureReconstructionNode::GetCandidatePolyDataFileNameForModel(std::string modelNodeID)
{
    if ( this->FragmentLabelmapsAndVolumes.find(modelNodeID) == this->FragmentLabelmapsAndVolumes.end() )
    {
        return "";
    }
    else
        return this->FragmentLabelmapsAndVolumes[modelNodeID].CandidatePolyDataFileName;
}

std::string vtkMRMLVirtualFractureReconstructionNode::GetReferencePolyDataFileNameForModel(std::string modelNodeID)
{
    if ( this->FragmentLabelmapsAndVolumes.find(modelNodeID) == this->FragmentLabelmapsAndVolumes.end() )
    {
        return "";
    }
    else
        return this->FragmentLabelmapsAndVolumes[modelNodeID].ReferencePolyDataFileName;
}
//----------------------------------------------------------------------------
vtkMRMLVirtualFractureReconstructionNode::~vtkMRMLVirtualFractureReconstructionNode()
{
    this->SetName("");
    if (ReferenceLabelmapNodeID)
    {
        this->SetReferenceLabelmapNodeID(NULL);
    }

    if (FragmentLabelmapNodeID)
    {
        this->SetFragmentLabelmapNodeID(NULL);
    }

    if (this->FragmentImageNodeID)
    {
        this->SetFragmentImageNodeID(NULL);
    }

    if (this->ReferenceImageNodeID)
    {
        this->SetReferenceImageNodeID(NULL);
    }

    if (this->OutputLabelmapNodeID)
    {
        this->SetOutputLabelmapNodeID(NULL);
    }

    if (OutputTransformNodeID)
    {
        this->SetOutputTransformNodeID(NULL);
    }

    if (this->CurrentFragmentModelID)
    {
        this->SetCurrentFragmentModelID(NULL);
    }

    if (this->CurrentReferenceModelID)
    {
        this->SetCurrentReferenceModelID(NULL);
    }
    if (this->ModelMakerNodeID)
    {
        this->SetModelMakerNodeID(NULL);
    }
    if (this->CurrentFragmentTransformNodeID)
    {
        this->SetCurrentFragmentTransformNodeID(NULL);
    }

    if (this->CurrentInitialTransformNodeID)
    {
        this->SetCurrentInitialTransformNodeID(NULL);
    }
    this->TransformContainer =NULL;
    this->FragmentLabelmapsAndVolumes.clear();
    this->ModelIDs.clear();
    this->LabelmapIDs.clear();

}


//----------------------------------------------------------------------------
void vtkMRMLVirtualFractureReconstructionNode::ReadXMLAttributes(const char** atts)
{
    std::cerr << "Reading VirtualFractureReconstructionNode node!" << std::endl;
    Superclass::ReadXMLAttributes(atts);

    const char* attName;
    const char* attValue;
    while (*atts != NULL)
    {
        attName = *(atts++);
        attValue = *(atts++);
        if (!strcmp(attName, "referenceLabelmapNodeID"))
        {
            this->SetReferenceLabelmapNodeID(attValue);
            continue;
        }
        if (!strcmp(attName, "fragmentLabelmapNodeID"))
        {
            this->SetFragmentLabelmapNodeID(attValue);
            continue;
        }
        if (!strcmp(attName, "fragmentImageNodeID"))
        {
            this->SetFragmentImageNodeID(attValue);
            continue;
        }
        if (!strcmp(attName, "referenceImageNodeID"))
        {
            this->SetReferenceImageNodeID(attValue);
            continue;
        }
        if (!strcmp(attName, "outputLabelmapNodeID"))
        {
            this->SetOutputLabelmapNodeID(attValue);
            continue;
        }

        if (!strcmp(attName, "outputTransformNodeID"))
        {
            this->SetOutputTransformNodeID(attValue);
            continue;
        }
    }

    this->WriteXML(std::cout,1);
}

//----------------------------------------------------------------------------
void vtkMRMLVirtualFractureReconstructionNode::WriteXML(ostream& os, int nIndent)
{
    Superclass::WriteXML(os, nIndent);

    vtkIndent indent(nIndent);

    os << indent << " ReferenceLabelmapNodeID=\"" << (this->ReferenceLabelmapNodeID ? this->ReferenceLabelmapNodeID : "NULL") << "\"";
    os << indent << " FragmentLabelmapNodeID=\"" << (this->FragmentLabelmapNodeID ? this->FragmentLabelmapNodeID : "NULL") << "\"";
    os << indent << " FragmentImageNodeID=\"" << (this->FragmentImageNodeID ? this->FragmentImageNodeID : "NULL") << "\"";
    os << indent << " ReferenceImageNodeID=\"" << (this->ReferenceImageNodeID ? this->ReferenceImageNodeID : "NULL") << "\"";
    os << indent << " CurrentReferenceModelID=\""<<(this->CurrentReferenceModelID ? this->CurrentReferenceModelID : "NULL") << "\"";
    os << indent << " FragmentLabelmapNodeID=\""<<(this->FragmentLabelmapNodeID ? this->FragmentLabelmapNodeID : "NULL") << "\"";
    os << indent << " CurrentFragmentModelID=\""<<(this->CurrentFragmentModelID ? this->CurrentFragmentModelID : "NULL") << "\"";
    os << indent << " FragmentImageNodeID=\""<<(this->FragmentImageNodeID ? this->FragmentImageNodeID : "NULL") << "\"";
    os << indent << " OutputLabelmapNodeID=\""<<(this->OutputLabelmapNodeID ? this->OutputLabelmapNodeID : "NULL") << "\"";
    os << indent << " OutputTransformNodeID=\""<<(this->OutputTransformNodeID ? this->OutputTransformNodeID : "NULL") << "\"";
    os << indent << " CurrentModifiedReferenceModelID=\""<<(this->CurrentModifiedReferenceModelID ? this->CurrentModifiedReferenceModelID: "NULL") << "\"";
    os << indent << " CurrentFragmentModelID=\""<<(this->CurrentFragmentModelID ? this->CurrentFragmentModelID : "NULL") << "\"";
    os << indent << " ModelMakerNodeID=\""<<(this->ModelMakerNodeID ? this->ModelMakerNodeID : "NULL") << "\"";
    os << indent << " CLIReconstructionNodeID=\""<<(this->CLIReconstructionNodeID ? this->CLIReconstructionNodeID : "NULL") << "\"";
    os << indent << " CurrentFragmentModelHierarchyNodeID=\""<<(this->CurrentFragmentModelHierarchyNodeID ? this->CurrentFragmentModelHierarchyNodeID : "NULL") << "\"";
    os << indent << " RegistrationStep1HierarchyNodeID=\""<<(this->RegistrationStep1HierarchyNodeID? this->RegistrationStep1HierarchyNodeID : "NULL") << "\"";
    os << indent << " CurrentFragmentTransformNodeID=\""<<(this->CurrentFragmentTransformNodeID ? this->CurrentFragmentTransformNodeID : "NULL") << "\"";
    os << indent << " CurrentInitialTransformNodeID=\""<<(this->CurrentInitialTransformNodeID ? this->CurrentInitialTransformNodeID : "NULL") << "\"";
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.

void vtkMRMLVirtualFractureReconstructionNode::Copy(vtkMRMLNode *anode)
{
    Superclass::Copy(anode);
    vtkMRMLVirtualFractureReconstructionNode *node = vtkMRMLVirtualFractureReconstructionNode::SafeDownCast(anode);
    this->DisableModifiedEventOn();

    this->SetReferenceImageNodeID(node->GetReferenceImageNodeID());
    this->SetReferenceLabelmapNodeID(node->GetReferenceLabelmapNodeID());
    this->SetCurrentReferenceModelID(node->GetCurrentReferenceModelID());
    this->SetFragmentLabelmapNodeID(node->GetFragmentLabelmapNodeID());
    this->SetCurrentFragmentModelID(node->GetCurrentFragmentModelID());
    this->SetFragmentImageNodeID(node->GetFragmentImageNodeID());
    this->SetOutputLabelmapNodeID(node->GetOutputLabelmapNodeID());
    this->SetOutputTransformNodeID(node->GetOutputTransformNodeID());
    this->SetCurrentModifiedReferenceModelID(node->GetCurrentModifiedReferenceModelID());
    this->SetCurrentFragmentModelID(node->GetCurrentFragmentModelID());
    this->SetModelMakerNodeID(node->GetModelMakerNodeID());
    this->SetCLIReconstructionNodeID(node->GetCLIReconstructionNodeID());
    this->SetCurrentFragmentModelHierarchyNodeID(node->GetCurrentFragmentModelHierarchyNodeID());
    this->SetRegistrationStep1HierarchyNodeID(node->GetRegistrationStep1HierarchyNodeID());
    this->SetCurrentFragmentTransformNodeID(node->GetCurrentFragmentTransformNodeID());
    this->SetCurrentInitialTransformNodeID(node->GetCurrentInitialTransformNodeID());
    this->SetCurrentModelNumber(node->GetCurrentModelNumber());
    this->SetModelMakerNodeID(node->GetModelMakerNodeID());

    this->DisableModifiedEventOff();
    this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLVirtualFractureReconstructionNode::PrintSelf(ostream& os, vtkIndent indent)
{
    Superclass::PrintSelf(os,indent);


    os << indent << " ReferenceLabelmapNodeID=\"" << (this->ReferenceLabelmapNodeID ? this->ReferenceLabelmapNodeID : "NULL") << "\"";
    os << indent << " FragmentLabelmapNodeID=\"" << (this->FragmentLabelmapNodeID ? this->FragmentLabelmapNodeID : "NULL") << "\"";
    os << indent << " FragmentImageNodeID=\"" << (this->FragmentImageNodeID ? this->FragmentImageNodeID : "NULL") << "\"";
    os << indent << " ReferenceImageNodeID=\"" << (this->ReferenceImageNodeID ? this->ReferenceImageNodeID : "NULL") << "\"";
    os << indent << " CurrentReferenceModelID=\""<<(this->CurrentReferenceModelID ? this->CurrentReferenceModelID : "NULL") << "\"";
    os << indent << " FragmentLabelmapNodeID=\""<<(this->FragmentLabelmapNodeID ? this->FragmentLabelmapNodeID : "NULL") << "\"";
    os << indent << " CurrentFragmentModelID=\""<<(this->CurrentFragmentModelID ? this->CurrentFragmentModelID : "NULL") << "\"";
    os << indent << " FragmentImageNodeID=\""<<(this->FragmentImageNodeID ? this->FragmentImageNodeID : "NULL") << "\"";
    os << indent << " OutputLabelmapNodeID=\""<<(this->OutputLabelmapNodeID ? this->OutputLabelmapNodeID : "NULL") << "\"";
    os << indent << " OutputTransformNodeID=\""<<(this->OutputTransformNodeID ? this->OutputTransformNodeID : "NULL") << "\"";
    os << indent << " CurrentModifiedReferenceModelID=\""<<(this->CurrentModifiedReferenceModelID ? this->CurrentModifiedReferenceModelID: "NULL") << "\"";
    os << indent << " CurrentFragmentModelID=\""<<(this->CurrentFragmentModelID ? this->CurrentFragmentModelID : "NULL") << "\"";
    os << indent << " ModelMakerNodeID=\""<<(this->ModelMakerNodeID ? this->ModelMakerNodeID : "NULL") << "\"";
    os << indent << " CLIReconstructionNodeID=\""<<(this->CLIReconstructionNodeID ? this->CLIReconstructionNodeID : "NULL") << "\"";
    os << indent << " CurrentFragmentModelHierarchyNodeID=\""<<(this->CurrentFragmentModelHierarchyNodeID ? this->CurrentFragmentModelHierarchyNodeID : "NULL") << "\"";
    os << indent << " RegistrationStep1HierarchyNodeID=\""<<(this->RegistrationStep1HierarchyNodeID? this->RegistrationStep1HierarchyNodeID : "NULL") << "\"";
    os << indent << " CurrentFragmentTransformNodeID=\""<<(this->CurrentFragmentTransformNodeID ? this->CurrentFragmentTransformNodeID : "NULL") << "\"";
    os << indent << " CurrentInitialTransformNodeID=\""<<(this->CurrentInitialTransformNodeID ? this->CurrentInitialTransformNodeID : "NULL") << "\"";

}

// End
