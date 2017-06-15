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

#ifndef __qSlicerVirtualFractureReconstructionModuleWidget_h
#define __qSlicerVirtualFractureReconstructionModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qradiobutton.h"

#include "qSlicerVirtualFractureReconstructionModuleExport.h"

#include "qabstractitemmodel.h"

//VTK includes
#include "vtkSmartPointer.h"

class qSlicerVirtualFractureReconstructionModuleWidgetPrivate;

class vtkMRMLNode;
class vtkMRMLVirtualFractureReconstructionNode;
class vtkMRMLLabelMapVolumeNode;
class vtkMRMLLinearTransformNode;
class vtkMRMLTransformableNode;
class vtkMRMLModelHierarchyNode;
class vtkMRMLScalarVolumeNode;
class qMRMLTreeView;

/// \ingroup Slicer_QtModules_VirtualFractureReconstruction
class Q_SLICER_QTMODULES_VIRTUALFRACTURERECONSTRUCTION_EXPORT qSlicerVirtualFractureReconstructionModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerVirtualFractureReconstructionModuleWidget(QWidget *parent=0);
  virtual ~ qSlicerVirtualFractureReconstructionModuleWidget();

public slots:

protected slots:

  void createModels();
  void createModelsOfFragments(vtkMRMLModelHierarchyNode* hierarchyNode=NULL);
  void createModelOfReference(vtkMRMLModelHierarchyNode* hierarchyNode=NULL);
  void createModelForNode(vtkMRMLLabelMapVolumeNode* vnode, vtkMRMLModelHierarchyNode* hierarchyNode = NULL);
  //void initializeNode(vtkMRMLNode *n);
  void AssignTransformToSliders(vtkMRMLLinearTransformNode* transform);

  void updateWidget();
  void updateParameters();
  void updateCLIModuleParameters();
  void onReferenceLabelmapChanged();
  void onReferenceImageChanged();
  void onFragmentImageChanged();
  void onFragmentModelsSelectionChanged();
  void onStep1FragmentSelectionChanged();
  void onStartRegistrationStep1();
  void prepareIONodesForRegistrationStep1();
  void onStartRegistrationStep2();
  void onAlignmentToggleButtonPressed();
  void onParameterSettingsCollapsed();
  void onTransformSlidersCollapsed();

  void onReconstructionTabChanged();
  void onResetManualTransformButtonPressed();


protected:
  QScopedPointer<qSlicerVirtualFractureReconstructionModuleWidgetPrivate> d_ptr;

  void setMRMLScene(vtkMRMLScene* scene);
  void checkReconstructionNode(vtkMRMLScene* scene);
  void SetupTreeView(qMRMLTreeView* tree, QString hierarchyName="ModelHierarchy");
  virtual void setup();
  void enter();
  void WatcherUpdate();

  vtkMRMLTransformableNode* GetNodeFromIndex(const QModelIndex indices);

  vtkMRMLLinearTransformNode*  InitializeTransform(bool reference=false,std::string infix="",std::string modelID="");
  vtkMRMLLinearTransformNode*  InitializeTransform2(std::string infix,std::string modelID);
  vtkMRMLLinearTransformNode*  InitializeInteractiveTransform(vtkSmartPointer<vtkMRMLLinearTransformNode> copyTransform);
  void InitializeTreeView();

  void CreateTransformAndConnectToModelAndLabelmap(vtkMRMLNode* newModelNode,std::string labelmapID,bool reference=false);


private:
  Q_DECLARE_PRIVATE(qSlicerVirtualFractureReconstructionModuleWidget);
  Q_DISABLE_COPY(qSlicerVirtualFractureReconstructionModuleWidget);

  vtkMRMLVirtualFractureReconstructionNode* ReconstructionNode;

};

#endif
