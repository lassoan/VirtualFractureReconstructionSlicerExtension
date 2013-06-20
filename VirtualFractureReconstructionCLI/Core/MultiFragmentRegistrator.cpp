#include "MultiFragmentRegistrator.h"

#include "TransformWriter.h"

#include "FileOutputWriter.h"

MultiFragmentRegistrator::MultiFragmentRegistrator()
{
    m_ActiveReferencePointSet=vtkSmartPointer<vtkPoints>::New();
    m_ActiveCandidatePointSet=vtkSmartPointer<vtkPoints>::New();

    m_ActiveReferencePolyData=vtkSmartPointer<vtkPolyData>::New();
    m_ActiveCandidatePolyData=vtkSmartPointer<vtkPolyData>::New();
    m_CurrentCombinedEnsemble=vtkSmartPointer<vtkPolyData>::New();
    m_GeometricPropCalculator = vtkComputeLocalGeometry::New();

    m_ActiveTransform = ITKTransformType::New();

    m_Ini=new MemIni();
    m_DefaultIniName="_Settings.ini";



    m_EMICP=new ibia::ICPWrapper();
    m_ClosestPointFinder= new ActivePointSelector();
    this->m_RelevantPointsArrayName="CrestPoints";

    m_MaxPointDistanceToRef=5;
    m_MaxPointDistanceToExtCand=5;

    m_NumberOfFragmentsInActiveSet=1;
    m_CrestCurvatureMaxVal=-0.05;
    m_RefineCrest=true;

    this->Initialize();
}

MultiFragmentRegistrator::~MultiFragmentRegistrator()
{
    this->m_FinalTransformContainer.clear();
}


void MultiFragmentRegistrator::Initialize(){

    //this->m_ActiveReferencePolyData->SetPoints(this->m_ActiveReferencePointSet);
    //this->m_ActiveCandidatePolyData->SetPoints(this->m_ActiveCandidatePointSet);
    this->m_Ini->SetFileName(this->m_DefaultIniName);
}

void MultiFragmentRegistrator::StartMultiFragmentAlignment(){
    this->m_Ini->LoadValues();


    this->m_FinalTransformContainer.clear();

    std::string outputFolder = this->m_Ini->ReadString("Multi","FragmentOutputFolder");
    m_MaxPointDistanceToRef=m_Ini->ReadValue<float>("Multi","MaxPointDistanceMulti",5);
    m_MaxPointDistanceToExtCand=m_Ini->ReadValue<float>("Multi","MaxPointDistanceMulti",5);
    m_CrestCurvatureMaxVal=m_Ini->ReadValue<float>("Multi","MaxCrestCurvatureMulti",-0.1);

    this->InitializeActiveReferencePointSet();

    for(unsigned int f=1;f<this->m_FragmentContainer.size();f++)
    {
        this->IdentifyClosestInactiveFragment();

        int activeCounter=0;
        for (unsigned int p=0;p<this->m_ActiveCandidatePolyData->GetNumberOfPoints();p++)
        {
            if(this->m_ActiveCandidatePolyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(p)==1)
            {
                this->m_ActiveCandidatePointSet->InsertNextPoint(this->m_ActiveCandidatePolyData->GetPoint(p));
                activeCounter++;
            }
        }

        this->ExtractActiveCandidatePoints();

        this->m_ActiveReferencePointSet=this->ExtractClosestNActivePoints(m_ActiveReferencePolyData);

        this->m_EMICP->SetRegistrationParameters(this->GetEMICPParameters());
        this->m_EMICP->SetViewer(0);

        this->m_EMICP->doRegistration(m_ActiveReferencePointSet,m_ActiveCandidatePointSet,100,100);

        m_ActiveTransform->SetCenter(m_EMICP->GetFinalTransform()->GetCenter());
        m_ActiveTransform->SetMatrix(m_EMICP->GetFinalTransform()->GetMatrix());
        m_ActiveTransform->SetOffset(m_EMICP->GetFinalTransform()->GetOffset());

        vtkSmartPointer<vtkPolyDataWriter> polywriter = vtkSmartPointer< vtkPolyDataWriter>::New();

        char temp[200];
        sprintf(temp,"CandidatePolyBeforeReg%i.vtk",f);
        polywriter->SetFileName(temp);
        polywriter->SetInput(m_ActiveCandidatePolyData);
        polywriter->Update();

        for (unsigned int frag=1;frag<this->m_FragmentContainer.size();frag++)
        {
            if(this->m_FragmentContainer[frag].second==0)
                TransformWriter::TransformPolyData(this->m_FragmentContainer[frag].first,m_ActiveTransform,false);

            sprintf(temp,"CandidatePoly%iAfterReg_Iter%i.vtk",f,frag);
            polywriter->SetFileName(FileOutputWriter::ComposeFilename(outputFolder,temp).c_str());
            polywriter->SetInput(m_ActiveCandidatePolyData);
            polywriter->Update();
        }
        this->m_FinalTransformContainer.push_back(m_ActiveTransform);


        sprintf(temp,"ReferencePoly%i.vtk",f);
        polywriter->SetFileName(FileOutputWriter::ComposeFilename(outputFolder,temp).c_str());
        polywriter->SetInput(m_ActiveReferencePolyData);
        polywriter->Update();

        this->UpdateActiveReferencePointSet();

        sprintf(temp,"ReferencePoly%iAfterUpdate.vtk",f);
        polywriter->SetFileName(FileOutputWriter::ComposeFilename(outputFolder,temp).c_str());
        polywriter->SetInput(m_ActiveReferencePolyData);
        polywriter->Update();

        sprintf(temp,"Fragment%iAfterMultiReg.vtk",f);
        polywriter->SetFileName(FileOutputWriter::ComposeFilename(outputFolder,temp).c_str());
        polywriter->SetInput(m_ActiveCandidatePolyData);
        polywriter->Update();

        this->m_FragmentContainer[this->m_CurrentlyActiveFragment].second=1;
    }
}

void MultiFragmentRegistrator::RefineCrestPointSelection(vtkSmartPointer<vtkPolyData> fragment)
{
    m_GeometricPropCalculator = vtkComputeLocalGeometry::New();
    m_GeometricPropCalculator->SetInput(fragment);
    m_GeometricPropCalculator->Set_adj_levels(1);
    m_GeometricPropCalculator->Set_H_smooth_its(5);
    m_GeometricPropCalculator->Update();

    fragment->GetPointData()->GetArray("CrestPoints")->FillComponent(0,0);

    for (unsigned int p=0;p<fragment->GetNumberOfPoints();p++)
    {
        if(this->m_GeometricPropCalculator->GetOutput()->GetPointData()->GetArray("SurfaceMeanCurvature")->GetTuple1(p)<this->m_CrestCurvatureMaxVal)
                //&&fragment->GetPointData()->GetArray("SurfacePointsClassifier")->GetTuple1(p)>0)
            fragment->GetPointData()->GetArray("CrestPoints")->SetTuple1(p,1);
    }
 }


registrationParameters MultiFragmentRegistrator::GetEMICPParameters()
{
    //Currently not in use
    registrationParameters para;
    para.sigma_p2=m_Ini->ReadValue<float>("EM-ICP-Multi","sigma_p2_ini",3);
    para.sigma_inf=m_Ini->ReadValue<float>("EM-ICP-Multi","sigma_inf_ini",0.1);
    para.sigma_factor=m_Ini->ReadValue<float>("EM-ICP-Multi","sigma_factor_ini",0.99);
    para.d_02=m_Ini->ReadValue<float>("EM-ICP-Multi","d_02_ini",0.01);
    //para.noviewer=!(m_Ini->ReadValue<int>("EM-ICP","UseViewer",1)>0);
    return para;
}




void MultiFragmentRegistrator::AddFragment(vtkPolyData* fragment)
{
    if(!fragment->GetPointData()->GetArray(this->m_RelevantPointsArrayName.c_str()))
    {
        std::cerr<<"No relevant points array available. Polydata will not be added to fragment container"<<std::endl;
        return;
    }

    if(this->m_RefineCrest)
        this->RefineCrestPointSelection(fragment);

    vtkSmartPointer<vtkIntArray> activePointsArray;
    //if(!fragment->GetPointData()->GetArray("ActivePoints"))
    //{
        activePointsArray=vtkSmartPointer<vtkIntArray>::New();
        activePointsArray->DeepCopy(fragment->GetPointData()->GetArray(this->m_RelevantPointsArrayName.c_str()));
        activePointsArray->SetName("ActivePoints");
        fragment->GetPointData()->AddArray(activePointsArray);
    //}
    //else
     //   activePointsArray=NULL;


    std::pair<vtkPolyData*,unsigned int> newFragment(fragment,0);
    this->m_FragmentContainer.push_back(newFragment);


}


void MultiFragmentRegistrator::AddFragment(vtkPolyData* fragment,ITKTransformPointer transform)
{
    ITKMatrixType itkMatrix = transform->GetMatrix();

    ITKTransformOffsetType itkOffset = transform->GetOffset();

    vtkSmartPointer<vtkTransform> vtkTrans =  vtkSmartPointer<vtkTransform>::New();

    for (unsigned int i = 0; i < 3; i++) {
            for (unsigned int j = 0; j < 3; j++) {
                    vtkTrans->GetMatrix()->SetElement(i, j,itkMatrix.GetVnlMatrix().get(i, j));
            }
            vtkTrans->GetMatrix()->SetElement(i, 3, itkOffset[i]);
    }

    std::cout<<"Starting transform filter"<<std::endl;
    vtkTransformPolyDataFilter* transformFilter =
                    vtkTransformPolyDataFilter::New();
    transformFilter->SetInput(fragment);
    //vtkTrans->Inverse();
    transformFilter->SetTransform(vtkTrans);
    transformFilter->Update();
    this->AddFragment(transformFilter->GetOutput());
}



vtkPoints* MultiFragmentRegistrator::InitializeActiveReferencePointSet()
{
    this->m_ActiveReferencePolyData=m_FragmentContainer[0].first;
    this->m_NumberOfFragmentsInActiveSet++;
    m_FragmentContainer[0].second=1;
    //m_ActiveReferencePointSet->DeepCopy(this->m_ActiveReferencePolyData->GetPoints());
    return this->m_ActiveReferencePolyData->GetPoints();
}


vtkPoints* MultiFragmentRegistrator::UpdateActiveReferencePointSet(){

    vtkSmartPointer<vtkIntArray> tempCrest = vtkSmartPointer<vtkIntArray>::New();
    vtkSmartPointer<vtkIntArray> tempActive= vtkSmartPointer<vtkIntArray>::New();
    vtkSmartPointer<vtkPoints> tempPoints = vtkSmartPointer<vtkPoints>::New();

    tempActive->DeepCopy(this->m_ActiveReferencePolyData->GetPointData()->GetArray("ActivePoints"));
    tempActive->SetName("ActivePoints");
    tempCrest->DeepCopy(this->m_ActiveReferencePolyData->GetPointData()->GetArray("CrestPoints"));
    tempCrest->SetName("CrestPoints");
    tempPoints->DeepCopy(this->m_ActiveReferencePolyData->GetPoints());
/*
    for (int p=0;p<this->m_ActiveReferencePolyData->GetNumberOfPoints();p++)
    {
        if( this->m_ActiveReferencePolyData->GetPointData()->GetArray("ClosestActivePoints")->GetTuple1(p)==1)
        {
             tempActive->SetTuple1(p,0);
             tempCrest->SetTuple1(p,0);
        }
    }
*/
    for (int p=0;p<this->m_ActiveCandidatePolyData->GetNumberOfPoints();p++)
    {
        if(this->m_ActiveCandidatePolyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(p)==0
           && this->m_ActiveCandidatePolyData->GetPointData()->GetArray("CrestPoints")->GetTuple1(p)==1 )
        {
            double newPoint[3];
            this->m_ActiveCandidatePolyData->GetPoint(p,newPoint);
            tempPoints->InsertNextPoint(newPoint);
            tempActive->InsertNextTuple1(1);
            tempCrest->InsertNextTuple1(1);
        }
    }
    this->m_ActiveReferencePolyData= vtkSmartPointer<vtkPolyData>::New();
    this->m_ActiveReferencePolyData->Initialize();
    this->m_ActiveReferencePolyData->SetPoints(tempPoints);
    this->m_ActiveReferencePolyData->GetPointData()->AddArray(tempCrest);
    this->m_ActiveReferencePolyData->GetPointData()->AddArray(tempActive);
    return m_ActiveReferencePolyData->GetPoints();
}

vtkPolyData* MultiFragmentRegistrator::IdentifyClosestInactiveFragment(){
    unsigned int maxNumberOfClosePoints=0;
    unsigned int currentActiveFragmentNumber=0;
    //vtkSmartPointer<vtkIntArray> activePoints= vtkSmartPointer<vtkIntArray>::New();

    this->m_ClosestPointFinder->SetReferencePolyData(this->m_ActiveReferencePolyData);

    for (unsigned int fragment=1;fragment<this->m_FragmentContainer.size();fragment++)
    {
        if(this->m_FragmentContainer[fragment].second==0)
        {
            //activePoints->DeepCopy(this->m_FragmentContainer[fragment].first->GetPointData()->GetArray("ActivePoints"));
            //GetPointsWithinRadius mehod changes ActivePoints array
            unsigned int tempNum=this->m_ClosestPointFinder->GetPointsWithinRadius(this->m_FragmentContainer[fragment].first,"CrestPoints",this->m_MaxPointDistanceToRef);
            //this->m_FragmentContainer[fragment].first->GetPointData()->GetArray("ActivePoints")->DeepCopy(activePoints);
            if (tempNum>maxNumberOfClosePoints)
            {
                maxNumberOfClosePoints=tempNum;
                m_ActiveCandidatePolyData=this->m_FragmentContainer[fragment].first;
                currentActiveFragmentNumber=fragment;
            }
        }
    }
    //this->m_ActiveCandidatePolyData->GetPoints();
    this->m_CurrentlyActiveFragment=currentActiveFragmentNumber;
    return m_ActiveCandidatePolyData;
}


vtkSmartPointer<vtkPoints> MultiFragmentRegistrator::ExtractClosestNActivePoints(vtkSmartPointer<vtkPolyData> polyData)
{
    vtkSmartPointer<vtkPoints> finalActivePoints = vtkSmartPointer<vtkPoints>::New();
/*
    for (unsigned int p=0;p<polyData->GetNumberOfPoints();p++)
    {
        std::cout<<distances->GetTuple1(p)<<" "<<polyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(p)<<std::endl;
        if(polyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(p)==1 && distances->GetTuple1(p)<maxDist)
        {
            finalActivePoints->InsertNextPoint(polyData->GetPoint(p));
           std::cout<<distances->GetTuple1(p)<<" "<<polyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(p)<<std::endl;
        }
    }*/

    for (unsigned int p=0;p<polyData->GetNumberOfPoints();p++)
    {

        if(polyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(p)==1 && polyData->GetPointData()->GetArray("ClosestActivePoints")->GetTuple1(p)==1)
        {
            finalActivePoints->InsertNextPoint(polyData->GetPoint(p));
           //std::cout<<distances->GetTuple1(p)<<" "<<polyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(p)<<std::endl;
        }
    }
   return finalActivePoints;

   //////////////////////
/*

   vtkSmartPointer<vtkDoubleArray> mahalanobisSort= vtkSmartPointer<vtkDoubleArray>::New();
   mahalanobisSort->DeepCopy(distances);
   vtkSortDataArray::Sort(mahalanobisSort);
   this->m_MaxMahalnobis=mahalanobisSort->GetTuple1(numberOfPoints);

   std::cout<<"Maximum Mahalanobis distance: "<<m_MaxMahalnobis<<std::endl;

   std::cout<<"Extracting "<<numberOfPoints<< " points from reference."<<std::endl;

   vtkSmartPointer<vtkPoints> pointSet = vtkSmartPointer<vtkPoints>::New();
   pointSet->Reset();
   int pointCounter=0;
   for(unsigned int point=0;point<inputPolyData->GetNumberOfPoints();point++)
   {
     //if(inputPolyData->GetPointData()->GetArray("SurfacePointsClassifier") && inputPolyData->GetPointData()->GetArray("SurfacePointsClassifier")->GetTuple1(point)==0) continue;
     if(mahalanobis->GetTuple1(point)<=m_MaxMahalnobis)
     {
        pointCounter++;
        pointSet->InsertNextPoint(inputPolyData->GetPoint(point));
     }
   }
   std::cout<<"REally extracting "<<pointCounter<< " points from reference."<<std::endl;
   m_OutputPolyData->SetPoints(pointSet);
   return m_OutputPolyData;*/
}

void MultiFragmentRegistrator::ExtractActiveCandidatePoints()
{
    m_ActiveCandidatePointSet->Initialize();
    int activeCounter=0;
    for (unsigned int p=0;p<this->m_ActiveCandidatePolyData->GetNumberOfPoints();p++)
    {
        if(this->m_ActiveCandidatePolyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(p)==1)
        {
            this->m_ActiveCandidatePointSet->InsertNextPoint(this->m_ActiveCandidatePolyData->GetPoint(p));
            activeCounter++;
        }
    }


}

void MultiFragmentRegistrator::TransformInactiveFragments(){

}
