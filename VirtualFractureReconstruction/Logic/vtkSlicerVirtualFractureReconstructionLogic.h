/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// .NAME vtkSlicerVirtualFractureReconstructionLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerVirtualFractureReconstructionLogic_h
#define __vtkSlicerVirtualFractureReconstructionLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes
#include "vtkMRMLModelHierarchyNode.h"
#include "vtkMRMLCLIFractureReconstructionPropertyNode.h"
#include "vtkMRMLCommandLineModuleNode.h"

//VTK includes
#include "vtkImageData.h"

// STD includes
#include <cstdlib>
#include "vtkSlicerVirtualFractureReconstructionModuleLogicExport.h"

// Qt includes
#include "qcolor.h"

class vtkMRMLVirtualFractureReconstructionNode;
class vtkSlicerCLIModuleLogic;
class vtkMRMLScalarVolumeNode;



/// \ingroup Slicer_QtModules_VirtualFractureReconstruction
class VTK_SLICER_VIRTUALFRACTURERECONSTRUCTION_MODULE_LOGIC_EXPORT vtkSlicerVirtualFractureReconstructionLogic :
  public vtkSlicerModuleLogic
{
public:
  
  static vtkSlicerVirtualFractureReconstructionLogic *New();
  vtkTypeMacro(vtkSlicerVirtualFractureReconstructionLogic,vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

   bool CreateModel(vtkMRMLVirtualFractureReconstructionNode *reconstructionNode,vtkMRMLScalarVolumeNode* volumeNode,vtkMRMLModelHierarchyNode* hierarchyNode=NULL,bool reference =false);

   bool StartRegistrationStep1(vtkMRMLVirtualFractureReconstructionNode *reconstructionNode);

   bool StartRegistrationStep2(vtkMRMLVirtualFractureReconstructionNode *reconstructionNode);

   void SetModelMakerLogic(vtkSlicerCLIModuleLogic* logic);

   void SetColorLogic(vtkMRMLColorLogic* logic);

   void CorrectPose(std::string incorrectModelID, std::string correctNodeID,std::string additionalTransformNodeID,bool invert);

   void SetCLIReconstructionLogic(vtkSlicerCLIModuleLogic* logic);
   void SetCLIReconstructionPropertyNode(vtkMRMLCLIFractureReconstructionPropertyNode* cliPropNode);

  double* GetColorFromIndex(int index) const;
  vtkMRMLCLIFractureReconstructionPropertyNode* GetCLIReconstructionPropertyNode();
  std::map<unsigned char, unsigned int> GetLabelIntensities(vtkSmartPointer<vtkImageData> image);
  std::vector<std::string > CreateScalarVolumeNodesFromMultiLabelNode(vtkSmartPointer<vtkMRMLScalarVolumeNode> node,std::map<unsigned char, unsigned int> labelColors,vtkSmartPointer<vtkMRMLScalarVolumeNode> referenceNode);

private:
  void UpdateStep1Parameters(vtkSmartPointer<vtkMRMLCommandLineModuleNode> reconstructionNodeCLI,vtkMRMLVirtualFractureReconstructionNode *reconstructionNode);

   vtkSetStringMacro(CurrentModelID);
   vtkGetStringMacro(CurrentModelID);

protected:
  vtkSlicerVirtualFractureReconstructionLogic();
  virtual ~vtkSlicerVirtualFractureReconstructionLogic();



  unsigned int GetLabelColor(vtkSmartPointer<vtkImageData> image);

  virtual void SetMRMLSceneInternal(vtkMRMLScene * newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

private:

  vtkSlicerVirtualFractureReconstructionLogic(const vtkSlicerVirtualFractureReconstructionLogic&); // Not implemented
  void operator=(const vtkSlicerVirtualFractureReconstructionLogic&);               // Not implemented

  class vtkInternal;
  vtkInternal* Internal;
  vtkSmartPointer<vtkMRMLCLIFractureReconstructionPropertyNode> CLIReconstructionPropertyNode;

  char* CurrentModelID;
};

#endif

