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

#ifndef __vtkMRMLCLIFractureReconstructionPropertyNode_h
#define __vtkMRMLCLIFractureReconstructionPropertyNode_h

#include "qdebug.h"

#include "vtkMRML.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLColorLogic.h"

#include "vtkCollection.h"
#include "vtkSmartPointer.h"



#include "vtkSlicerVirtualFractureReconstructionModuleMRMLExport.h"

/// \ingroup Slicer_QtModules_VirtualFractureReconstruction
class Q_SLICER_QTMODULES_VIRTUALFRACTURERECONSTRUCTION_EXPORT vtkMRMLCLIFractureReconstructionPropertyNode : public vtkMRMLNode
{

  public:

    vtkSetMacro(UseICP,bool);
    vtkGetMacro(UseICP,bool);

    vtkSetMacro(UseCoordinates,bool);
    vtkGetMacro(UseCoordinates,bool);

    vtkSetMacro(UseCurvature,bool);
    vtkGetMacro(UseCurvature,bool);

    vtkSetMacro(UseNormals,bool);
    vtkGetMacro(UseNormals,bool);

    vtkSetMacro(UseTexture,bool);
    vtkGetMacro(UseTexture,bool);


    vtkSetMacro(StartAdjLevel,unsigned int);
    vtkGetMacro(StartAdjLevel,unsigned int);

    vtkSetMacro(NumberOfAdjLevels,unsigned int);
    vtkGetMacro(NumberOfAdjLevels,unsigned int);

    vtkSetMacro(MaxDist,float);
    vtkGetMacro(MaxDist,float);

    vtkSetMacro(UseReferenceModel,bool);
    vtkGetMacro(UseReferenceModel,bool);

    vtkSetMacro(FineTuningOnly,bool);
    vtkGetMacro(FineTuningOnly,bool);

    vtkSetStringMacro(InputReferenceModelID);
    vtkGetStringMacro(InputReferenceModelID);

    vtkSetStringMacro(InputReferenceFileName);
    vtkGetStringMacro(InputReferenceFileName);

    vtkSetStringMacro(OutputReferencePolyDataID);
    vtkGetStringMacro(OutputReferencePolyDataID);

    vtkSetStringMacro(ModifiedCandidatePolyDataID);
    vtkGetStringMacro(ModifiedCandidatePolyDataID);

    vtkSetStringMacro(ModifiedCandidatePolyDataFileName);
    vtkGetStringMacro(ModifiedCandidatePolyDataFileName);

    vtkSetStringMacro(CommaSeparatedModelFileNames);
    vtkGetStringMacro(CommaSeparatedModelFileNames);

    vtkSetStringMacro(CommaSeparatedTransformIDs);
    vtkGetStringMacro(CommaSeparatedTransformIDs);

    vtkSetStringMacro(CommaSeparatedInputTransformIDs);
    vtkGetStringMacro(CommaSeparatedInputTransformIDs);

    vtkSetStringMacro(OutputLabelMapNodeID);
    vtkGetStringMacro(OutputLabelMapNodeID);

    vtkSetStringMacro(TempPath);
    vtkGetStringMacro(TempPath);

    vtkSetMacro(SigmaInf,float);
    vtkGetMacro(SigmaInf,float);

    vtkSetMacro(SigmaFactor,float);
    vtkGetMacro(SigmaFactor,float);

    vtkSetMacro(EMPointPercentage,float);
    vtkGetMacro(EMPointPercentage,float);

    vtkSetMacro(DecimationFactor,float);
    vtkGetMacro(DecimationFactor,float);

    vtkSetMacro(NumberOfHistogramBins,float);
    vtkGetMacro(NumberOfHistogramBins,float);

    vtkSetMacro(CylinderLength,float);
    vtkGetMacro(CylinderLength,float);

    vtkSetMacro(CylinderRadius,float);
    vtkGetMacro(CylinderRadius,float);

    vtkSetMacro(MaxSmoothingIts, unsigned int);
    vtkGetMacro(MaxSmoothingIts, unsigned int);

    vtkSetMacro(MaxICPIterations,unsigned int);
    vtkGetMacro(MaxICPIterations,unsigned int);


    vtkSetMacro(StepSize, unsigned int);
    vtkGetMacro(StepSize, unsigned int);

    vtkSetMacro(MaxPointDistanceMulti,float);
    vtkGetMacro(MaxPointDistanceMulti,float);

    vtkSetMacro(CrestCurvatureValueMulti,float);
    vtkGetMacro(CrestCurvatureValueMulti,float);


    static vtkMRMLCLIFractureReconstructionPropertyNode *New();
    vtkTypeMacro(vtkMRMLCLIFractureReconstructionPropertyNode,vtkMRMLNode);
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


protected:
    vtkMRMLCLIFractureReconstructionPropertyNode();
    ~vtkMRMLCLIFractureReconstructionPropertyNode();

    vtkMRMLCLIFractureReconstructionPropertyNode(const vtkMRMLCLIFractureReconstructionPropertyNode&);
    void operator=(const vtkMRMLCLIFractureReconstructionPropertyNode&);

    bool UseICP;
    bool UseCoordinates;
    bool UseNormals;
    bool UseCurvature;
    bool UseTexture;

    unsigned int StartAdjLevel;
    unsigned int NumberOfAdjLevels;
    unsigned int MaxSmoothingIts;
    unsigned int NumberOfHistogramBins;
    unsigned int MaxICPIterations;

    float MaxDist;
    unsigned int StepSize;
    bool UseReferenceModel;
    bool FineTuningOnly;
    char* InputReferenceFileName;
    char* InputReferenceModelID;
    char* OutputReferencePolyDataID;
    char* ModifiedCandidatePolyDataID;
    char* ModifiedCandidatePolyDataFileName;
    char* OutputLabelMapNodeID;
    char* CommaSeparatedModelFileNames;
    char* CommaSeparatedTransformIDs;
    char* CommaSeparatedInputTransformIDs;
    char* TempPath;

    float SigmaInf;
    float SigmaFactor;

    float CylinderRadius;
    float CylinderLength;

    float EMPointPercentage;
    float DecimationFactor;

    float MaxPointDistanceMulti;
    float CrestCurvatureValueMulti;




private:


};

#endif

