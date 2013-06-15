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
    //TempPath=NULL;

    SigmaInf=1.0;
    SigmaFactor=0.99;

    CylinderRadius=2;
    CylinderLength=4;

    NumberOfHistogramBins=150;

    EMPointPercentage=50;
    DecimationFactor=0.4;

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
    if (!strcmp(attName, "useICP"))
    {
      this->SetUseICP(attValue);
      continue;
    }

  }

  this->WriteXML(std::cout,1);
}

//----------------------------------------------------------------------------
void vtkMRMLCLIFractureReconstructionPropertyNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  //of << indent << " referenceLabelmapNodeID=\"" << (this->ReferenceLabelmapNodeID ? this->ReferenceLabelmapNodeID : "NULL") << "\"";

}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLCLIFractureReconstructionPropertyNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLCLIFractureReconstructionPropertyNode *node = vtkMRMLCLIFractureReconstructionPropertyNode::SafeDownCast(anode);
  this->DisableModifiedEventOn();

  this->SetInputReferenceModelID(node->GetInputReferenceModelID());

  this->DisableModifiedEventOff();
  this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLCLIFractureReconstructionPropertyNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

}

// End
