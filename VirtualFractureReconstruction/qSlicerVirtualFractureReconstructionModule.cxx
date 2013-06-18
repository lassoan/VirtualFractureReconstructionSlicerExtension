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
// Slicer includes
#include <qSlicerCoreApplication.h>
#include <qSlicerModuleManager.h>

// Qt includes
#include <QtPlugin>
#include <QDebug>

// VirtualFractureReconstruction Logic includes
#include <vtkSlicerCLIModuleLogic.h>
#include <vtkMRMLColorLogic.h>
#include <vtkSlicerVirtualFractureReconstructionLogic.h>

// VirtualFractureReconstruction includes
#include "qSlicerVirtualFractureReconstructionModule.h"
#include "qSlicerVirtualFractureReconstructionModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerVirtualFractureReconstructionModule, qSlicerVirtualFractureReconstructionModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_VirtualFractureReconstruction
class qSlicerVirtualFractureReconstructionModulePrivate
{
public:
  qSlicerVirtualFractureReconstructionModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVirtualFractureReconstructionModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerVirtualFractureReconstructionModulePrivate::qSlicerVirtualFractureReconstructionModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVirtualFractureReconstructionModule methods

//-----------------------------------------------------------------------------
qSlicerVirtualFractureReconstructionModule::qSlicerVirtualFractureReconstructionModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVirtualFractureReconstructionModulePrivate)
{
    //this->reconstructionNode = NULL;
}

//-----------------------------------------------------------------------------
QStringList qSlicerVirtualFractureReconstructionModule::categories()const
{
  return QStringList() << "Developer Tools";
}

//-----------------------------------------------------------------------------
qSlicerVirtualFractureReconstructionModule::~qSlicerVirtualFractureReconstructionModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerVirtualFractureReconstructionModule::helpText()const
{
  QString help = 
    "This template module is meant to be used with the"
    "with the ModuleWizard.py script distributed with the"
    "Slicer source code (starting with version 4)."
    "Developers can generate their own source code using the"
    "wizard and then customize it to fit their needs.";
  return help;
}

//-----------------------------------------------------------------------------
QString qSlicerVirtualFractureReconstructionModule::acknowledgementText()const
{
  return "This work was supported by NAMIC, NAC, and the Slicer Community...";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVirtualFractureReconstructionModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (Organization)");
  // moduleContributors << QString("Richard Roe (Organization2)");
  // ...
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerVirtualFractureReconstructionModule::icon()const
{
  return QIcon(":/Icons/VirtualFractureReconstruction.png");
}

//-----------------------------------------------------------------------------
void qSlicerVirtualFractureReconstructionModule::setup()
{
  this->Superclass::setup();
    vtkSlicerVirtualFractureReconstructionLogic* fractureReconstructionLogic =
      vtkSlicerVirtualFractureReconstructionLogic::SafeDownCast(this->logic());


    qSlicerAbstractCoreModule* modelMakerModule =
      qSlicerCoreApplication::application()->moduleManager()->module("ModelMaker");
    if (modelMakerModule)
      {
      vtkSlicerCLIModuleLogic* modelMakerLogic =
        vtkSlicerCLIModuleLogic::SafeDownCast(modelMakerModule->logic());
      fractureReconstructionLogic->SetModelMakerLogic(modelMakerLogic );
      }
    else
      {
       qWarning() << "Model maker module not found";
      }


    qSlicerAbstractCoreModule* colorsModule =
      qSlicerCoreApplication::application()->moduleManager()->module("Colors");
    if (colorsModule)
      {
         vtkMRMLColorLogic* colorLogic =
           vtkMRMLColorLogic::SafeDownCast(colorsModule->logic());
         fractureReconstructionLogic->SetColorLogic(colorLogic);
      }
    else
      {
       qWarning() << "Colors module not found";
      }

    qSlicerAbstractCoreModule* fractureReconstructionModule =
      qSlicerCoreApplication::application()->moduleManager()->module("FractureReconstructionCLI");
    if (fractureReconstructionModule)
      {
         vtkSlicerCLIModuleLogic* fractureReconstructionCLILogic =
           vtkSlicerCLIModuleLogic::SafeDownCast(fractureReconstructionModule->logic());
         fractureReconstructionLogic->SetCLIReconstructionLogic(fractureReconstructionCLILogic);
         vtkSmartPointer<vtkMRMLCLIFractureReconstructionPropertyNode> CLIReconstructionPropertyNode = vtkSmartPointer<vtkMRMLCLIFractureReconstructionPropertyNode>::New();
         fractureReconstructionLogic->SetCLIReconstructionPropertyNode(CLIReconstructionPropertyNode);
      }
    else
      {
       qWarning() << "Fracture reconstruction CLI module not found";
      }
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerVirtualFractureReconstructionModule::createWidgetRepresentation()
{
  return new qSlicerVirtualFractureReconstructionModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerVirtualFractureReconstructionModule::createLogic()
{
  return vtkSlicerVirtualFractureReconstructionLogic::New();
}
