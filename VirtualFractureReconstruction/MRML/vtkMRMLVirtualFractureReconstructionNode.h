/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLFractureReconstructionNode.h,v $


=========================================================================auto=*/
// .NAME vtkMRMLFractureReconstructionNode
// .SECTION Description
// This node stores the information about fracture recosntruction module
//
//

#ifndef __vtkMRMLVirtualFractureReconstructionNode_h
#define __vtkMRMLVirtualFractureReconstructionNode_h

#include "qdebug.h"

#include "vtkMRML.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLColorLogic.h"

#include "vtkCollection.h"
#include "vtkSmartPointer.h"



#include "vtkSlicerVirtualFractureReconstructionModuleMRMLExport.h"

typedef struct
{
    std::string LabelmapID;
    std::string InitialTransformID;
    std::string Step1TransformID;
    std::string Step1InteractiveTransformID;
    std::string Step2TransformID;
    std::string FinalTransformID;
    std::string ReferencePolyDataID;
    std::string ReferencePolyDataFileName;
    std::string CandidatePolyDataID;
    std::string CandidatePolyDataFileName;
}ModelDataContainer;

/// \ingroup Slicer_QtModules_VirtualFractureReconstruction
class VTK_SLICER_VIRTUALFRACTURERECONSTRUCTION_MODULE_MRML_EXPORT vtkMRMLVirtualFractureReconstructionNode : public vtkMRMLNode
{

  public:   

  static vtkMRMLVirtualFractureReconstructionNode *New();
  vtkTypeMacro(vtkMRMLVirtualFractureReconstructionNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "Fracture reconstruction node";};

  // Description:
  vtkSetStringMacro(ReferenceLabelmapNodeID);
  vtkGetStringMacro (ReferenceLabelmapNodeID);

  vtkSetStringMacro (FragmentLabelmapNodeID);
  vtkGetStringMacro (FragmentLabelmapNodeID);


  vtkSetStringMacro(FragmentImageNodeID);
  vtkGetStringMacro (FragmentImageNodeID);

  vtkSetStringMacro(ReferenceImageNodeID);
  vtkGetStringMacro (ReferenceImageNodeID);

  vtkSetStringMacro(OutputLabelmapNodeID);
  vtkGetStringMacro (OutputLabelmapNodeID);

  vtkSetStringMacro(CurrentModifiedReferenceModelID);
  vtkGetStringMacro (CurrentModifiedReferenceModelID);

  vtkSetStringMacro(OutputTransformNodeID);
  vtkGetStringMacro (OutputTransformNodeID);

  vtkSetStringMacro (CurrentFragmentModelID);
  vtkGetStringMacro (CurrentFragmentModelID);

  vtkSetStringMacro (CurrentReferenceModelID);
  vtkGetStringMacro (CurrentReferenceModelID);

  vtkSetStringMacro(ModelMakerNodeID);
  vtkGetStringMacro(ModelMakerNodeID);

  vtkSetStringMacro(CLIReconstructionNodeID);
  vtkGetStringMacro(CLIReconstructionNodeID);

  vtkSetStringMacro(CurrentFragmentModelHierarchyNodeID);
  vtkGetStringMacro(CurrentFragmentModelHierarchyNodeID);

  vtkSetStringMacro(RegistrationStep1HierarchyNodeID);
  vtkGetStringMacro(RegistrationStep1HierarchyNodeID);

  vtkSetStringMacro(CurrentFragmentTransformNodeID);
  vtkGetStringMacro(CurrentFragmentTransformNodeID);

  vtkSetStringMacro(CurrentInitialTransformNodeID);
  vtkGetStringMacro(CurrentInitialTransformNodeID);

  vtkSetMacro(CurrentModelNumber,unsigned int);
  vtkGetMacro(CurrentModelNumber,unsigned int);


  vtkGetMacro(CurrentlyActiveModelTab,unsigned int);
  vtkSetMacro(CurrentlyActiveModelTab,unsigned int);

  /*vtkMRMLLinearTransformNode* GetCurrentTransformNode()
  {
      return MRMLFragmentTransformNode;
  }*/

  unsigned int GetNumberOfTransformations()
  {
      return TransformContainer.GetPointer()->GetNumberOfItems();
  }

  /* void SetCurrentTransformNode(vtkMRMLLinearTransformNode* tnode)
  {
      MRMLFragmentTransformNode=tnode;
  }*/


   void AddTransform(vtkMRMLTransformNode* transformNode)
   {
       TransformContainer.GetPointer()->AddItem(transformNode);
   }

   void CreateDataContainerForModel(std::string modelID)
   {
       ModelDataContainer newContainer;
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           this->FragmentLabelmapsAndVolumes.insert(std::pair<std::string,ModelDataContainer>(modelID,newContainer));
       }
       else
        this->FragmentLabelmapsAndVolumes[modelID]=newContainer;
        this->ModelIDs.push_back(modelID);
   }

   void AssignDataContainerToModel(std::string modelID,ModelDataContainer dataContainer)
   {
       this->FragmentLabelmapsAndVolumes[modelID]=dataContainer;
   }

   bool AddLabelmapIDToModel(std::string modelID, std::string labelmapID)
   {
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           return 0;
       }
       this->FragmentLabelmapsAndVolumes[modelID].LabelmapID=labelmapID;
       LabelmapIDs.push_back(labelmapID);
       return 1;
   }

   bool AddCandidatePolyDataIDToModel(std::string modelID, std::string cPolyID)
   {
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           return 0;
       }
       this->FragmentLabelmapsAndVolumes[modelID].CandidatePolyDataID=cPolyID;
       return 1;
   }

   bool AddReferencePolyDataIDToModel(std::string modelID, std::string rPolyID)
   {
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           return 0;
       }
       this->FragmentLabelmapsAndVolumes[modelID].ReferencePolyDataID=rPolyID;
       return 1;
   }

   bool AddReferencePolyDataFileNameToModel(std::string modelID, std::string rPolyFName)
   {
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           return 0;
       }
       this->FragmentLabelmapsAndVolumes[modelID].ReferencePolyDataFileName=rPolyFName;
       return 1;
   }

   bool AddCandidatePolyDataFileNameToModel(std::string modelID, std::string cPolyFName)
   {
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           return 0;
       }
       this->FragmentLabelmapsAndVolumes[modelID].CandidatePolyDataFileName=cPolyFName;
       return 1;
   }

   bool AddInitialTransformIDToModel(std::string modelID, std::string transformID)
   {
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           return 0;
       }
       this->FragmentLabelmapsAndVolumes[modelID].InitialTransformID=transformID;
       return 1;
   }

   bool AddStep1TransformIDToModel(std::string modelID, std::string transformID)
   {
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           return 0;
       }
       this->FragmentLabelmapsAndVolumes[modelID].Step1TransformID=transformID;
       return 1;
   }

   bool AddStep1InteractiveTransformIDToModel(std::string modelID, std::string transformID)
   {
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           return 0;
       }
       this->FragmentLabelmapsAndVolumes[modelID].Step1InteractiveTransformID=transformID;
       return 1;
   }

   bool AddStep2TransformIDToModel(std::string modelID, std::string transformID)
   {
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           return 0;
       }
       this->FragmentLabelmapsAndVolumes[modelID].Step2TransformID=transformID;
       return 1;
   }

   bool AddFinalTransformIDToModel(std::string modelID, std::string transformID)
   {
       if ( this->FragmentLabelmapsAndVolumes.find(modelID) == this->FragmentLabelmapsAndVolumes.end() )
       {
           return 0;
       }
       this->FragmentLabelmapsAndVolumes[modelID].FinalTransformID=transformID;
       return 1;
   }

   std::vector<std::string> GetModelIDs()
   {
       return ModelIDs;
   }

   std::vector<std::string> GetLabelmapIDs()
   {
       return LabelmapIDs;
   }

   std::string GetLabelmapIDForModel(std::string modelNodeID);
   std::string GetInitialTransformIDForModel(std::string modelNodeID);
   std::string GetStep1TransformIDForModel(std::string modelNodeID);
   std::string GetStep1InteractiveTransformIDForModel(std::string modelNodeID);
   std::string GetStep2TransformIDForModel(std::string modelNodeID);
   std::string GetFinalTransformIDForModel(std::string modelNodeID);
   std::string GetCandidatePolyDataIDForModel(std::string modelNodeID);
   std::string GetReferencePolyDataIDForModel(std::string modelNodeID);

   std::string GetCandidatePolyDataFileNameForModel(std::string modelNodeID);
   std::string GetReferencePolyDataFileNameForModel(std::string modelNodeID);


protected:
  vtkMRMLVirtualFractureReconstructionNode();
  ~vtkMRMLVirtualFractureReconstructionNode();

  vtkMRMLVirtualFractureReconstructionNode(const vtkMRMLVirtualFractureReconstructionNode&);
  void operator=(const vtkMRMLVirtualFractureReconstructionNode&);

  char *ReferenceLabelmapNodeID;
  char *ReferenceImageNodeID;
  char *FragmentLabelmapNodeID;
  char *FragmentImageNodeID;

  char *OutputLabelmapNodeID;
  char *CurrentFragmentModelID;
  char* OutputTransformNodeID;
  char *ReferenceModelNodeID;
  char *CurrentReferenceModelID;
  char *CurrentModifiedReferenceModelID;
  char* ModelMakerNodeID;
  char* CLIReconstructionNodeID;

  char* CurrentFragmentModelHierarchyNodeID;
  char* RegistrationStep1HierarchyNodeID;
  char* CurrentFragmentTransformNodeID;
  char* CurrentInitialTransformNodeID;
  unsigned int CurrentModelNumber,CurrentlyActiveModelTab;

  char* TempPath;

  //vtkMRMLLinearTransformNode*   MRMLFragmentTransformNode;
  //vtkMRMLLinearTransformNode*   MRMLInitialFragmentTransformNode;
  vtkSmartPointer<vtkCollection> TransformContainer;

private:

  std::map< std::string,ModelDataContainer > FragmentLabelmapsAndVolumes;
  std::vector<std::string> ModelIDs;
  std::vector<std::string> LabelmapIDs;

};

#endif

