#include "ActivePointSelector.h"



#include "vtkPointData.h"

#include "vtkWeakPointer.h"

ActivePointSelector::ActivePointSelector()
{
    m_Locator = vtkSmartPointer<vtkPointLocator>::New();
    this->m_Radius=2;
}

ActivePointSelector::~ActivePointSelector()
{

}

void ActivePointSelector::SetReferencePolyData(vtkPolyData *referencePolyData, bool closestPoints, int inactiveValue)
{
    this->m_ReferencePolyData=referencePolyData;

    m_Locator = vtkSmartPointer<vtkPointLocator>::New();

    m_Locator->SetDataSet(this->m_ReferencePolyData);
    m_Locator->BuildLocator();
    m_Locator->Update();

    if (closestPoints)
    {
         if(!referencePolyData->GetPointData()->GetArray("ClosestActivePoints"))
        {
            m_ClosestActivePoints=vtkSmartPointer<vtkIntArray>::New();
            m_ClosestActivePoints->SetName("ClosestActivePoints");
            m_ClosestActivePoints->SetNumberOfComponents(1);
            m_ClosestActivePoints->SetNumberOfTuples(referencePolyData->GetNumberOfPoints());
            m_ClosestActivePoints->FillComponent(0,inactiveValue);
            referencePolyData->GetPointData()->AddArray(m_ClosestActivePoints);
        }
        else
         m_ClosestActivePoints=vtkIntArray::SafeDownCast(referencePolyData->GetPointData()->GetArray("ClosestActivePoints"));
    }

}


void ActivePointSelector::MarkInactivePoints(vtkPolyData* candidatePolyData)
{
    if(!this->m_ReferencePolyData)
    {
        std::cerr<<"No reference polydata set"<<std::endl;
        return;
    }
    vtkWeakPointer<vtkDataArray> surf=this->m_ReferencePolyData->GetPointData()->GetArray("SurfacePointsClassifier");

    if(!candidatePolyData->GetPointData()->GetArray("ActivePoints"))
    {
        m_ActivePoints=vtkSmartPointer<vtkIntArray>::New();
        m_ActivePoints->SetName("ActivePoints");
        m_ActivePoints->SetNumberOfComponents(1);
        m_ActivePoints->SetNumberOfTuples(candidatePolyData->GetNumberOfPoints());
        m_ActivePoints->FillComponent(0,1);
        candidatePolyData->GetPointData()->AddArray(m_ActivePoints);
    }
    else
     m_ActivePoints=vtkIntArray::SafeDownCast(candidatePolyData->GetPointData()->GetArray("ActivePoints"));

    if(!surf)
    {
        std::cout<<"No SurfaceClassifier array available"<<std::endl;
        return;
    }
    vtkIdType closestPointID=0;
    double pointCoord[3];

    double dist=0;
    for (unsigned int point=0;point<candidatePolyData->GetNumberOfPoints();point++)
    {
      candidatePolyData->GetPoint(point,pointCoord);
      closestPointID = this->m_Locator->FindClosestPointWithinRadius(this->m_Radius,pointCoord, dist);
      if(closestPointID>-1)// &&surf->GetTuple1(closestPointID)>0)
      {
        this->m_ActivePoints->SetTuple1(point,0);
      }
    }
}


unsigned int ActivePointSelector::GetPointsWithinRadius(vtkPolyData* candidatePolyData,std::string relevantArrayName,float radius,int activeValue)
{
    vtkWeakPointer<vtkIntArray>  relevantArray;
    //vtkWeakPointer<vtkIntArray>  activePointsArray;
    if(!candidatePolyData->GetPointData()->GetArray("ActivePoints"))
    {
        m_ActivePoints=vtkSmartPointer<vtkIntArray>::New();
        m_ActivePoints->SetName("ActivePoints");
        m_ActivePoints->SetNumberOfComponents(1);
        m_ActivePoints->SetNumberOfTuples(candidatePolyData->GetNumberOfPoints());
        candidatePolyData->GetPointData()->AddArray(m_ActivePoints);
    }
    else
     m_ActivePoints=vtkIntArray::SafeDownCast(candidatePolyData->GetPointData()->GetArray("ActivePoints"));

    m_ActivePoints->FillComponent(0,abs(activeValue-1));
    if(!candidatePolyData->GetPointData()->GetArray(relevantArrayName.c_str()))
    {
        std::cerr<<"Relevant array not available."<<std::endl;
        return 0;
    }
    else
     relevantArray=vtkIntArray::SafeDownCast(candidatePolyData->GetPointData()->GetArray(relevantArrayName.c_str()));
    unsigned int activePointsCounter=0;
    vtkIdType closestPointID=0;
    double pointCoord[3];

    double dist=0;
    vtkWeakPointer<vtkIntArray> activeRef=vtkIntArray::SafeDownCast(this->m_ReferencePolyData->GetPointData()->GetArray("ActivePoints"));
    for (unsigned int point=0;point<candidatePolyData->GetNumberOfPoints();point++)
    {
        if(relevantArray->GetTuple1(point)!=0)
        {
          candidatePolyData->GetPoint(point,pointCoord);
          closestPointID = this->m_Locator->FindClosestPointWithinRadius(radius,pointCoord, dist);
          if(closestPointID>-1 && activeRef->GetTuple1(closestPointID)>0 &&m_ClosestActivePoints->GetTuple1(closestPointID)==0)
          {
            m_ActivePoints->SetTuple1(point,activeValue);
            activePointsCounter++;
            m_ClosestActivePoints->SetTuple1(closestPointID,activeValue);
          }
          //else
           //  activePointsArray->SetTuple1(point,0);  //else branch necessary?
        }
    }
    return activePointsCounter;
}
