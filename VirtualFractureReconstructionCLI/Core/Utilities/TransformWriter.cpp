/*
 * TransformWriter.cpp
 *
 *  Created on: 28.11.2011
 *      Author: kfritscher
 */
#include "TransformWriter.h"


void TransformWriter::WriteTransform(SPTransformType::Pointer transform,std::string name)
{
  GroupType::Pointer grp = GroupType::New();
  SPWriterType::Pointer spwriter = SPWriterType::New();
  SPTransformType::Pointer sptransform = SPTransformType::New();

  sptransform->SetCenter(transform->GetCenter());
  sptransform->SetMatrix(transform->GetMatrix());
  sptransform->SetOffset(transform->GetOffset());
  grp->SetObjectToParentTransform(sptransform.GetPointer());

  char transformName[300];
  sprintf(transformName, "%s.tfm", name.c_str());
  spwriter->SetFileName(transformName);
  spwriter->SetInput(grp);
  spwriter->Update();
}

vtkPolyData* TransformWriter::TransformPolyData(vtkSmartPointer<vtkPolyData> polyData, ITKTransformPointer transform, bool invert) {

        ITKMatrixType itkMatrix = transform->GetMatrix();

        ITKTransformOffsetType itkOffset = transform->GetOffset();

        vtkSmartPointer<vtkTransform> vtkTrans =  vtkSmartPointer<vtkTransform>::New();

        for (unsigned int i = 0; i < 3; i++) {
                for (unsigned int j = 0; j < 3; j++)
                {
                        vtkTrans->GetMatrix()->SetElement(i, j,itkMatrix.GetVnlMatrix().get(i, j));
                }
                vtkTrans->GetMatrix()->SetElement(i, 3, itkOffset[i]);
        }
        std::cout<<"Starting transform filter"<<std::endl;
        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
                        vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter->SetInput(polyData);
        if(invert) vtkTrans->Inverse();
        transformFilter->SetTransform(vtkTrans);
        transformFilter->Update();

        std::cout<<"Polydata transformed"<<std::endl;
        polyData->DeepCopy(transformFilter->GetOutput());
        return transformFilter->GetOutput();
}
