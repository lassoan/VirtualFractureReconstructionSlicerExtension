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
// VirtualFractureReconstruction includes
#include "vtkSlicerVirtualFractureReconstructionLogic.h"

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerVirtualFractureReconstructionModuleWidget.h"
#include "ui_qSlicerVirtualFractureReconstructionModuleWidget.h"
#include "qSlicerCoreApplication.h"

// MRML includes
#include <vtkMRMLApplicationLogic.h>
#include "vtkMRMLVolumeNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLLabelMapVolumeNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLVirtualFractureReconstructionNode.h"
#include "qMRMLNodeFactory.h"
#include "vtkMRMLCLIFractureReconstructionPropertyNode.h"
#include "vtkMRMLModelStorageNode.h"
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLSubjectHierarchyNode.h"
#include "vtkMRMLTransformStorageNode.h"
#include "vtkMRMLColorNode.h"
#include "qMRMLSortFilterSubjectHierarchyProxyModel.h"


//VTK includes
#include"vtkMetaImageWriter.h"
#include "vtkMatrix4x4.h"
#include "vtkNew.h"
#include "vtkTrivialProducer.h"
#include "vtkImageData.h"

#include "itkTransformFileWriter.h"



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_VirtualFractureReconstruction
class qSlicerVirtualFractureReconstructionModuleWidgetPrivate : public Ui_qSlicerVirtualFractureReconstructionModule
{
  Q_DECLARE_PUBLIC(qSlicerVirtualFractureReconstructionModuleWidget);
protected:
  qSlicerVirtualFractureReconstructionModuleWidget* const q_ptr;

public:
  qSlicerVirtualFractureReconstructionModuleWidgetPrivate(qSlicerVirtualFractureReconstructionModuleWidget& object);
  ~qSlicerVirtualFractureReconstructionModuleWidgetPrivate();

  vtkSlicerVirtualFractureReconstructionLogic* logic() const;


};

//-----------------------------------------------------------------------------
// qSlicerVirtualFractureReconstructionModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerVirtualFractureReconstructionModuleWidgetPrivate::qSlicerVirtualFractureReconstructionModuleWidgetPrivate(qSlicerVirtualFractureReconstructionModuleWidget& object) : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
qSlicerVirtualFractureReconstructionModuleWidgetPrivate::~qSlicerVirtualFractureReconstructionModuleWidgetPrivate()
{
}

vtkSlicerVirtualFractureReconstructionLogic* qSlicerVirtualFractureReconstructionModuleWidgetPrivate::logic() const
{
  Q_Q(const qSlicerVirtualFractureReconstructionModuleWidget);
  return vtkSlicerVirtualFractureReconstructionLogic::SafeDownCast(q->logic());
}



//-----------------------------------------------------------------------------
// qSlicerVirtualFractureReconstructionModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerVirtualFractureReconstructionModuleWidget::qSlicerVirtualFractureReconstructionModuleWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVirtualFractureReconstructionModuleWidgetPrivate(*this))
{
}

qSlicerVirtualFractureReconstructionModuleWidget::~qSlicerVirtualFractureReconstructionModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
  this->Superclass::setMRMLScene(scene);
  if (scene == NULL)
  {
    return;
  }

  this->checkReconstructionNode(scene);

  this->updateWidget();

  /*
  this->SetupTreeView(d->FragmentModelsTree);
  this->SetupTreeView(d->ResultModelsTree1);
  this->SetupTreeView(d->ResultModelsTree2);
  */

  QStringList nodeTypesInput;

  //d->InputDataTree->setRootIndex(d->InputDataTree->sortFilterProxyModel()->mrmlSceneIndex());
  nodeTypesInput << QString("vtkMRMLScalarVolumeNode");

  d->InputDataTree->setNodeTypes(nodeTypesInput);
  d->FragmentImageComboBox->setNodeTypes(nodeTypesInput);
  d->ReferenceImageComboBox->setNodeTypes(nodeTypesInput);
  d->ReferenceLabelmapComboBox->setNodeTypes(nodeTypesInput);

  qDebug() << Q_FUNC_INFO << ": Fracture reconstruction MRML scene set";

}


void qSlicerVirtualFractureReconstructionModuleWidget::SetupTreeView(qMRMLSubjectHierarchyTreeView* tree, QString hierarchyName)
{
  /*
  tree->setSceneModelType(QString(hierarchyName));
  qobject_cast<qMRMLSceneModelHierarchyModel*>(
  tree->sceneModel())->setIDColumn(-1);
  qobject_cast<qMRMLSceneModelHierarchyModel*>(
  tree->sceneModel())->setColorColumn(1);
  qobject_cast<qMRMLSceneModelHierarchyModel*>(
  tree->sceneModel())->setOpacityColumn(2);
  qobject_cast<qMRMLSceneModelHierarchyModel*>(
  tree->sceneModel())->setColumnCount(3);

  tree->header()->setStretchLastSection(false);
  tree->header()->setResizeMode(0, QHeaderView::Stretch);
  tree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
  tree->header()->setResizeMode(2, QHeaderView::ResizeToContents);

  tree->sortFilterProxyModel()->setHideChildNodeTypes(
  QStringList() << "vtkMRMLFiberBundleNode" << "vtkMRMLAnnotationNode"<<"vtkMRMLScalarVolumeNode");
  //tree->sortFilterProxyModel()->setShowHiddenForTypes(
  //  QStringList() << "vtkMRMLModelHierarchyNode");

  tree->sortFilterProxyModel()->setShowChildNodeTypes(true);

  tree->setSelectionMode(QTreeView::ExtendedSelection);
  tree->setSelectionBehavior(QTreeView::SelectRows);
  */
}


void qSlicerVirtualFractureReconstructionModuleWidget::checkReconstructionNode(vtkMRMLScene* scene)
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
  this->ReconstructionNode = vtkMRMLVirtualFractureReconstructionNode::SafeDownCast(
    scene->GetFirstNodeByClass("vtkMRMLVirtualFractureReconstructionNode"));
  if (!this->ReconstructionNode)
  {
    qDebug() << Q_FUNC_INFO << ": Creating new fracture reconstruction node";
    this->ReconstructionNode = scene->AddNewNodeByClass("vtkMRMLVirtualFractureReconstructionNode");

    //Reseting input data tree, should rather be somewhere else
    //d->InputDataTree->reset();
    //d->InputDataTree->setRootIndex(d->InputDataTree->sortFilterProxyModel()->mrmlSceneIndex());

    QStringList nodeTypesInput;
    nodeTypesInput << QString("vtkMRMLScalarVolumeNode");
    d->InputDataTree->setNodeTypes(nodeTypesInput);
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::enter()
{
  //this->onInputVolumeChanged();
  this->Superclass::enter();
}


//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::updateWidget()
{
  if (!this->ReconstructionNode || !this->mrmlScene())
  {
    qWarning() << Q_FUNC_INFO << ": No reconstruction node or mrml scene. Aborting Widget update";
    return;
  }
}


void qSlicerVirtualFractureReconstructionModuleWidget::updateCLIModuleParameters()
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
  QString tempPath = qSlicerCoreApplication::application()->temporaryPath();
  qDebug() << Q_FUNC_INFO << ": SLICER TEMP-PATH LOADABLE: " << tempPath;
  d->logic()->GetCLIReconstructionPropertyNode()->SetUseCoordinates(d->UseCoordinatesCB->isChecked());
  d->logic()->GetCLIReconstructionPropertyNode()->SetUseCurvature(d->UseCurvatureCB->isChecked());
  d->logic()->GetCLIReconstructionPropertyNode()->SetUseNormals(d->UseNormalsCB->isChecked());
  d->logic()->GetCLIReconstructionPropertyNode()->SetUseTexture(d->UseTextureCB->isChecked());
  d->logic()->GetCLIReconstructionPropertyNode()->SetSigmaFactor(d->SigmaFactor->value());
  d->logic()->GetCLIReconstructionPropertyNode()->SetSigmaInf(d->SigmaInf->value());
  d->logic()->GetCLIReconstructionPropertyNode()->SetMaxICPIterations(d->MaxICPIterations->value());
  d->logic()->GetCLIReconstructionPropertyNode()->SetNumberOfHistogramBins(d->HistogramBins->value());
  d->logic()->GetCLIReconstructionPropertyNode()->SetDecimationFactor(d->PointDecimation->value());
  d->logic()->GetCLIReconstructionPropertyNode()->SetCylinderLength(d->CylinderAxisLength->value());
  d->logic()->GetCLIReconstructionPropertyNode()->SetCylinderRadius(d->CylinderRadius->value());
  d->logic()->GetCLIReconstructionPropertyNode()->SetMaxPointDistanceMulti(d->PointDistanceFT->value());
  d->logic()->GetCLIReconstructionPropertyNode()->SetCrestCurvatureValueMulti(d->CrestCurvatureFT->value());
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::updateParameters()
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
  if (!this->ReconstructionNode)
  { 
    return;
  }
  vtkMRMLNode* referenceLabelmapNode = d->ReferenceLabelmapComboBox->currentNode();
  vtkMRMLNode* referenceImageNode = d->ReferenceImageComboBox->currentNode();
  vtkMRMLNode* fragmentImageNode = d->FragmentImageComboBox->currentNode();
  this->ReconstructionNode->SetReferenceLabelmapNodeID(referenceLabelmapNode ? referenceLabelmapNode->GetID() : nullptr);
  this->ReconstructionNode->SetReferenceImageNodeID(referenceImageNode ? referenceImageNode->GetID() : nullptr);
  this->ReconstructionNode->SetFragmentImageNodeID(fragmentImageNode ? fragmentImageNode->GetID() :nullptr);
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::onReferenceLabelmapChanged()
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
  //make sure reconstruction node is present
  this->checkReconstructionNode(this->mrmlScene());
  if (!this->ReconstructionNode)
  {
    qDebug() << Q_FUNC_INFO << ": No reconstruction node present";
    return;
  }
  vtkMRMLNode* node = d->ReferenceLabelmapComboBox->currentNode();
  if (node)
  {
    this->ReconstructionNode->SetReferenceLabelmapNodeID(node->GetID());
    qDebug() << Q_FUNC_INFO << ": Setting ID of ReferenceLabelmap node" << this->ReconstructionNode->GetReferenceLabelmapNodeID();
    //d->InputDataTree->sortFilterProxyModel()->setHiddenNodeIDs(QStringList()<<d->InputDataTree->sortFilterProxyModel()->hiddenNodeIDs()<<node->GetID());
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::onReferenceImageChanged()
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
  //make sure reconstruction node is present
  this->checkReconstructionNode(this->mrmlScene());
  if (!this->ReconstructionNode)
  {
    qDebug() << Q_FUNC_INFO << ": No reconstruction node present";
    return;
  }
  vtkMRMLNode* node = d->ReferenceImageComboBox->currentNode();
  if (node)
  {
    qDebug() << Q_FUNC_INFO << ": Setting ID of ReferenceImage node" << node->GetID();
    this->ReconstructionNode->SetReferenceImageNodeID(node->GetID());
    //d->InputDataTree->sortFilterProxyModel()->setHiddenNodeIDs(QStringList()<<d->InputDataTree->sortFilterProxyModel()->hiddenNodeIDs()<<node->GetID());
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::onFragmentImageChanged()
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);

  //make sure reconstruction node is present
  this->checkReconstructionNode(this->mrmlScene());

  vtkMRMLNode* node = d->FragmentImageComboBox->currentNode();
  if (node)
  {
    qDebug() << Q_FUNC_INFO << ": Setting ID of FragmentImage node" << node->GetID();
    this->ReconstructionNode->SetFragmentImageNodeID(node->GetID());
    //d->InputDataTree->sortFilterProxyModel()->setHiddenNodeIDs(QStringList()<<d->InputDataTree->sortFilterProxyModel()->hiddenNodeIDs()<<node->GetID());
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::onFragmentModelsSelectionChanged()
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);

  this->checkReconstructionNode(this->mrmlScene());

  vtkMRMLNode* node = d->FragmentModelsTree->currentNode();

  if (node)
  {
    qDebug() << Q_FUNC_INFO << ": Fragment selection changed to " << node->GetID() << "of class" << node->GetClassName();
  }
  else
  {
    qDebug() << Q_FUNC_INFO << ": No valid node selected ";
    return;
  }

  vtkMRMLLinearTransformNode* tnode;

  /*std::string className=node->GetClassName();

  qDebug ()<< "Comparing class name"<<className.c_str();
  if(className.compare("vtkMRMLLinearTransformNode")==0)
  {
      qDebug()<<"Setting transform "<<vtkMRMLTransformNode::SafeDownCast(node)->GetID()<<"for sliders";
      tnode=vtkMRMLLinearTransformNode::SafeDownCast(node);
  }
  else if (className.compare("vtkMRMLModelNode")==0)
  {*/
  vtkMRMLModelNode* mnode = vtkMRMLModelNode::SafeDownCast(node);
  qDebug() << Q_FUNC_INFO << ": Using model node.Looking for initialTransformID for " << mnode->GetID();
  std::string tnodeID = this->ReconstructionNode->GetInitialTransformIDForModel(mnode->GetID());

  if (tnodeID.compare("") == 0)
  {
    qDebug() << Q_FUNC_INFO << ": No transform found for model" << mnode->GetID();
    return;
  }
  tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(tnodeID));
  this->AssignTransformToSliders(tnode);
  qDebug() << Q_FUNC_INFO << ": Setting transform " << tnode->GetID() << "for sliders";
  //}
}

//TODO Create only ONE onFragmentSelectionChangedForAllTrees
//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::onStep1FragmentSelectionChanged()
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
  if (!this->ReconstructionNode)
  {
    qDebug() << Q_FUNC_INFO << ": No reconstruction node present";
    return;
  }
  vtkMRMLNode* node = d->ResultModelsTree1->currentNode();

  if (node)
  {
    qDebug() << Q_FUNC_INFO << ": Fragment selection changed to " << node->GetID() << "of class" << node->GetClassName();
  }
  std::string className = node->GetClassName();
  vtkMRMLLinearTransformNode* tnode;
  if (className.compare("vtkMRMLLinearTransformNode") == 0)
  {
    qDebug() << Q_FUNC_INFO << ": Setting transform " << vtkMRMLTransformNode::SafeDownCast(node)->GetID() << "for sliders";
    tnode = vtkMRMLLinearTransformNode::SafeDownCast(node);
  }
  else if (className.compare("vtkMRMLModelNode") == 0)
  {
    vtkMRMLModelNode* mnode = vtkMRMLModelNode::SafeDownCast(node);
    std::string tnodeID = this->ReconstructionNode->GetStep1InteractiveTransformIDForModel(mnode->GetID());
    if (tnodeID.compare("") == 0)
    {
      qDebug() << Q_FUNC_INFO << ": No transform found for model" << mnode->GetID();
      return;
    }
    tnode = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(tnodeID));
    this->AssignTransformToSliders(tnode);
    qDebug() << Q_FUNC_INFO << ": Setting transform " << tnode->GetID() << "for sliders";
  }
}

//-----------------------------------------------------------------------------
vtkMRMLLinearTransformNode* qSlicerVirtualFractureReconstructionModuleWidget::InitializeTransform(bool reference, std::string infix, std::string modelID)
{
  std::string name = "TransformReferenceBone";
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::New();
  //unsigned int tnum=this->ReconstructionNode->GetNumberOfTransformations()-1;
  if (!reference)
  {
    std::ostringstream sstream;
    sstream << "Transform" << infix << "For_" << this->mrmlScene()->GetNodeByID(modelID)->GetName();
    name = sstream.str();
  }
  //TODO delete existing (transform/model) nodes when creating new reference transform
  transformNode->SetName(name.c_str());
  vtkSmartPointer<vtkMatrix4x4> transform = vtkSmartPointer<vtkMatrix4x4>::New();
  transform->Identity();
  transformNode->ApplyTransformMatrix(transform);

  //qDebug() << Q_FUNC_INFO << ": Transformation created.";

  this->ReconstructionNode->AddTransform(transformNode);
  this->mrmlScene()->AddNode(transformNode);
  if (!reference)
    this->ReconstructionNode->SetCurrentFragmentTransformNodeID(transformNode->GetID());

  //qDebug() << Q_FUNC_INFO << ": TransformNode created";
  return transformNode;
}

vtkMRMLLinearTransformNode* qSlicerVirtualFractureReconstructionModuleWidget::InitializeTransform2(std::string infix, std::string modelID)
{
  vtkSmartPointer<vtkMRMLLinearTransformNode> transformStep2Node = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();

  std::string name;
  std::ostringstream sstream;
  sstream << "Transform" << infix << "For_" << this->mrmlScene()->GetNodeByID(modelID)->GetName();
  name = sstream.str();

  vtkSmartPointer<vtkMRMLModelNode> modelNode = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelID));
  vtkSmartPointer<vtkMRMLLinearTransformNode> transformStep1Node = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelNode->GetTransformNodeID()));
  vtkSmartPointer<vtkMatrix4x4> newTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  newTransformMatrix->DeepCopy(transformStep1Node->GetMatrixTransformToParent());
  transformStep2Node->ApplyTransformMatrix(newTransformMatrix);
  transformStep2Node->SetName(name.c_str());

  //qDebug() << Q_FUNC_INFO << ": Transformation created.";

  this->ReconstructionNode->AddTransform(transformStep2Node);
  this->mrmlScene()->AddNode(transformStep2Node);
  this->ReconstructionNode->SetCurrentFragmentTransformNodeID(transformStep2Node->GetID());

  //qDebug() << Q_FUNC_INFO << ": TransformNode created";
  return transformStep2Node;
}

vtkMRMLLinearTransformNode* qSlicerVirtualFractureReconstructionModuleWidget::InitializeInteractiveTransform(vtkSmartPointer<vtkMRMLLinearTransformNode> copyTransform)
{
  char transformName[100];
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::New();

  if (copyTransform)
  {
    transformNode->GetMatrixTransformToParent()->Identity();
    transformNode->ApplyTransformMatrix(copyTransform->GetMatrixTransformToParent());
  }
  sprintf(transformName, "%s%s", copyTransform->GetName(), "_interact");
  transformNode->SetName(transformName);
  //TODO Remove/Replace AddTransform??
  this->ReconstructionNode->AddTransform(transformNode);
  this->mrmlScene()->AddNode(transformNode);
  this->ReconstructionNode->SetCurrentFragmentTransformNodeID(transformNode->GetID());

  qDebug() << Q_FUNC_INFO << ": InteractiveTransformNode created";
  return transformNode;
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::InitializeTreeView()
{
  /*
 // Listen for Transform node changes
 this->qvtkReconnect(d->MRMLFragmentTransformNode, transformNode,
   vtkMRMLTransformableNode::TransformModifiedEvent,
   this, SLOT(onMRMLTransformNodeModified(vtkObject*)));
 */

}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::setup()
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  //connect(d->ReferenceLabelmapComboBox->nodeFactory(), SIGNAL(nodeInitialized(vtkMRMLNode*)),
   //       this, SLOT(initializeNode(vtkMRMLNode*)));

  connect(d->ReferenceLabelmapComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(onReferenceLabelmapChanged()));

  connect(d->ReferenceImageComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(onReferenceImageChanged()));

  connect(d->FragmentImageComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(onFragmentImageChanged()));

  connect(d->FragmentModelsTree, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(onFragmentModelsSelectionChanged()));

  connect(d->ResultModelsTree1, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this, SLOT(onStep1FragmentSelectionChanged()));

  connect(d->AddFragmentButton, SIGNAL(clicked()),
    this, SLOT(createModels()));

  connect(d->StartButton1, SIGNAL(clicked()),
    this, SLOT(onStartRegistrationStep1()));

  connect(d->StartButton2, SIGNAL(clicked()),
    this, SLOT(onStartRegistrationStep2()));

  connect(d->ReconstructionTab, SIGNAL(currentChanged(int)),
    this, SLOT(onReconstructionTabChanged()));

  connect(d->ResetManualTransformButton, SIGNAL(pressed()),
    this, SLOT(onResetManualTransformButtonPressed()));

  connect(d->AlignmentToggleButton, SIGNAL(pressed()),
    this, SLOT(onAlignmentToggleButtonPressed()));


  connect(d->AlignmentToggleButton2, SIGNAL(pressed()),
    this, SLOT(onAlignmentToggleButtonPressed()));

  //connect(d->ParameterSettings,SIGNAL(toggled()),
  //        this,SLOT(onParameterSettingsCollapsed()));

  //connect(d->TransformSliders,SIGNAL(toggled()),
  //        this,SLOT(onTransformSlidersCollapsed()));

  /*connect(d->RegistrationStep1Tab, SIGNAL(currentChanged(int)),
          this, SLOT(onStep1TabOpened(int)  ));*/

          //Not in use yet
  d->PresetBox->addItem("Standard scenario");
  /*d->PresetBox->addItem("Proximal femur");
  d->PresetBox->addItem("Proximal tibia");
  d->PresetBox->addItem("Distal humerus");
  d->PresetBox->addItem("Distal femur");
  d->PresetBox->addItem("Distal tibia");*/

  d->InputDescription->setPlainText("Please choose preset if appropriate. Moreover, provide the following input data: \n\n -3D volume of non fractured bone (='Reference image') \n -3D labelmap of reference bone (='Reference labelmap') \n -3D volume containing fractured bone (= 'Fragment image')\n - one labelmap for each fragment (='Fragment labelmaps')\n\nPress 'Initialize reconstruction' -button to create 3D models of the labelmaps and initialize the reconstruction process ");
  d->InputDescription->setReadOnly(true);

  d->Step1Description->setPlainText("Press 'Start fragment alignment'-button to start reconstruction process: \n\nTo set parameters for fracture reconstruction, please use sliders in parameter settings section.\nFor manual pre-alignment select one fragment and use Transform sliders (Rotation and Translation sliders)\n\n ");
  d->Step1Description->setReadOnly(true);

  d->Step2Description->setPlainText("If you are satisfied with the alignment, press 'Accept registration'-button. Otherwise you can: \n\nPerform manual corrections by clicking on the misaligned fragment in the ReconstructionHierarchy and  using the Transform sliders. \nAlternatively you can go back to the previous tab and restart the alignment process after altering the parameters for fragment alignment and/or using manual pre-alignment.\n\nIf only minor corrections are necessary press the 'Start fine tuning'-button for automatic 'fine tuning' of the reconstruction ");
  d->Step2Description->setReadOnly(true);

  d->TransformSliders->setCollapsed(false);
  d->FragmentModelsTree->sortByColumn(5);
}

void qSlicerVirtualFractureReconstructionModuleWidget::onParameterSettingsCollapsed()
{
  //Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
  //d->TransformSliders->setCollapsed(true);
}

void qSlicerVirtualFractureReconstructionModuleWidget::onTransformSlidersCollapsed()
{
  //Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
   //d->ParameterSettings->setCollapsed(true);
}


//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::CreateTransformAndConnectToModelAndLabelmap(vtkMRMLNode* newModelNode, std::string labelmapID, bool reference)
{

  //Creating initial transform
  vtkSmartPointer<vtkMRMLLinearTransformNode> transform;
  transform.TakeReference(this->InitializeTransform(reference, "_ini_", newModelNode->GetID()));
  char* transformID = transform->GetID();

  vtkMRMLTransformableNode* modelNode = vtkMRMLTransformableNode::SafeDownCast(newModelNode);
  vtkMRMLScalarVolumeNode* labelmapNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(labelmapID.c_str()));
  if (!modelNode)
  {
    qDebug() << Q_FUNC_INFO << ": Model with name " << newModelNode->GetID() << " does not exist. Nothing connected to Transform";
    return;
  }
  std::string name = "ModelForBoneReference(flipped)";

  if (!reference)
  {
    std::ostringstream sstream;
    sstream << "ModelFor" << labelmapNode->GetName();
    name = sstream.str();
  }


  modelNode->SetName(name.c_str());

  if (!modelNode->GetTransformNodeID() || modelNode->GetTransformNodeID() != transformID)
    modelNode->SetAndObserveTransformNodeID(transformID);

  std::string modelID = modelNode->GetID();
  if (labelmapNode)
  {
    this->ReconstructionNode->AddLabelmapIDToModel(modelID, labelmapID);
    if (!labelmapNode->GetTransformNodeID() || labelmapNode->GetTransformNodeID() != transformID)
      labelmapNode->SetAndObserveTransformNodeID(transformID);
  }
  else
    qDebug() << Q_FUNC_INFO << ": labelmap node" << labelmapID.c_str() << " not found!";


  //qDebug()<<"Creating new data container for model and adding transform"<<modelID.c_str();
  this->ReconstructionNode->AddInitialTransformIDToModel(modelID, transformID);
  ///Let labelmap observe transform


  this->AssignTransformToSliders(transform);
}

void qSlicerVirtualFractureReconstructionModuleWidget::AssignTransformToSliders(vtkMRMLLinearTransformNode* transform)
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);
  d->MRMLRotationSliders->setMRMLTransformNode(transform);
  d->MRMLRotationSliders->activateWindow();
  d->MRMLRotationSliders->update();

  d->MRMLTranslationSliders->setMRMLTransformNode(transform);
  d->MRMLTranslationSliders->activateWindow();
  d->MRMLTranslationSliders->update();
}


//-----------------------------------------------------------------------------
/*
vtkMRMLTransformableNode* qSlicerVirtualFractureReconstructionModuleWidget::GetNodeFromIndex(const QModelIndex indices)
{
    Q_D(const qSlicerVirtualFractureReconstructionModuleWidget);
    vtkMRMLTransformableNode* node = vtkMRMLTransformableNode::SafeDownCast(
    d->InputDataTree->sortFilterProxyModel()->mrmlNodeFromIndex( indices ));
    return node;
}
*/

void qSlicerVirtualFractureReconstructionModuleWidget::onResetManualTransformButtonPressed()
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);

  std::vector<std::string> modelIDs = this->ReconstructionNode->GetModelIDs();
  std::string transformNodeID = "";
  std::string interactiveTransformNodeID = "";
  std::string finalTransformNodeID = "";
  QModelIndexList selectedIndices;
  selectedIndices = d->FragmentModelsTree->selectionModel()->selectedRows();
  unsigned int i = 0;
  int tabIndex = d->ReconstructionTab->currentIndex();
  //Reset Transform depending on currently selected tab
  if (tabIndex == 3)
  {
    foreach(QModelIndex selectedIndex, selectedIndices)
    {
      i = selectedIndex.row();

      qDebug() << Q_FUNC_INFO << ": Seeking transform for model" << modelIDs[i].c_str();
      transformNodeID = this->ReconstructionNode->GetStep2TransformIDForModel(modelIDs[i]);
      finalTransformNodeID = this->ReconstructionNode->GetFinalTransformIDForModel(modelIDs[i]);
      if (transformNodeID == "" || finalTransformNodeID == "")
      {
        qDebug() << Q_FUNC_INFO << ": No transform found for " << modelIDs[i].c_str();
      }
      else
      {
        //reset step1InteractiveTransform to step1Transform
        qDebug() << Q_FUNC_INFO << ": Resetting transform" << finalTransformNodeID.c_str() << "for model" << modelIDs[i].c_str();
        vtkSmartPointer<vtkMRMLLinearTransformNode> step2Transform = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(transformNodeID));
        vtkSmartPointer<vtkMRMLLinearTransformNode> finalTrans = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(interactiveTransformNodeID));
        std::string finalName = step2Transform->GetName();
        step2Transform->Copy(finalTrans);
        step2Transform->SetName(finalName.c_str());
      }

    }
  }
  else if (tabIndex == 2)
  {
    foreach(QModelIndex selectedIndex, selectedIndices)
    {
      i = selectedIndex.row();

      qDebug() << Q_FUNC_INFO << ": Seeking transform for model" << modelIDs[i].c_str();
      transformNodeID = this->ReconstructionNode->GetStep1TransformIDForModel(modelIDs[i]);
      interactiveTransformNodeID = this->ReconstructionNode->GetStep1InteractiveTransformIDForModel(modelIDs[i]);
      if (transformNodeID == "" || interactiveTransformNodeID == "")
      {
        qDebug() << Q_FUNC_INFO << ": No transform found for " << modelIDs[i].c_str();
      }
      else
      {
        //reset step1InteractiveTransform to step1Transform
        qDebug() << Q_FUNC_INFO << ": Resetting transform" << interactiveTransformNodeID.c_str() << "for model" << modelIDs[i].c_str();
        vtkSmartPointer<vtkMRMLLinearTransformNode> step1Transform = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(transformNodeID));
        vtkSmartPointer<vtkMRMLLinearTransformNode> currentTrans = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(interactiveTransformNodeID));
        std::string currentName = currentTrans->GetName();
        currentTrans->Copy(step1Transform);
        currentTrans->SetName(currentName.c_str());
      }

    }
  }
  else if (tabIndex == 1)
  {
    QModelIndexList selectedIndices;
    selectedIndices = d->FragmentModelsTree->selectionModel()->selectedRows();
    foreach(QModelIndex selectedIndex, selectedIndices)
    {
      i = selectedIndex.row();
      transformNodeID = this->ReconstructionNode->GetInitialTransformIDForModel(modelIDs[i]);
      vtkSmartPointer<vtkMRMLLinearTransformNode> iniTransform = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(transformNodeID));
      d->MRMLRotationSliders->resetUnactiveSliders();
      d->MRMLTranslationSliders->resetUnactiveSliders();
      iniTransform->GetMatrixTransformToParent()->Identity();
      qDebug() << Q_FUNC_INFO << ": Resetting transform node " << iniTransform->GetName();
    }


  }
}

void qSlicerVirtualFractureReconstructionModuleWidget::onAlignmentToggleButtonPressed()
{
  Q_D(qSlicerVirtualFractureReconstructionModuleWidget);

  vtkMRMLModelNode* modelNode;
  vtkMRMLScalarVolumeNode* labelMapNode;
  std::string transformNodeID = "";
  std::vector<std::string> modelIDs = this->ReconstructionNode->GetModelIDs();

  qDebug() << Q_FUNC_INFO << ": model(s) found. " << d->ResultModelsTree1->selectionModel()->selectedRows().size() << " model(s) initially selected";

  QModelIndexList selectedIndices;
  if (d->ResultModelsTree1->selectionModel()->selectedRows().size() < 1)
    d->ResultModelsTree1->selectAll();
  selectedIndices = d->ResultModelsTree1->selectionModel()->selectedRows();

  int i = 0;
  if (!d->AlignmentToggleButton->isChecked() || !d->AlignmentToggleButton2->isChecked())
  {
    if (!d->AlignmentToggleButton->isChecked() && ReconstructionNode->GetCurrentlyActiveModelTab() == 2)
    {
      if (d->ResultModelsTree1->selectionModel()->selectedRows().size() < 1)
        d->ResultModelsTree1->selectAll();
      selectedIndices = d->ResultModelsTree1->selectionModel()->selectedRows();
      d->AlignmentToggleButton2->setChecked(true);
    }
    else
    {
      if (d->ResultModelsTree2->selectionModel()->selectedRows().size() < 1)
        d->ResultModelsTree2->selectAll();
      selectedIndices = d->ResultModelsTree2->selectionModel()->selectedRows();
      d->AlignmentToggleButton->setChecked(true);
    }
    qDebug() << Q_FUNC_INFO << ": Toggle button checked!";
    qDebug() << selectedIndices.size() << " model(s) selected";
    foreach(QModelIndex selectedIndex, selectedIndices)
    {
      //Minus 1 if reference labelis ont displayed
      i = selectedIndex.row();
      if (modelIDs[i].compare(this->ReconstructionNode->GetCurrentReferenceModelID()) == 0) continue;
      qDebug() << Q_FUNC_INFO << ": Seeking transform for model" << modelIDs[i].c_str();
      //Setting up nodes

      modelNode = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelIDs[i]));
      labelMapNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetLabelmapIDForModel(modelIDs[i])));
      transformNodeID = this->ReconstructionNode->GetInitialTransformIDForModel(modelIDs[i]).c_str();

      if (transformNodeID == "")
      {
        qDebug() << Q_FUNC_INFO << ": No transform found for " << modelIDs[i].c_str();
      }
      else if (modelNode->GetTransformNodeID() != transformNodeID)
      {
        qDebug() << Q_FUNC_INFO << ": Setting transform" << transformNodeID.c_str() << "for model" << modelIDs[i].c_str();
        modelNode->SetAndObserveTransformNodeID(transformNodeID.c_str());
        labelMapNode->SetAndObserveTransformNodeID(transformNodeID.c_str());
        this->ReconstructionNode->SetCurrentFragmentTransformNodeID(transformNodeID.c_str());
      }
    }
  }
  else
  {
    if (ReconstructionNode->GetCurrentlyActiveModelTab() == 2)
    {
      if (d->ResultModelsTree1->selectionModel()->selectedRows().size() < 1)
        d->ResultModelsTree1->selectAll();
      selectedIndices = d->ResultModelsTree1->selectionModel()->selectedRows();
      d->AlignmentToggleButton2->setChecked(false);
    }
    else
    {
      if (d->ResultModelsTree2->selectionModel()->selectedRows().size() < 1)
        d->ResultModelsTree2->selectAll();
      selectedIndices = d->ResultModelsTree2->selectionModel()->selectedRows();
      d->AlignmentToggleButton->setChecked(false);
    }
    qDebug() << Q_FUNC_INFO << ": Toggle button NOT checked!";
    qDebug() << selectedIndices.size() << " model(s) selected";
    foreach(QModelIndex selectedIndex, selectedIndices)
    {
      i = selectedIndex.row();
      if (modelIDs[i].compare(this->ReconstructionNode->GetCurrentReferenceModelID()) == 0) continue;
      qDebug() << Q_FUNC_INFO << ": Seeking transform for model" << modelIDs[i].c_str();
      modelNode = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelIDs[i]));
      labelMapNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetLabelmapIDForModel(modelIDs[i])));
      if (d->AlignmentToggleButton->isChecked())
        transformNodeID = this->ReconstructionNode->GetStep1InteractiveTransformIDForModel(modelIDs[i]);
      else
        transformNodeID = this->ReconstructionNode->GetFinalTransformIDForModel(modelIDs[i]);
      if (transformNodeID == "")
      {
        qDebug() << Q_FUNC_INFO << ": No transform found for " << modelIDs[i].c_str();
      }
      else if (modelNode->GetTransformNodeID() != transformNodeID)
      {
        qDebug() << Q_FUNC_INFO << ": Setting transform" << transformNodeID.c_str() << "for model" << modelIDs[i].c_str();
        modelNode->SetAndObserveTransformNodeID(transformNodeID.c_str());
        labelMapNode->SetAndObserveTransformNodeID(transformNodeID.c_str());
        this->ReconstructionNode->SetCurrentFragmentTransformNodeID(transformNodeID.c_str());
      }
    }
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::onReconstructionTabChanged()
{
  Q_D(const qSlicerVirtualFractureReconstructionModuleWidget);
  int tabIndex = d->ReconstructionTab->currentIndex();
  qDebug() << Q_FUNC_INFO << ": Tab index:" << tabIndex << ", FormerlyActiveTab: " << this->ReconstructionNode->GetCurrentlyActiveModelTab();
  std::vector<std::string> modelIDs = this->ReconstructionNode->GetModelIDs();
  qDebug() << modelIDs.size() << " model(s) found";
  if (modelIDs.size() > 0)
  {
    vtkMRMLModelNode* modelNode;
    vtkMRMLScalarVolumeNode* labelMapNode;
    std::string transformNodeID = "";

    // for(unsigned int i=0;i<modelIDs.size();i++)
    //  {
    qDebug() << Q_FUNC_INFO << ": Current reference model ID" << this->ReconstructionNode->GetCurrentReferenceModelID();
    //if(modelIDs[i]!=this->ReconstructionNode->GetCurrentReferenceModelID())
    //{
        //labelMapNode=vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetLabelmapIDForModel(modelIDs[i])));
    QModelIndexList selectedIndices;
    int i = 0;
    if (tabIndex == 3)
    {
      if (this->ReconstructionNode->GetCurrentlyActiveModelTab() == 2)
        selectedIndices = d->ResultModelsTree1->selectionModel()->selectedRows();
      else
        selectedIndices = d->FragmentModelsTree->selectionModel()->selectedRows();
      this->ReconstructionNode->SetCurrentlyActiveModelTab(3);
      qDebug() << Q_FUNC_INFO << ": Number of selected models " << selectedIndices.size();
      foreach(QModelIndex selectedIndex, selectedIndices)
      {
        i = selectedIndex.row();
        d->ResultModelsTree2->selectionModel()->select(selectedIndex, QItemSelectionModel::Select);
        modelNode = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelIDs[i]));
        if (modelIDs[i].compare(this->ReconstructionNode->GetCurrentReferenceModelID()) == 0) continue;
        d->FragmentModelsTree->selectionModel()->clearSelection();
        qDebug() << Q_FUNC_INFO << ": Seeking transform for model" << modelIDs[i].c_str();
        transformNodeID = this->ReconstructionNode->GetStep2TransformIDForModel(modelIDs[i]);
        if (transformNodeID == "")
        {
          qDebug() << Q_FUNC_INFO << ": No transform found for " << modelIDs[i].c_str();
          return;
        }
        if (modelNode->GetTransformNodeID() != transformNodeID)
        {
          qDebug() << Q_FUNC_INFO << ": Setting transform" << transformNodeID.c_str() << "for model" << modelIDs[i].c_str();
          modelNode->SetAndObserveTransformNodeID(transformNodeID.c_str());
          labelMapNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetLabelmapIDForModel(modelIDs[i])));
          labelMapNode->SetAndObserveTransformNodeID(transformNodeID.c_str());
          this->ReconstructionNode->SetCurrentFragmentTransformNodeID(transformNodeID.c_str());
        }
      }
    }
    else if (tabIndex == 2)
    {

      if (this->ReconstructionNode->GetCurrentlyActiveModelTab() == 3)
        selectedIndices = d->ResultModelsTree2->selectionModel()->selectedRows();
      else
        selectedIndices = d->FragmentModelsTree->selectionModel()->selectedRows();
      this->ReconstructionNode->SetCurrentlyActiveModelTab(2);
      qDebug() << Q_FUNC_INFO << ": Number of selected models " << selectedIndices.size();
      qDebug() << Q_FUNC_INFO << ": Seeking transform for model" << modelIDs[i].c_str();
      selectedIndices = d->ResultModelsTree1->selectionModel()->selectedRows();
      foreach(QModelIndex selectedIndex, selectedIndices)
      {
        i = selectedIndex.row();
        d->ResultModelsTree1->selectionModel()->select(selectedIndex, QItemSelectionModel::Select);
        modelNode = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelIDs[i]));
        if (modelIDs[i].compare(this->ReconstructionNode->GetCurrentReferenceModelID()) == 0) continue;
        transformNodeID = this->ReconstructionNode->GetStep1InteractiveTransformIDForModel(modelIDs[i]);
        if (transformNodeID == "")
        {
          qDebug() << Q_FUNC_INFO << ": No transform found for " << modelIDs[i].c_str();
          d->TransformSliders->setEnabled(false);
        }
        else if (modelNode->GetTransformNodeID() != transformNodeID)
        {
          qDebug() << Q_FUNC_INFO << ": Setting transform" << transformNodeID.c_str() << "for model" << modelIDs[i].c_str();

          modelNode->SetAndObserveTransformNodeID(transformNodeID.c_str());
          if (modelNode->GetDisplayNode()->GetOpacity() < 1) modelNode->GetDisplayNode()->SetOpacity(1);
          labelMapNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetLabelmapIDForModel(modelIDs[i])));
          labelMapNode->SetAndObserveTransformNodeID(transformNodeID.c_str());
          this->AssignTransformToSliders(vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelIDs[i].c_str())));
          this->ReconstructionNode->SetCurrentFragmentTransformNodeID(transformNodeID.c_str());
        }
      }
    }
    else if (tabIndex == 1)
    {
      if (this->ReconstructionNode->GetCurrentlyActiveModelTab() == 3)
        selectedIndices = d->ResultModelsTree2->selectionModel()->selectedRows();
      else
        selectedIndices = d->ResultModelsTree1->selectionModel()->selectedRows();
      this->ReconstructionNode->SetCurrentlyActiveModelTab(1);
      qDebug() << Q_FUNC_INFO << ": Number of selected models " << selectedIndices.size();
      //Change tooltip
      d->ReconstructionTab->setToolTip("Please select fragment(s) that shall be aligned. ");
      qDebug() << Q_FUNC_INFO << ": Seeking transform for model" << modelIDs[i].c_str();
      foreach(QModelIndex selectedIndex, selectedIndices)
      {
        i = selectedIndex.row();
        d->FragmentModelsTree->selectionModel()->select(selectedIndex, QItemSelectionModel::Select);
        modelNode = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelIDs[i]));
        if (modelIDs[i].compare(this->ReconstructionNode->GetCurrentReferenceModelID()) == 0) continue;
        transformNodeID = this->ReconstructionNode->GetInitialTransformIDForModel(modelIDs[i]).c_str();
        if (transformNodeID == "")
        {
          qDebug() << Q_FUNC_INFO << ": No transform found for " << modelIDs[i].c_str();
          d->TransformSliders->setEnabled(false);
        }
        else if (modelNode->GetTransformNodeID() != transformNodeID)
        {
          qDebug() << Q_FUNC_INFO << ": Setting transform" << transformNodeID.c_str() << "for model" << modelIDs[i].c_str();
          modelNode->SetAndObserveTransformNodeID(transformNodeID.c_str());
          if (modelNode->GetDisplayNode()->GetOpacity() < 1) modelNode->GetDisplayNode()->SetOpacity(1);
          labelMapNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetLabelmapIDForModel(modelIDs[i])));
          labelMapNode->SetAndObserveTransformNodeID(transformNodeID.c_str());
          this->AssignTransformToSliders(vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelIDs[i].c_str())));
          this->ReconstructionNode->SetCurrentFragmentTransformNodeID(transformNodeID.c_str());
        }
      }
    }
    //}
//}
  }
}

void qSlicerVirtualFractureReconstructionModuleWidget::createModelOfReference(vtkMRMLModelHierarchyNode* hierarchyNode)
{
  Q_D(const qSlicerVirtualFractureReconstructionModuleWidget);

  vtkSlicerVirtualFractureReconstructionLogic* logic = d->logic();
  char* lmID = this->ReconstructionNode->GetReferenceLabelmapNodeID();
  vtkMRMLLabelMapVolumeNode* vnode = vtkMRMLLabelMapVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(lmID));
  //this->ChangeColorOfLabelmap(vnode,10);
  if (!logic->CreateModel(this->ReconstructionNode, vnode, hierarchyNode, true))
  {
    char* refModelID = this->ReconstructionNode->GetCurrentReferenceModelID();
    this->ReconstructionNode->CreateDataContainerForModel(refModelID);
    qDebug() << Q_FUNC_INFO << ": Trying to connect " << refModelID << " with transform.";
    this->CreateTransformAndConnectToModelAndLabelmap(this->mrmlScene()->GetNodeByID(refModelID), lmID, true);
    //logic->GetCLIReconstructionPropertyNode()->SetReferenceModelID(refModelID);
  }
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::createModels()
{
  Q_D(const qSlicerVirtualFractureReconstructionModuleWidget);
  //d->ReconstructionTab->setCurrentIndex(1);
  this->setCursor(Qt::WaitCursor);
  vtkMRMLModelHierarchyNode* reconstructionHierarchy = vtkMRMLModelHierarchyNode::SafeDownCast(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetCurrentFragmentModelHierarchyNodeID()));
  if (!this->ReconstructionNode->GetCurrentReferenceModelID())
    this->createModelOfReference(reconstructionHierarchy);
  this->createModelsOfFragments(reconstructionHierarchy);
  this->setCursor(Qt::ArrowCursor);
  d->InputDataTab->setEnabled(1);
  d->ParametersStep1->setEnabled(1);
  this->ReconstructionNode->SetCurrentlyActiveModelTab(1);
  d->FragmentModelsTree->expandAll();
  //d->FragmentModelsTree->expand(d->FragmentModelsTree->sortFilterProxyModel()->indexFromMRMLNode(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetCurrentFragmentModelHierarchyNodeID())));
  this->setCursor(Qt::ArrowCursor);
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModuleWidget::createModelsOfFragments(vtkMRMLModelHierarchyNode* hierarchyNode)
{
  Q_D(const qSlicerVirtualFractureReconstructionModuleWidget);
  //d->ReconstructionTab->setCurrentIndex(1);
  QModelIndexList selectedIndices;

  if (d->InputDataTree->selectionModel()->selectedRows().size() < 1)
    d->InputDataTree->selectAll();
  selectedIndices = d->InputDataTree->selectionModel()->selectedRows();
  qDebug() << selectedIndices.size() << " rows selected";

  std::vector<vtkSmartPointer<vtkMRMLLabelMapVolumeNode> > labelmaps;
  std::map<unsigned char, unsigned int> labelColors;
  foreach(QModelIndex selectedIndex, selectedIndices)
  {
    vtkMRMLLabelMapVolumeNode* node = vtkMRMLLabelMapVolumeNode::SafeDownCast(d->InputDataTree->currentNode());

    labelColors = d->logic()->GetLabelIntensities(node->GetImageData());
    //Convert single labelmap with multiple labels to multiple labelmaps with one (foreground) label value
    if (labelColors.size() > 1)
    {
      std::vector<std::string> labelmapIDs;

      vtkMRMLScalarVolumeNode* referenceNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetReferenceLabelmapNodeID()));

      labelmapIDs = d->logic()->CreateScalarVolumeNodesFromMultiLabelNode(node, labelColors, referenceNode);
      for (unsigned int i = 0; i < labelmapIDs.size(); i++)
      {
        labelmaps.push_back(vtkMRMLLabelMapVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(labelmapIDs[i])));
      }
    }
    else
      labelmaps.push_back(node);
  }
  for (unsigned int n = 0; n < labelmaps.size(); n++)
  {
    vtkSmartPointer<vtkMRMLLabelMapVolumeNode> vnode = labelmaps[n];
    if (!vnode)
    {
      qDebug() << Q_FUNC_INFO << ": no valid node available to create model";
    }
    else
    {
      //this->ChangeColorOfLabelmap(vnode,10);
      qDebug() << Q_FUNC_INFO << ": Creating model for vnode" << vnode->GetID();

      this->createModelForNode(vnode, hierarchyNode);
      vnode->Modified();
      vnode->UpdateScene(this->mrmlScene());
    }
    //d->InputDataTree->sortFilterProxyModel()->setHiddenNodeIDs(QStringList()<<d->InputDataTree->sortFilterProxyModel()->hiddenNodeIDs()<<vnode->GetID());
  }
  d->ReconstructionTab->setCurrentIndex(1);
  labelmaps.clear();
  labelColors.clear();

}

void qSlicerVirtualFractureReconstructionModuleWidget::createModelForNode(vtkMRMLLabelMapVolumeNode* vnode, vtkMRMLModelHierarchyNode* hierarchyNode)
{
  Q_D(const qSlicerVirtualFractureReconstructionModuleWidget);
  //d->ReconstructionTab->setCurrentIndex(1);
  vtkSlicerVirtualFractureReconstructionLogic* logic = d->logic();
  if (!logic->CreateModel(this->ReconstructionNode, vnode, hierarchyNode))
  {
    char* modelID = this->ReconstructionNode->GetCurrentFragmentModelID();
    this->ReconstructionNode->CreateDataContainerForModel(modelID);
    qDebug() << Q_FUNC_INFO << ": Creating new transform for" << modelID;
    this->CreateTransformAndConnectToModelAndLabelmap(this->mrmlScene()->GetNodeByID(modelID), vnode->GetID(), false);
  }
}

void qSlicerVirtualFractureReconstructionModuleWidget::onStartRegistrationStep1()
{

  Q_D(const qSlicerVirtualFractureReconstructionModuleWidget);
  bool registrationPerformed = false;
  qDebug() << Q_FUNC_INFO << ": Updating CLI module parameters";
  this->updateCLIModuleParameters();

  qDebug() << Q_FUNC_INFO << ": Reference labelmap on regstart" << this->ReconstructionNode->GetReferenceLabelmapNodeID();

  vtkSlicerVirtualFractureReconstructionLogic* logic = d->logic();

  qDebug() << Q_FUNC_INFO << ": Invoking logic for registration";

  d->FragmentModelsTree->setSelectionBehavior(QAbstractItemView::SelectRows);
  QModelIndexList selectedIndices;

  if (d->FragmentModelsTree->selectionModel()->selectedRows().size() < 1)
    d->FragmentModelsTree->selectAll();
  selectedIndices =
    d->FragmentModelsTree->selectionModel()->selectedRows();

  QList<vtkIdType> selectedItemIds = d->FragmentModelsTree->currentItems();
  vtkMRMLSubjectHierarchyNode* shNode = vtkMRMLSubjectHierarchyNode::GetSubjectHierarchyNode(this->mrmlScene());
  foreach(vtkIdType selectedItemId, selectedItemIds)
  {
    vtkMRMLModelNode* mnode = vtkMRMLModelNode::SafeDownCast(shNode->GetItemDataNode(selectedItemId));

    if (!mnode)
    {
      qDebug() << Q_FUNC_INFO << ": No valid fragment node available to start registration";
    }
    //Do not align reference label map
    else
    {
      std::string selectedlabelMapID = this->ReconstructionNode->GetLabelmapIDForModel(mnode->GetID());
      if (selectedlabelMapID.compare(this->ReconstructionNode->GetReferenceLabelmapNodeID()) == 0)
      {
        qDebug() << Q_FUNC_INFO << ": Skipping reference node";
      }
      else
      {
        vtkMRMLScalarVolumeNode* labelMapNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(selectedlabelMapID));
        bool transformPresent = false;
        //Create new transform for output if not already present
        vtkSmartPointer<vtkMRMLLinearTransformNode> step1Transform;
        std::string currentTransform1ID = this->ReconstructionNode->GetStep1TransformIDForModel(mnode->GetID());
        if (currentTransform1ID.compare("") != 0)
        {
          transformPresent = true;
          step1Transform = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(currentTransform1ID));
        }
        else
          step1Transform.TakeReference(this->InitializeTransform(false, "_Step1_", mnode->GetID()));


        //Update current IDs in reconstruction node
        std::string currentIniTransformID = this->ReconstructionNode->GetInitialTransformIDForModel(mnode->GetID());
        this->ReconstructionNode->SetCurrentFragmentModelID(mnode->GetID());
        this->ReconstructionNode->SetOutputTransformNodeID(step1Transform->GetID());
        this->ReconstructionNode->SetCurrentInitialTransformNodeID(currentIniTransformID.c_str());
        this->ReconstructionNode->SetFragmentLabelmapNodeID(selectedlabelMapID.c_str());

        this->prepareIONodesForRegistrationStep1();

        qDebug() << Q_FUNC_INFO << ": Selected model " << mnode->GetID();
        qDebug() << Q_FUNC_INFO << ": Selected fragment labelmap " << selectedlabelMapID.c_str();
        qDebug() << Q_FUNC_INFO << ": Reference labelmap " << this->ReconstructionNode->GetReferenceLabelmapNodeID();
        qDebug() << Q_FUNC_INFO << ": Output transform: " << this->ReconstructionNode->GetOutputTransformNodeID();
        qDebug() << Q_FUNC_INFO << ": labelmapID set " << selectedlabelMapID.c_str();
        qDebug() << Q_FUNC_INFO << ": starting registration";
        this->setCursor(Qt::WaitCursor);
        if (!logic->StartRegistrationStep1(this->ReconstructionNode))
        {
          //If we do not have transform nodes for the model, create them
          char* modelID = mnode->GetID();
          vtkSmartPointer<vtkMRMLLinearTransformNode> step1InteractiveTransform;
          if (!transformPresent)
          {
            qDebug() << Q_FUNC_INFO << ": Adding step 1 transform" << step1Transform->GetID() << " to  " << modelID;
            this->ReconstructionNode->AddStep1TransformIDToModel(modelID, step1Transform->GetID());
            //using interactive Transform for manual interactions and keep Step1Transform unchanged to have fallback when user resets manual interaction
            step1InteractiveTransform.TakeReference(this->InitializeInteractiveTransform(step1Transform));

            qDebug() << Q_FUNC_INFO << ": Adding step 1 interactive transform" << step1InteractiveTransform->GetID() << " to  " << modelID;
            this->ReconstructionNode->AddStep1InteractiveTransformIDToModel(modelID, step1InteractiveTransform->GetID());

            this->ReconstructionNode->AddCandidatePolyDataIDToModel(modelID, d->logic()->GetCLIReconstructionPropertyNode()->GetModifiedCandidatePolyDataID());
            this->ReconstructionNode->AddReferencePolyDataIDToModel(modelID, d->logic()->GetCLIReconstructionPropertyNode()->GetOutputReferencePolyDataID());

            qDebug() << Q_FUNC_INFO << ": Observing " << step1InteractiveTransform->GetID();
            mnode->SetAndObserveTransformNodeID(step1InteractiveTransform->GetID());
            labelMapNode->SetAndObserveTransformNodeID(step1InteractiveTransform->GetID());
            mnode->GetDisplayNode()->SetOpacity(1);
          }
          this->AssignTransformToSliders(step1InteractiveTransform);

          registrationPerformed = true;
          //Correct pose of modifiedReferenceModel
          //logic->CorrectPose(d->logic()->GetCLIReconstructionPropertyNode()->GetOutputReferencePolyDataID(),labelMapNode->GetID(),"","");
          //logic->CorrectPose(d->logic()->GetCLIReconstructionPropertyNode()->GetModifiedCandidatePolyDataID(),labelMapNode->GetID(),step1InteractiveTransform->GetID(),true);

          //this->ReconstructionNode->SetOutputLabelmapNodeID(d->logic()->GetCLIReconstructionPropertyNode()->GetOutputLabelMapNodeID());
          this->ReconstructionNode->SetCurrentModifiedReferenceModelID(d->logic()->GetCLIReconstructionPropertyNode()->GetOutputReferencePolyDataID());
          this->ReconstructionNode->AddCandidatePolyDataIDToModel(this->ReconstructionNode->GetCurrentFragmentModelID(), d->logic()->GetCLIReconstructionPropertyNode()->GetModifiedCandidatePolyDataID());

          vtkMRMLModelDisplayNode::SafeDownCast(vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(d->logic()->GetCLIReconstructionPropertyNode()->GetOutputReferencePolyDataID()))->GetDisplayNode())->SetVisibility(0);
          vtkMRMLModelDisplayNode::SafeDownCast(vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(d->logic()->GetCLIReconstructionPropertyNode()->GetModifiedCandidatePolyDataID()))->GetDisplayNode())->SetVisibility(0);
          //Saving candidate polydata for later use
          QString tempName = qSlicerCoreApplication::application()->temporaryPath();
          tempName.append("/");
          tempName.append(this->mrmlScene()->GetNodeByID(d->logic()->GetCLIReconstructionPropertyNode()->GetModifiedCandidatePolyDataID())->GetName());
          tempName.append(".vtk");

          ReconstructionNode->AddCandidatePolyDataFileNameToModel(modelID, tempName.toStdString());

          vtkMRMLModelNode* candout = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(d->logic()->GetCLIReconstructionPropertyNode()->GetModifiedCandidatePolyDataID()));
          candout->GetStorageNode()->SetFileName(tempName.toStdString().c_str());
          candout->GetStorageNode()->WriteData(candout);
        }
      }
    }
  }

  if (registrationPerformed)
  {
    this->setCursor(Qt::ArrowCursor);
    d->RegistrationStep1Tab->setEnabled(true);
    //Start button 2 will only be enabled when Registration step 1 has been performed for all fragments
    //d->StartButton2->setEnabled(0);
    this->ReconstructionNode->SetCurrentlyActiveModelTab(2);
    qDebug() << Q_FUNC_INFO << ": DONE";


    d->AcceptResultButton->setEnabled(1);

    d->ReconstructionTab->setCurrentIndex(2);
    //d->ResultModelsTree1->expand(d->ResultModelsTree1->sortFilterProxyModel()->indexFromMRMLNode(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetCurrentFragmentModelHierarchyNodeID())));
  }
  //Check if all fragments have been registered so that fine tuning button can be activated
  //std::vector<std::string> modelIDs=this->ReconstructionNode->GetModelIDs();
  //Skip reference model therefore start with numModel=1 (remove reference from model container??)
  /*for(unsigned int numModel=1;numModel<modelIDs.size();numModel++)
  {
      if(this->ReconstructionNode->GetStep1TransformIDForModel(modelIDs[numModel]).compare("")==0) return ;
  }
  d->StartButton2->setEnabled(1);*/
}


void qSlicerVirtualFractureReconstructionModuleWidget::prepareIONodesForRegistrationStep1()
{
  Q_D(const qSlicerVirtualFractureReconstructionModuleWidget);
  QString temp;
  this->updateCLIModuleParameters();
  //Check if reference model node (containing precalculated features) is present
  if (this->ReconstructionNode->GetCurrentModifiedReferenceModelID() && d->UsePrecompReferenceCB->isChecked())
  {
    d->logic()->GetCLIReconstructionPropertyNode()->SetInputReferenceModelID(ReconstructionNode->GetCurrentModifiedReferenceModelID());
    //d->logic()->GetCLIReconstructionPropertyNode()->SetInputReferenceModelID(ReconstructionNode->GetCurrentReferenceModelID());
  }
  else
    d->logic()->GetCLIReconstructionPropertyNode()->SetInputReferenceModelID("");

  //Add node for updated reference model
  vtkNew<vtkMRMLModelNode> modifiedReferenceModel;
  this->mrmlScene()->AddNode(modifiedReferenceModel.GetPointer());
  temp = "NewReferenceModelFor";
  temp.append(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetCurrentFragmentModelID())->GetName());
  std::string refname = this->mrmlScene()->GetUniqueNameByString(temp.toStdString().c_str());
  modifiedReferenceModel->SetName(refname.c_str());
  d->logic()->GetCLIReconstructionPropertyNode()->SetOutputReferencePolyDataID(modifiedReferenceModel->GetID());
  modifiedReferenceModel->HideFromEditorsOn();
  /*
      vtkSmartPointer<vtkMRMLModelStorageNode> storageNode = NULL;
      vtkSmartPointer<vtkMRMLStorageNode> snode = modifiedReferenceModel->GetStorageNode();
      if (snode != NULL)
      {
        storageNode = vtkMRMLModelStorageNode::SafeDownCast(snode);
      }
      if (storageNode == NULL)
      {
        storageNode = vtkMRMLModelStorageNode::New();
        storageNode->SetScene(this->mrmlScene());
        QString tempName = qSlicerCoreApplication::application()->temporaryPath();
        tempName.append(temp);
        tempName.append(".vtk");
        storageNode->SetFileName(tempName.toStdString().c_str());
        this->mrmlScene()->AddNode(storageNode);
        modifiedReferenceModel->SetAndObserveStorageNodeID(storageNode->GetID());
        //storageNode->Delete();
      }
  */
  //Add node for updated candidate model
  vtkSmartPointer<vtkMRMLModelNode> currentModelNodeModified = vtkSmartPointer<vtkMRMLModelNode>::New();
  this->mrmlScene()->AddNode(currentModelNodeModified);
  temp = "OutputModelForNode";
  temp.append(this->ReconstructionNode->GetCurrentFragmentModelID());
  std::string outname = this->mrmlScene()->GetUniqueNameByString(temp.toStdString().c_str());
  currentModelNodeModified->SetName(outname.c_str());
  d->logic()->GetCLIReconstructionPropertyNode()->SetModifiedCandidatePolyDataID(currentModelNodeModified->GetID());
  currentModelNodeModified->HideFromEditorsOn();

  //Creating result node (still necessary??)

  /*vtkNew<vtkMRMLScalarVolumeNode> registrationResult;
  registrationResult->SetLabelMap(1);
  this->mrmlScene()->AddNode(registrationResult.GetPointer());
  temp="ResultLabelMapForNode";
  temp.append(registrationResult->GetID());
  std::string uname = this->mrmlScene()->GetUniqueNameByString(temp.toStdString().c_str());
  registrationResult->SetName(uname.c_str());
  d->logic()->GetCLIReconstructionPropertyNode()->SetOutputLabelMapNodeID(registrationResult->GetID());*/

  /*qDebug()<<"Creating result display node";
  vtkNew<vtkMRMLLabelMapVolumeDisplayNode> displayNode;
  displayNode->SetScene(this->mrmlScene());
  this->mrmlScene()->AddNode(displayNode.GetPointer());


  displayNode->SetAndObserveColorNodeID(this->Internal->ColorLogic->GetDefaultLabelMapColorNodeID());
  registrationResult->SetAndObserveDisplayNodeID(displayNode.GetPointer()->GetID());*/
}

void qSlicerVirtualFractureReconstructionModuleWidget::onStartRegistrationStep2()
{

  Q_D(const qSlicerVirtualFractureReconstructionModuleWidget);
  this->setCursor(Qt::WaitCursor);
  this->updateCLIModuleParameters();
  vtkSlicerVirtualFractureReconstructionLogic* logic = d->logic();

  qDebug() << Q_FUNC_INFO << ": Invoking logic for registration";
  std::vector<std::string> modelIDs = this->ReconstructionNode->GetModelIDs();
  //Skip reference model therefore start with numModel=1 (remove reference from model container??)
  for (unsigned int numModel = 1; numModel < modelIDs.size(); numModel++)
  {
    std::string modelID = modelIDs[numModel];
    //Disable display of unregistered fragment
    vtkSmartPointer<vtkMRMLLinearTransformNode> finalTransform;
    finalTransform.TakeReference(this->InitializeTransform2("_Step2_", modelID));
    qDebug() << Q_FUNC_INFO << ": Adding step 2 transform" << finalTransform->GetID() << " to  " << modelID.c_str();
    this->ReconstructionNode->AddFinalTransformIDToModel(modelID, finalTransform->GetID());

    //using interactive Transform for manual interactions and keep Step1Transform unchanged to have fallback when user resets manual interaction
    vtkSmartPointer<vtkMRMLLinearTransformNode> step2InteractiveTransform;
    step2InteractiveTransform.TakeReference(this->InitializeInteractiveTransform(finalTransform));
    qDebug() << Q_FUNC_INFO << ": Adding step 2 interactive transform" << step2InteractiveTransform->GetID() << " to  " << modelID.c_str();
    this->ReconstructionNode->AddStep2TransformIDToModel(modelID, step2InteractiveTransform->GetID());
  }

  //Writing input models
  //Do not save reference model (not needed for fine tuning step)
  vtkSmartPointer<vtkMRMLModelNode> tempModelNode;
  QString commaSeparatedModelFileNames, commaSeparatedTransformIDs, commaSeparatedInputTransformIDs;
  std::vector<std::string> outputTransformNames;
  for (unsigned int numModel = 1; numModel < modelIDs.size(); numModel++)
  {
    tempModelNode = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelIDs[numModel]));
    if (!tempModelNode)
    {
      qDebug() << Q_FUNC_INFO << ": Model node " << modelIDs[numModel].c_str() << " not found.";
    }
    std::string tempname = this->ReconstructionNode->GetCandidatePolyDataFileNameForModel(tempModelNode->GetID());
    commaSeparatedModelFileNames.append(tempname.c_str());
    if (numModel < modelIDs.size() - 1)
      commaSeparatedModelFileNames.append(",");
    qDebug() << Q_FUNC_INFO << ": Polydata " << tempname.c_str() << " set as parameter.";

    //Add transforms for polydata. Unfortunately a vector of transforms cannot be passed, so transforms have to bes saved before passing parameters
    std::string tempNameInputTransform = this->ReconstructionNode->GetStep2TransformIDForModel(tempModelNode->GetID());
    vtkSmartPointer<vtkMRMLTransformNode> inputTransformNode = vtkMRMLTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(tempNameInputTransform));
    QString tempNameInputTransFinal = qSlicerCoreApplication::application()->temporaryPath();
    tempNameInputTransFinal.append("/");
    tempNameInputTransFinal.append(tempNameInputTransform.c_str());
    tempNameInputTransFinal.append(".txt");


    if (!inputTransformNode->GetStorageNode())
    {
      vtkSmartPointer<vtkMRMLTransformStorageNode> transformInputStorage = vtkMRMLTransformStorageNode::SafeDownCast(inputTransformNode->GetStorageNode());
      if (transformInputStorage == NULL)
        transformInputStorage = vtkSmartPointer<vtkMRMLTransformStorageNode>::New();
      transformInputStorage->SetFileName(tempNameInputTransFinal.toStdString().c_str());
      transformInputStorage->SetScene(this->mrmlScene());
      this->mrmlScene()->AddNode(transformInputStorage.GetPointer());
      inputTransformNode->SetAndObserveStorageNodeID(transformInputStorage->GetID());
    }
    else
      inputTransformNode->GetStorageNode()->SetFileName(tempNameInputTransFinal.toStdString().c_str());

    inputTransformNode->GetStorageNode()->WriteData(inputTransformNode);

    commaSeparatedInputTransformIDs.append(tempNameInputTransFinal.toStdString().c_str());
    if (numModel < modelIDs.size() - 1)
    {
      commaSeparatedInputTransformIDs.append(",");
    }
    if (numModel > 1)
    {
      std::string tempNameTransform = this->ReconstructionNode->GetFinalTransformIDForModel(tempModelNode->GetID());
      vtkSmartPointer<vtkMRMLTransformNode> transformNode = vtkMRMLTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(tempNameTransform));
      QString tempNameFinal = qSlicerCoreApplication::application()->temporaryPath();
      tempNameFinal.append("/");
      tempNameFinal.append(tempNameTransform.c_str());
      tempNameFinal.append(".txt");

      if (transformNode->GetStorageNode() == NULL)
      {
        vtkSmartPointer<vtkMRMLTransformStorageNode> transformStorage = vtkMRMLTransformStorageNode::SafeDownCast(transformNode->GetStorageNode());
        if (transformStorage == NULL)
          transformStorage = vtkSmartPointer<vtkMRMLTransformStorageNode>::New();
        transformStorage->SetFileName(tempNameFinal.toStdString().c_str());
        transformStorage->SetScene(this->mrmlScene());
        this->mrmlScene()->AddNode(transformStorage.GetPointer());
        transformNode->SetAndObserveStorageNodeID(transformStorage->GetID());
      }
      else
        transformNode->GetStorageNode()->SetFileName(tempNameFinal.toStdString().c_str());

      transformNode->GetStorageNode()->WriteData(transformNode);
      //transformNames.push_back(transformNode->GetID());

      //Create comma separated string containing transform names
      commaSeparatedTransformIDs.append(tempNameFinal.toStdString().c_str());
      outputTransformNames.push_back(tempNameFinal.toStdString());

      //commaSeparatedTransformIDs.append(transformNode->GetID());
      if (numModel < modelIDs.size() - 1)
      {
        commaSeparatedTransformIDs.append(",");
      }
      qDebug() << Q_FUNC_INFO << ": Transform " << tempNameTransform.c_str() << " set as output transform parameter.";
      qDebug() << Q_FUNC_INFO << ": Transform " << tempNameInputTransform.c_str() << " set as input transform parameter.";
      //tempModelNode->SetAndObserveTransformNodeID(transformNode->GetID());
    }
  }
  logic->GetCLIReconstructionPropertyNode()->SetCommaSeparatedModelFileNames(commaSeparatedModelFileNames.toStdString().c_str());
  logic->GetCLIReconstructionPropertyNode()->SetCommaSeparatedInputTransformIDs(commaSeparatedInputTransformIDs.toStdString().c_str());
  logic->GetCLIReconstructionPropertyNode()->SetCommaSeparatedTransformIDs(commaSeparatedTransformIDs.toStdString().c_str());


  if (!logic->StartRegistrationStep2(this->ReconstructionNode))
  {
    std::string labelMapID;
    ///Update transform after registration step (starting with numModel=2 since there is no result tranfsorm for reference and
    for (unsigned int numModel = 2; numModel < modelIDs.size(); numModel++)
    {
      vtkSmartPointer<vtkMRMLLinearTransformNode> tempFinalTrans = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(ReconstructionNode->GetFinalTransformIDForModel(modelIDs[numModel])));
      vtkSmartPointer<vtkMRMLLinearTransformNode> tempStep2InteractiveTrans = vtkMRMLLinearTransformNode::SafeDownCast(this->mrmlScene()->GetNodeByID(ReconstructionNode->GetStep2TransformIDForModel(modelIDs[numModel])));
      std::string filename = outputTransformNames[numModel - 2].c_str();
      qDebug() << Q_FUNC_INFO << ": Output transfer name that will be re-read: " << filename.c_str() << " for model node " << modelIDs[numModel].c_str();
      tempStep2InteractiveTrans->GetStorageNode()->SetFileName(filename.c_str());
      tempStep2InteractiveTrans->GetStorageNode()->ReadData(tempStep2InteractiveTrans);

      tempFinalTrans->GetStorageNode()->SetFileName(filename.c_str());
      tempFinalTrans->GetStorageNode()->ReadData(tempFinalTrans);
      tempModelNode = vtkMRMLModelNode::SafeDownCast(this->mrmlScene()->GetNodeByID(modelIDs[numModel]));
      tempModelNode->SetAndObserveTransformNodeID(tempStep2InteractiveTrans->GetID());
      labelMapID = ReconstructionNode->GetLabelmapIDForModel(modelIDs[numModel]);
      vtkMRMLScalarVolumeNode* labelMapNode = vtkMRMLScalarVolumeNode::SafeDownCast(this->mrmlScene()->GetNodeByID(labelMapID));
      labelMapNode->SetAndObserveTransformNodeID(tempStep2InteractiveTrans->GetID());
    }
    //OLD
    //std::pair<std::string,std::string> LabelAndTransformIDs(vnode->GetID(),currentTransformID);
    //std::pair<std::string,std::string> LabelAndTransformIDs(selectedlabelMapID.c_str(),currentTransformID);
    //qDebug()<<"Assigning label"<<LabelAndTransformIDs.first.c_str()<<" and transform "<<LabelAndTransformIDs.second.c_str()<<" to model "<<modelID;
  }

  this->setCursor(Qt::ArrowCursor);
  d->ReconstructionTab->setCurrentIndex(3);
  this->ReconstructionNode->SetCurrentlyActiveModelTab(3);

  d->RegistrationStep2Tab->setEnabled(true);
  d->ResultModelsTree1->selectAll();
  this->setCursor(Qt::ArrowCursor);
  qDebug() << Q_FUNC_INFO << ": DONE";

  //d->ResultModelsTree2->sortFilterProxyModel()->setHiddenNodeIDs(QStringList() <<this->ReconstructionNode->GetCurrentReferenceModelID());
  d->AcceptResultButton2->setEnabled(1);
  //d->ResultModelsTree1->expand(d->ResultModelsTree2->sortFilterProxyModel()->indexFromMRMLNode(this->mrmlScene()->GetNodeByID(this->ReconstructionNode->GetCurrentFragmentModelHierarchyNodeID())));
  //d->ResultModelsTree1->sortFilterProxyModel()->setHiddenNodeIDs(QStringList() <<this->ReconstructionNode->GetCurrentFragmentModelHierarchyNodeID());
}


void qSlicerVirtualFractureReconstructionModuleWidget::WatcherUpdate()
{
  qDebug() << Q_FUNC_INFO << ": Update!!!";
}

