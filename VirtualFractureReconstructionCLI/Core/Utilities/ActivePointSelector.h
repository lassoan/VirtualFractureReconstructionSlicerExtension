#ifndef ACTIVEPOINTSELECTOR_H
#define ACTIVEPOINTSELECTOR_H

#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkPointLocator.h"

class ActivePointSelector
{
public:
    void SetReferencePolyData(vtkPolyData *referencePolyData, bool closestPoints=true,int inactiveValue=0);

    void MarkInactivePoints(vtkPolyData* candidatePolyData);

    void SetSelectionRadius(float radius)
    {
        this->m_Radius=radius;
    }
    vtkSmartPointer<vtkIntArray> GetActivePointsArray()
    {
        return this->m_ActivePoints;
    }

    unsigned int GetPointsWithinRadius(vtkPolyData* candidatePolyData, std::string relevantArrayName,  float radius=5, int activeValue=1);

    ActivePointSelector();
    ~ActivePointSelector();

protected:
   vtkSmartPointer<vtkPolyData> m_ReferencePolyData;
   vtkSmartPointer<vtkIntArray> m_ActivePoints;
   vtkSmartPointer<vtkIntArray> m_ClosestActivePoints;
   vtkSmartPointer<vtkPointLocator> m_Locator;

   float m_Radius;
};

#endif // ACTIVEPOINTSELECTOR_H
