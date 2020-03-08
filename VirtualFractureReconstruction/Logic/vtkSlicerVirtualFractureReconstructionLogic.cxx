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
//QT includes


// VirtualFractureReconstruction includes
#include "vtkSlicerVirtualFractureReconstructionLogic.h"
#include "vtkSlicerCLIModuleLogic.h"


// MRML includes
#include <vtkMRMLVirtualFractureReconstructionNode.h>
#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLLabelMapVolumeNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLModelHierarchyNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLColorNode.h"
#include "vtkMRMLCommandLineModuleNode.h"
#include "vtkMRMLColorTableStorageNode.h"
#include "vtkMRMLLabelMapVolumeDisplayNode.h"
#include "vtkMRMLVolumeArchetypeStorageNode.h"

// VTK includes
#include <vtkNew.h>
#include "vtkPolyDataWriter.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkMatrix4x4.h"
#include "vtkXMLImageDataWriter.h"
#include "vtksys/SystemTools.hxx"

// STD includes
#include <cassert>


#include "qSlicerApplication.h"
#include "qSlicerModuleManager.h"
#include "qSlicerCLIModule.h"
#include "qSlicerCLIModuleWidget.h"

#include "vtkCollection.h"


class vtkSlicerVirtualFractureReconstructionLogic::vtkInternal
{
public:
  vtkInternal();

   vtkSlicerCLIModuleLogic* ModelMakerLogic;
   vtkMRMLColorLogic* ColorLogic;
   vtkSlicerCLIModuleLogic* CLIReconstructorLogic;
};

vtkSlicerVirtualFractureReconstructionLogic::vtkInternal::vtkInternal()
{
  this->ModelMakerLogic = 0;
  this->ColorLogic = 0;
  this->CLIReconstructorLogic = 0;
}



//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerVirtualFractureReconstructionLogic);

//----------------------------------------------------------------------------
vtkSlicerVirtualFractureReconstructionLogic::vtkSlicerVirtualFractureReconstructionLogic()
{
    this->Internal = new vtkInternal;
}

//----------------------------------------------------------------------------
vtkSlicerVirtualFractureReconstructionLogic::~vtkSlicerVirtualFractureReconstructionLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerVirtualFractureReconstructionLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vtkSlicerVirtualFractureReconstructionLogic::SetModelMakerLogic(vtkSlicerCLIModuleLogic* logic)
{
  this->Internal->ModelMakerLogic = logic;
}

void vtkSlicerVirtualFractureReconstructionLogic::SetColorLogic(vtkMRMLColorLogic* logic)
{
  this->Internal->ColorLogic = logic;
}


void vtkSlicerVirtualFractureReconstructionLogic::SetCLIReconstructionLogic(vtkSlicerCLIModuleLogic* logic)
{
  this->Internal->CLIReconstructorLogic = logic;
}

void vtkSlicerVirtualFractureReconstructionLogic::SetCLIReconstructionPropertyNode(vtkMRMLCLIFractureReconstructionPropertyNode* propNode)
{
  this->CLIReconstructionPropertyNode.TakeReference(propNode);
}

vtkMRMLCLIFractureReconstructionPropertyNode* vtkSlicerVirtualFractureReconstructionLogic::GetCLIReconstructionPropertyNode()
{
    return this->CLIReconstructionPropertyNode;
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualFractureReconstructionLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerVirtualFractureReconstructionLogic::RegisterNodes()
{
    if(!this->GetMRMLScene())
      return;
    vtkMRMLVirtualFractureReconstructionNode* pNode = vtkMRMLVirtualFractureReconstructionNode::New();
    this->GetMRMLScene()->RegisterNodeClass(pNode);
    pNode->Delete();
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualFractureReconstructionLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualFractureReconstructionLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerVirtualFractureReconstructionLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
double* vtkSlicerVirtualFractureReconstructionLogic::GetColorFromIndex(int index) const
{
    //qDebug() << "qMRMLLabelComboBox::colorFromIndex - index:" << index;
    double* colorTable= new double[4];
    if (index < 0)
    {
        for(int i=0;i<4;i++)
            colorTable[i]=0;
        return colorTable;
    }

    vtkMRMLColorNode*cNode=vtkMRMLColorNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->Internal->ColorLogic->GetDefaultLabelMapColorNodeID()));

    vtkLookupTable* table = cNode->GetLookupTable();

    table->GetTableValue(index, colorTable);

    return colorTable;
}

bool vtkSlicerVirtualFractureReconstructionLogic
::CreateModel(vtkMRMLVirtualFractureReconstructionNode *reconstructionNode,vtkMRMLLabelMapVolumeNode* volumeNode,vtkMRMLModelHierarchyNode* hierarchyNode,bool reference)
{
   vtkMRMLCommandLineModuleNode* modelMakerNode=
    this->Internal->ModelMakerLogic->CreateNodeInScene();
    reconstructionNode->SetModelMakerNodeID(modelMakerNode->GetID());
    if(!modelMakerNode) return 1;

   vtkSmartPointer<vtkMRMLModelHierarchyNode> modelHierarchyNode;
   if(reconstructionNode->GetCurrentFragmentModelHierarchyNodeID()&&!hierarchyNode)
   {
       modelHierarchyNode=vtkMRMLModelHierarchyNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(reconstructionNode->GetCurrentFragmentModelHierarchyNodeID()));
       modelMakerNode->SetDescription("ModelMakerNode");
   }
   else if(hierarchyNode)
   {
       qDebug() <<"Yes we do have a hierarchy node";
       modelHierarchyNode=hierarchyNode;
       qDebug() <<"Model hierarchy node "<<modelHierarchyNode->GetID()<<"will be used";
   }
   else
   {
       qDebug() <<"New model hierarchy node will be created";
       modelHierarchyNode=vtkSmartPointer<vtkMRMLModelHierarchyNode>::New();
       modelHierarchyNode->SetDescription("New model for fracture reconstruction");
       modelHierarchyNode->SetName("BoneModelHierarchy");
       modelHierarchyNode->SetExpanded(true);

       modelHierarchyNode->SetAddToScene(true);
       this->GetMRMLScene()->AddNode(modelHierarchyNode.GetPointer() );
       reconstructionNode->SetCurrentFragmentModelHierarchyNodeID(modelHierarchyNode->GetID());
   }


    qDebug() <<"Using model hierarchy node"<<modelHierarchyNode->GetID()<<" as model scene id.";

    char modelName[100];
    sprintf(modelName,"ModelFor%s",volumeNode->GetName());
    modelMakerNode->SetParameterAsString("InputVolume",volumeNode->GetID());
    modelMakerNode->SetParameterAsString("ModelSceneFile",modelHierarchyNode->GetID());
    modelMakerNode->SetParameterAsString("Name",modelName);
    modelMakerNode->SetParameterAsString("ColorTable", "ModelColorTable");
    modelMakerNode->SetParameterAsBool("debug",true);
    qDebug() <<"model maker input set";

    this->Internal->ModelMakerLogic->ApplyAndWait(modelMakerNode);

    vtkMRMLModelHierarchyNode* hierarchy = vtkMRMLModelHierarchyNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(modelHierarchyNode->GetID()));
    qDebug() <<"Model hierarchy node with ID "<<hierarchy->GetID()<<"  was created.";

    vtkSmartPointer<vtkCollection> modelNodeColl=vtkSmartPointer<vtkCollection>::New();
    hierarchy->GetChildrenDisplayableNodes(modelNodeColl);

    vtkSmartPointer<vtkMRMLModelNode> modelNode=vtkMRMLModelNode::SafeDownCast(modelNodeColl->GetItemAsObject(modelNodeColl->GetNumberOfItems()-1));

    int labelVal=this->GetLabelColor(volumeNode->GetImageData());
    qDebug()<<"ModelDisplayNode: "<<modelNode->GetModelDisplayNode()<<"with color index "<<labelVal<<" created ";
    modelNode->GetModelDisplayNode()->SetColor(this->GetColorFromIndex(labelVal));
    if(reference)
        modelNode->GetModelDisplayNode()->SetOpacity(0.5);

    modelNode->Modified();
    //Updating properties for labelmap
    volumeNode->GetDisplayNode()->SetColor(this->GetColorFromIndex(1));
    volumeNode->Modified();

    modelNode->UpdateScene(this->GetMRMLScene());

    ///TODO:Move this to widget class
    if(!reference)
        reconstructionNode->SetCurrentFragmentModelID(modelNode->GetID());
    else
        reconstructionNode->SetCurrentReferenceModelID(modelNode->GetID());

    this->GetMRMLScene()->RemoveNode(modelMakerNode);


    modelMakerNode=NULL;
    return 0;
}

//---------------------------------------------------------------------------
bool vtkSlicerVirtualFractureReconstructionLogic
::StartRegistrationStep1(vtkMRMLVirtualFractureReconstructionNode *reconstructionNode)
{
    vtkSmartPointer<vtkMRMLCommandLineModuleNode> reconstructionNodeCLI;
    qDebug() <<"Creating FractureReconstructionCLI node for logic "<<this->Internal->CLIReconstructorLogic;
    reconstructionNodeCLI= this->Internal->CLIReconstructorLogic->CreateNodeInScene();
    assert(reconstructionNodeCLI.GetPointer() != 0);
    qDebug() <<"Setting CLINodeID "<<reconstructionNodeCLI->GetID()<<" for reconstruction node with address"<<reconstructionNode;
    reconstructionNode->SetCLIReconstructionNodeID(reconstructionNodeCLI->GetID());

    //Set all parameters
    this->UpdateStep1Parameters(reconstructionNodeCLI,reconstructionNode);

    //reconstructionNodeCLI->SetParameterAsString("ModelSceneFile",reconstructionNode->GetCurrentFragmentModelHierarchyNodeID());
    //TODO Why do i get fault when setting configFile?
    //reconstructionNodeCLI->SetParameterAsString("configFilePath","/home/kfritscher/bin/Slicer-source/Slicer4/Slicer4/Modules/Loadable/VirtualFractureReconstruction/_Settings2.ini");

    qDebug()<<"Parameters for registration set.Starting registration";

    this->Internal->CLIReconstructorLogic->ApplyAndWait(reconstructionNodeCLI);

    this->GetMRMLScene()->RemoveNode(reconstructionNodeCLI);

    return 0;
}

bool vtkSlicerVirtualFractureReconstructionLogic
::StartRegistrationStep2(vtkMRMLVirtualFractureReconstructionNode *reconstructionNode)
{
    vtkSmartPointer<vtkMRMLCommandLineModuleNode> reconstructionNodeCLI;
    qDebug() <<"Creating FractureReconstructionCLI node for fine tuning "<<this->Internal->CLIReconstructorLogic;
    reconstructionNodeCLI=this->Internal->CLIReconstructorLogic->CreateNodeInScene();
    assert(reconstructionNodeCLI.GetPointer() != 0);

    qDebug() <<"Setting CLINodeID "<<reconstructionNodeCLI->GetID()<<" for reconstruction node with address"<<reconstructionNode;

    reconstructionNode->SetCLIReconstructionNodeID(reconstructionNodeCLI->GetID());

    qDebug()<<"CS-InputTransform: "<<this->GetCLIReconstructionPropertyNode()->GetCommaSeparatedInputTransformIDs();
    qDebug()<<"CS-OutputTransform: "<<this->GetCLIReconstructionPropertyNode()->GetCommaSeparatedTransformIDs();

    this->UpdateStep2Parameters(reconstructionNodeCLI);

    this->Internal->CLIReconstructorLogic->ApplyAndWait(reconstructionNodeCLI);
    this->GetMRMLScene()->RemoveNode(reconstructionNodeCLI);

    return 0;
}

void vtkSlicerVirtualFractureReconstructionLogic::CorrectPose(std::string incorrectModelID, std::string correctNodeID,std::string additionalTransformNodeID,bool invert)
{

    vtkSmartPointer<vtkMatrix4x4> referenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(correctNodeID))->GetIJKToRASDirectionMatrix(referenceMatrix);
    vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(incorrectModelID))->ApplyTransformMatrix(referenceMatrix);
    referenceMatrix->Identity();

    if(additionalTransformNodeID.compare("")!=0)
    {
        vtkMRMLTransformNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(additionalTransformNodeID))->GetMatrixTransformToWorld(referenceMatrix);
        if(invert)
            referenceMatrix->Invert();
        vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(incorrectModelID))->ApplyTransformMatrix(referenceMatrix);
    }
    vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(incorrectModelID))->Modified();

}


std::map<unsigned char, unsigned int> vtkSlicerVirtualFractureReconstructionLogic::GetLabelIntensities(vtkSmartPointer<vtkImageData> image)
{
    int *dimensions = image->GetDimensions();
    int tempVal=0;
    unsigned int numVoxels=dimensions[0]*dimensions[1]*dimensions[2];
    typedef std::map<unsigned char,unsigned int> intMapType;
    intMapType labelColors;
    for(unsigned int x=0;x<numVoxels;x++)
    {
      tempVal=image->GetPointData()->GetScalars()->GetTuple1(x);
      if(tempVal>0)
      {
        if(labelColors.find(tempVal)==labelColors.end())
            labelColors.insert(std::map<int,unsigned int>::value_type(tempVal,1));
        else
           (*labelColors.find(tempVal)).second++;
      }
    }
    return labelColors;
}


unsigned int vtkSlicerVirtualFractureReconstructionLogic::GetLabelColor(vtkSmartPointer<vtkImageData> image)
{
    int *dimensions = image->GetDimensions();
    int tempVal=0;
    unsigned int numVoxels=dimensions[0]*dimensions[1]*dimensions[2];

    for(unsigned int x=0;x<numVoxels;x++)
    {
      tempVal=image->GetPointData()->GetScalars()->GetTuple1(x);
      if(tempVal>0)
       return tempVal;
    }
    return 0;
}

std::vector<std::string > vtkSlicerVirtualFractureReconstructionLogic::CreateScalarVolumeNodesFromMultiLabelNode(vtkSmartPointer<vtkMRMLScalarVolumeNode> node,std::map<unsigned char, unsigned int> labelColors,vtkSmartPointer<vtkMRMLScalarVolumeNode> referenceNode)
{
    int dimensions[3]={0,0,0};
    double spacing[3]={0,0,0};
    node->GetImageData()->GetDimensions(dimensions);
    node->GetImageData()->GetSpacing(spacing);
    typedef std::map< unsigned int, vtkSmartPointer<vtkImageData> > map_type;
    typedef map_type::value_type pair_type;
    map_type imageMap;

    typedef std::map<unsigned char, unsigned int> labelColorMap_type;
    for(labelColorMap_type::iterator it = labelColors.begin(); it!=labelColors.end(); ++it)
    {
        vtkSmartPointer<vtkImageData> newImage = vtkSmartPointer<vtkImageData>::New();
        newImage->SetExtent(0,dimensions[0],0,dimensions[1],0,dimensions[2]);
        newImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
        newImage->SetSpacing(spacing);
        newImage->GetPointData()->GetScalars()->FillComponent(0, 0);

        imageMap.insert(pair_type((*it).first,newImage));
    }

    //for(labelColorMap_type::iterator it = labelColors.begin(); it!=labelColors.end(); ++it)
    //{
        for (int z = 0; z < dimensions[2]; z++)
        {
           for (int y = 0; y < dimensions[1]; y++)
           {
             for (int x = 0; x < dimensions[0]; x++)
             {
                unsigned char* pixel = static_cast<unsigned char*>(node->GetImageData()->GetScalarPointer(x,y,z));
                if(pixel[0]!=0)
                {
                    unsigned char val=pixel[0];
                    map_type::iterator imageIt=imageMap.find(val);
                    if(imageIt!=imageMap.end())
                    {
                        unsigned char* newPixel = static_cast<unsigned char*>((*imageIt).second->GetScalarPointer(x,y,z));
                        newPixel[0]=pixel[0];
                    }
                }
              }
           }
        }
    //}
    map_type volumeOrderedImages;
    map_type::iterator it;
    for(it = imageMap.begin(); it!=imageMap.end(); ++it)
    {
        volumeOrderedImages.insert(pair_type((*labelColors.find((*it).first)).second,(*it).second));

    }

    std::vector<std::string> volumeNodes(volumeOrderedImages.size());
    int counter=1;
    for(it = volumeOrderedImages.begin(); it!=volumeOrderedImages.end(); ++it)
    {
        vtkSmartPointer<vtkMRMLLabelMapVolumeNode> vnode =vtkSmartPointer<vtkMRMLLabelMapVolumeNode>::New();
        vnode->SetAndObserveImageData((*it).second);
        //Transform node so that orientation corresponds to itk::Image
        vtkSmartPointer<vtkMatrix4x4> referenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        referenceNode->GetIJKToRASMatrix(referenceMatrix);
        vnode->ApplyTransformMatrix(referenceMatrix);

        //vtkMRMLModelNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(incorrectModelID))->Modified();

        ///Get color again (SLOW!) to generate name
        std::string labelName;
        std::ostringstream sstream;
        sstream<<"Fragment"<<(volumeOrderedImages.size()-counter)+1;
        labelName=sstream.str();
        vnode->SetName(labelName.c_str());

        this->GetMRMLScene()->AddNode(vnode);
        vnode->SetScene(this->GetMRMLScene());

        //Define display node, color node, stroage node for new volume
        vtkSmartPointer<vtkMRMLLabelMapVolumeDisplayNode> displayNode = vtkSmartPointer<vtkMRMLLabelMapVolumeDisplayNode>::New();

        this->GetMRMLScene()->AddNode(displayNode);
        vnode->SetScene(this->GetMRMLScene());

        displayNode->SetDefaultColorMap();

        //vnode->UpdateScene(this->GetMRMLScene());
        displayNode->UpdateScene(this->GetMRMLScene());

        vnode->AddAndObserveDisplayNodeID(displayNode->GetID());
        vtkMRMLColorNode *colorNode = vtkMRMLColorNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(this->Internal->ColorLogic->GetDefaultLabelMapColorNodeID()));

        displayNode->SetAndObserveColorNodeID(colorNode->GetID());

        vtkMRMLVolumeArchetypeStorageNode *storageNode = vtkMRMLVolumeArchetypeStorageNode::New();
        QString tempNameFinal = qSlicerCoreApplication::application()->temporaryPath();
        tempNameFinal.append("/");
        tempNameFinal.append(labelName.c_str());
        tempNameFinal.append(".vtk");
        storageNode->SetFileName(tempNameFinal.toStdString().c_str());

        this->GetMRMLScene()->AddNode(storageNode);
        storageNode->SetScene(this->GetMRMLScene());
        //vnode->AddAndObserveStorageNodeID(storageNode->GetID());

        volumeNodes[volumeOrderedImages.size()-counter]=vnode->GetID();

        qDebug()<<"ADDING LABEL WITH "<<(*it).first<<" VOXELS" ;
        counter++;

    }
    //imageMap.clear();
    //volumeOrderedImages.clear();

    return volumeNodes;
}


void vtkSlicerVirtualFractureReconstructionLogic::UpdateStep2Parameters(vtkSmartPointer<vtkMRMLCommandLineModuleNode> reconstructionNodeCLI)
{
    const char* tempPath=qSlicerCoreApplication::application()->temporaryPath().toUtf8().constData();
    reconstructionNodeCLI->SetParameterAsString("slicerTempPath",tempPath);
    qDebug()<<"SLICER TEMP LOGIG"<<reconstructionNodeCLI->GetParameterAsString("slicerTempPath").c_str();

    reconstructionNodeCLI->SetParameterAsString("fineTuningInputTransforms",this->GetCLIReconstructionPropertyNode()->GetCommaSeparatedInputTransformIDs());
    reconstructionNodeCLI->SetParameterAsString("fineTuningOutputTransforms",this->GetCLIReconstructionPropertyNode()->GetCommaSeparatedTransformIDs());
    reconstructionNodeCLI->SetParameterAsString("fragmentModelFineTuning",this->GetCLIReconstructionPropertyNode()->GetCommaSeparatedModelFileNames());
    reconstructionNodeCLI->SetParameterAsFloat("maxPointDistanceMulti",this->GetCLIReconstructionPropertyNode()->GetMaxPointDistanceMulti());
    reconstructionNodeCLI->SetParameterAsInt("maxICPIterations",this->GetCLIReconstructionPropertyNode()->GetMaxICPIterations());
    reconstructionNodeCLI->SetParameterAsFloat("maxCrestCurvatureMulti",this->GetCLIReconstructionPropertyNode()->GetCrestCurvatureValueMulti());
    reconstructionNodeCLI->SetParameterAsBool("fineTuning",true);
}

void vtkSlicerVirtualFractureReconstructionLogic::UpdateStep1Parameters(vtkSmartPointer<vtkMRMLCommandLineModuleNode> reconstructionNodeCLI,vtkMRMLVirtualFractureReconstructionNode *reconstructionNode)
{
    const char* tempPath = qSlicerCoreApplication::application()->temporaryPath().toUtf8().constData();
    reconstructionNodeCLI->SetParameterAsString("slicerTempPath",tempPath);
    qDebug()<<"SLICER TEMP LOGIG"<<reconstructionNodeCLI->GetParameterAsString("slicerTempPath").c_str();

    reconstructionNodeCLI->SetParameterAsString("referenceLabelMap", reconstructionNode->GetReferenceLabelmapNodeID());

    reconstructionNodeCLI->SetParameterAsString("fragmentLabelMap", reconstructionNode->GetFragmentLabelmapNodeID());

    reconstructionNodeCLI->SetParameterAsString("referenceImage", reconstructionNode->GetReferenceImageNodeID());

    reconstructionNodeCLI->SetParameterAsString("fragmentImage", reconstructionNode->GetFragmentImageNodeID());

    reconstructionNodeCLI->SetParameterAsString("fragmentModelOutput",this->CLIReconstructionPropertyNode->GetModifiedCandidatePolyDataID());

    if(this->CLIReconstructionPropertyNode->GetInputReferenceModelID())
        reconstructionNodeCLI->SetParameterAsString("referenceModelInput",this->CLIReconstructionPropertyNode->GetInputReferenceModelID());

    reconstructionNodeCLI->SetParameterAsString("referenceModelOutput",this->CLIReconstructionPropertyNode->GetOutputReferencePolyDataID());

    //reconstructionNodeCLI->SetParameterAsString("outputFragment", this->CLIReconstructionPropertyNode->GetOutputLabelMapNodeID());

    reconstructionNodeCLI->SetParameterAsString("outputTransform",reconstructionNode->GetOutputTransformNodeID());

    reconstructionNodeCLI->SetParameterAsString("inputTransform",reconstructionNode->GetCurrentInitialTransformNodeID());

    reconstructionNodeCLI->SetParameterAsBool("useCoordinates",this->CLIReconstructionPropertyNode->GetUseCoordinates());

    reconstructionNodeCLI->SetParameterAsBool("useCurvature",this->CLIReconstructionPropertyNode->GetUseCurvature());

    reconstructionNodeCLI->SetParameterAsBool("useTexture",this->CLIReconstructionPropertyNode->GetUseTexture());

    reconstructionNodeCLI->SetParameterAsBool("useNormals",this->CLIReconstructionPropertyNode->GetUseNormals());

    reconstructionNodeCLI->SetParameterAsFloat("decimationFactor",this->CLIReconstructionPropertyNode->GetDecimationFactor());

    reconstructionNodeCLI->SetParameterAsFloat("pointPercentageEMICP",this->CLIReconstructionPropertyNode->GetEMPointPercentage());

    reconstructionNodeCLI->SetParameterAsInt("histogramBins",this->CLIReconstructionPropertyNode->GetNumberOfHistogramBins());

    reconstructionNodeCLI->SetParameterAsFloat("sigmaInf",this->CLIReconstructionPropertyNode->GetSigmaInf());

    reconstructionNodeCLI->SetParameterAsFloat("sigmaFactor",this->CLIReconstructionPropertyNode->GetSigmaFactor());

    reconstructionNodeCLI->SetParameterAsBool("fineTuning",this->CLIReconstructionPropertyNode->GetFineTuningOnly());

    reconstructionNodeCLI->SetParameterAsFloat("cylinderLength",this->CLIReconstructionPropertyNode->GetCylinderLength());

    reconstructionNodeCLI->SetParameterAsFloat("cylinderRadius",this->CLIReconstructionPropertyNode->GetCylinderRadius());

}

