#include "vtkComputeLocalGeometry.h"
#include "MeshOps.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCellLocator.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTriangleFilter.h"
#include "vtkSmartPointer.h"

#include <valarray>
#include <vector>
#include <list>

// DEBUG_DISPLAY_1:
void Debug_Display( vtkIntArray* activeContourVertIdx, 
                          const std::vector<int>& Lstar) 
{
      int numVerts = Lstar.size();
      for( int i = 0; i < numVerts; i++ ) {
        activeContourVertIdx->SetValue(i, Lstar[i]);
      }
}

vtkStandardNewMacro(vtkComputeLocalGeometry);

vtkComputeLocalGeometry::vtkComputeLocalGeometry() {
 // this->SetNumberOfInputPorts(2);
 // this->SetNumberOfOutputPorts(1);
}

void vtkComputeLocalGeometry::SetSource(vtkPolyData *source) {
  this->SetInputData(1, source);
}

vtkPolyData *vtkComputeLocalGeometry::GetSource() {
  if (this->GetNumberOfInputConnections(1) < 1) {
    return NULL;
  }
  return vtkPolyData::SafeDownCast(
    this->GetExecutive()->GetInputData(1, 0));
}

// Where the bulk of work is done!
int vtkComputeLocalGeometry::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector) {
   
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
 // vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  
  // update progress bar at some intervals
  this->UpdateProgress(0.15);

    // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
//  vtkPolyData *source = 0;
 /* if (sourceInfo)
    {
    source = vtkPolyData::SafeDownCast(
      sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
    }*/
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Check input
  //
  size_t numVerts=input->GetNumberOfPoints();
  size_t numCells=input->GetNumberOfCells();
  if (numVerts < 1 || numCells < 1)
  {
    vtkErrorMacro(<<"No data to smooth!");
    return 1;
  }
  output->CopyStructure(input);
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());
  vtkDebugMacro(<<"Analyzing ____ ...");
   


// assign some data from curvature computation 
// check for the prescence of this named array!
//  vtkDataArray* surfaceCurvature = input->GetPointData()->GetArray("SurfaceMeanCurvature");
  if( 1 /*NULL == surfaceCurvature*/ ) {
    vtkFloatArray* surfaceCurvature = vtkFloatArray::New();
    sC= new float[numVerts];
    surfaceCurvature->SetName("SurfaceMeanCurvature");
    surfaceCurvature->SetNumberOfComponents(1);
    surfaceCurvature->SetNumberOfTuples( numVerts );
    std::valarray<double> surfH(numVerts);
    
    this->myMeshData = new MeshDataCustom;
    { // compute the mean curvature using K-neighborhood paraboloid-fitting

      // do the curvature computations / pre-processing
      myMeshData->polydata = output;
      myMeshData->smoothH_its = H_smooth_its;
      myMeshData->adj_levels = adj_levels;
      myMeshData->showLS = 0;
      myMeshData->rightHandMesh = rightHandMesh;
      ComputeCurvatureData( myMeshData );
      surfH = myMeshData->MeanCurv;
    }


    double scale=1;
    { // perform the assignment to scalar array attached to vtkPolyData
      for( ::size_t i = 0; i < numVerts; i++ ) {
        surfH[i] = scale * surfH[i];
        surfaceCurvature->SetTuple1(i,surfH[i]);
        sC[i]=surfH[i];
      }
      output->GetPointData()->AddArray(surfaceCurvature);
      output->GetPointData()->SetScalars( surfaceCurvature );
      //output->GetPointData()->SetActiveScalars( surfaceCurvature->GetName() );
    }

  }

  // update progress bar at some intervals
  this->UpdateProgress(1.0);
  return 1;
 }
 
 // what does this do, and is it necessary?
int vtkComputeLocalGeometry::FillInputPortInformation(int port,
                                                      vtkInformation *info)
{
  if (!this->Superclass::FillInputPortInformation(port, info))
    {
    return 0;
    }
  
  if (port == 1)
    {
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    }
  return 1;
}


void vtkComputeLocalGeometry::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Truth: " << 42 << std::endl;
  if ( this->GetSource() )
    {
      os << indent << "Source: " << static_cast<void *>(this->GetSource()) << std::endl;
    }
  else
    {
    os << indent << "Source (none)\n";
    }
}
