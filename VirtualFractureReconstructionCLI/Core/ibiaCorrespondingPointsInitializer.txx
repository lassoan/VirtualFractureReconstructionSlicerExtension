#ifndef CorrespondingPointsInitializer_txx_
#define CorrespondingPointsInitializer_txx_

/*
 * ibiaCorrespondingPointsInitializer.cpp
 *
 *  Created on: 08.03.2012
 *      Author: kfritscher
 */

#include "ibiaCorrespondingPointsInitializer.h"
#include "ComputeMahalanobisDistance.h"

#include "time.h"




template<class InputImageType,class GreyscaleImageType>
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::CorrespondingPointsInitializer() {

  m_EMICP= new ICPWrapper();
  m_PointsExtractorCandidate = SurfacePointExtractorType::New();
  m_PointsExtractorReference = SurfacePointExtractorType::New();
  m_CandPerct=50;
  m_RefPerct=15;
  m_ExtractCandidatePoints=true;
  m_ExtractReferencePoints=true;
  m_MDistances= vtkSmartPointer<vtkDoubleArray>::New();
  m_InitialITKTransformPS=ITKTransformType::New();

  m_ReferencePolyData = vtkSmartPointer<vtkPolyData>::New();
  m_CandidatePolyData = vtkSmartPointer<vtkPolyData>::New();
  m_ActiveCandidatePointSelector = new ActivePointSelector();
  m_MaxMahalanobisDistance=FLT_MAX;
  m_RegistrationPerformed=false;
  m_EstimateSigmaP2Ini=true;
  m_UseOnlyActiveSet=false;
  m_SigmaP2Ini=FLT_MAX;
  this->m_UseActiveReferencePoints=true;

  //m_CandidateGreyscaleImage=NULL;
  //m_ReferenceGreyscaleImage=NULL;
}

template<class InputImageType,class GreyscaleImageType>
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::~CorrespondingPointsInitializer() {
    delete m_ActiveCandidatePointSelector;
	// TODO Auto-generated destructor stub
  //Delete leads to crash
  //delete m_EMICP;
}



template<class InputImageType,class GreyscaleImageType>
void
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::AlignLabels(InputImagePointer referenceImage, InputImagePointer candidateImage, bool performDMReg,float mcSigma)
{

    m_EMICP= new ICPWrapper();
    MarchingCubesFilterType* candSurface = new MarchingCubesFilterType();
    MarchingCubesFilterType* refSurface = new MarchingCubesFilterType();

    if(performDMReg)
    {
      SignedDistanceMapFilterPointer dmReference =
                        SignedDistanceMapFilterType::New();
      SignedDistanceMapFilterPointer dmCandidate =
                        SignedDistanceMapFilterType::New();

      ImageRegistrationPointer registration =  ImageRegistrationAppType::New();

      SPTransformPointer sptransform = SPTransformType::New();

      DistImagePointer dmRefImage = dmReference->doFilter(referenceImage);
      DistImagePointer dmCandImage = dmCandidate->doFilter(candidateImage);
      dmRefImage->DisconnectPipeline();
      dmCandImage->DisconnectPipeline();
      registration->SetFixedImage(dmRefImage);
      registration->SetMovingImage(dmCandImage);
      registration->SetMovingImageRegion(dmCandImage->GetLargestPossibleRegion());
      int samples = 1;
      float multi = 0.07;
      for (int dim = 0; dim < 3; dim++)
              samples *= dmCandImage->GetLargestPossibleRegion().GetSize()[dim];

      samples = (int) ((std::ceil(samples * multi)));
      std::cout << "SAMPLES:" << samples;
      registration->SetRigidNumberOfSpatialSamples(samples);
      registration->SetRigidNumberOfIterations(0);
      registration->RegisterUsingNone();
      registration->SetDefaultBackgroundPixelValue(-1);
      registration->SetRigidNumberOfIterations(500); ///ITERATIONS!!
      registration->RegisterUsingRigid();

      sptransform->SetCenter(registration->GetFinalTransform()->GetCenter());
      sptransform->SetMatrix(registration->GetFinalTransform()->GetMatrix());
      sptransform->SetOffset(registration->GetFinalTransform()->GetOffset());
      //grp->SetObjectToParentTransform(sptransform.GetPointer());

      std::cout << "TransformInitializer: Distance map registered..." << std::endl;
      m_InitialITKTransformDM = sptransform;
    }

    this->AlignPolyData(refSurface->MarchingCubesFilter(referenceImage,mcSigma),candSurface->MarchingCubesFilter(candidateImage,mcSigma));
    delete candSurface;
    delete refSurface;
}


template<class InputImageType,class GreyscaleImageType>
void
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::ExtractPoints()
{
    this->ExtractPointsFromReference();
    this->ExtractPointsFromCandidate();
}


template<class InputImageType,class GreyscaleImageType>
vtkSmartPointer<vtkPolyData>
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::CorrectSurfaceClassification(vtkSmartPointer<vtkPolyData> polyInput)
{
     polyInput->GetPointData()->SetScalars(polyInput->GetPointData()->GetArray("SurfacePointsClassifier"));

     vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
     connectivityFilter->SetInput(polyInput);
     connectivityFilter->SetScalarConnectivity(1);
     connectivityFilter->SetScalarRange(1,1);
     connectivityFilter->SetExtractionModeToLargestRegion();
     connectivityFilter->ColorRegionsOn();
     connectivityFilter->Update();

     polyInput->GetPointData()->GetArray("SurfacePointsClassifier")->FillComponent(0,0);


     for(vtkIdType n=0;n<connectivityFilter->GetOutput()->GetNumberOfCells();n++)
     {
         vtkCell* cell=connectivityFilter->GetOutput()->GetCell(n);
         vtkPoints*points=cell->GetPoints();
         for(unsigned int p=0;p<points->GetNumberOfPoints();p++)
         {
             polyInput->GetPointData()->GetArray("SurfacePointsClassifier")->SetTuple1(polyInput->FindPoint(points->GetPoint(p)),1);
         }

     }

     vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
     writer->SetFileName("ConnectivityFilter.vtk");
     writer->SetInput(polyInput);
     writer->Update();

     return connectivityFilter->GetOutput();
}

template<class InputImageType,class GreyscaleImageType>
void
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::ExtractPointsFromReference() {

    //SurfacePointExtractorPointer pointsExtractorCand = SurfacePointExtractorType::New();
    vtkPolyDataWriter* vtkWriterIni= vtkPolyDataWriter::New();
    if(this->m_ExtractReferencePoints && m_ReferenceGreyscaleImage)
    {
      if (m_ReferencePolyData->GetPointData()->GetArray("SurfacePointsClassifier") && !this->m_Atlas)
      {
          m_ReferencePolyData->DeepCopy(m_PointsExtractorReference->ExtractPointsFromPolyData(m_ReferencePolyData,true));
      }
      else if(this->m_Atlas)
      {
        m_PointsExtractorReference->SetUseAtlas(1);
        m_PointsExtractorReference->SetAtlas(this->m_Atlas);
      }
//      else
//      {
//          m_PointsExtractorReference->SetUseMaxSheetnessForExtraction(1);
//      }
      vtkWriterIni->SetFileName("ExtractedReference.vtk");
      vtkWriterIni->SetInput(m_ReferencePolyData);
      vtkWriterIni->Update();
    }

}

template<class InputImageType,class GreyscaleImageType>
void
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::ExtractPointsFromCandidate()
{
    if(this->m_ExtractCandidatePoints && m_CandidateGreyscaleImage)
    {
        if (m_CandidatePolyData->GetPointData()->GetArray("SurfacePointsClassifier"))
        {
            m_CandidatePolyData->DeepCopy(m_PointsExtractorCandidate->ExtractPointsFromPolyData(m_CandidatePolyData,false));
        }
        else
        {
            m_PointsExtractorCandidate->SetUseAtlas(0);
//            m_PointsExtractorCandidate->SetUseMaxSheetnessForExtraction(1);

            //Array lenght and numberOfPoints are NOT EQUAL anymore!! CAVE!!!!
            m_CandidatePolyData->DeepCopy(m_PointsExtractorCandidate->doFilter(m_CandidateGreyscaleImage,m_CandidatePolyData,"-cand",this->m_ExtractCandidateCrest));
        }
       /*vtkWriterIni->SetFileName("ExtractedCandidate.vtk");
        vtkWriterIni->SetInput(m_CandidatePolyData);
        vtkWriterIni->Update();*/
    }
}



template<class InputImageType,class GreyscaleImageType>
void
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::EstimateSigmaP2Ini()
{
    std::cout<<"Starting sigma estimation"<<std::endl;
    ibia::ComputeMahalanobisDistance* mahalanobis = new ibia::ComputeMahalanobisDistance();
    mahalanobis->SetReference(this->m_CandidatePolyData,1,0,0,0);
    vtkDoubleArray* mdist =vtkDoubleArray::New();
    mahalanobis->Evaluate(this->m_ReferencePolyData,mdist,false,this->m_UseActiveReferencePoints);
    std::vector <double>v;
    double sum=0;
    unsigned int validPointsCounter=0;
    for (int i=0;i<mdist->GetSize();i++)
    {
        if(mdist->GetTuple1(i)<FLT_MAX)
        {
            sum+=mdist->GetTuple1(i);
            validPointsCounter++;
            v.push_back(mdist->GetTuple1(i));
        }
    }
    double mean = sum / v.size();
    double variance = 0;
    int vecSize=v.size();
    for(int i = 0;i<vecSize;++i)
    {
        variance += (v[i]-mean)*(v[i]-mean)/(vecSize-1);
    }
    this->m_SigmaP2Ini = pow(mean,2);//pow(*std::max_element(v.begin(),v.end()),2);
    this->m_D02ini=variance;
    std::cout<<"\nMX: "<<this->m_SigmaP2Ini<<"MEAN_MAHAL: "<<mean <<" ^2:"<<pow(mean,2)<<" ^4:"<<pow(mean,4)<<" VARIANCE:"<<variance<<std::endl;

    //delete mahalanobis;
}

template<class InputImageType,class GreyscaleImageType>
typename CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::ITKTransformPointer
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::AlignPolyData(vtkSmartPointer<vtkPolyData> referencePolyData, vtkSmartPointer<vtkPolyData> candidatePolyData)
{
    m_ReferencePolyData->DeepCopy(referencePolyData);
    m_ReferencePolyData->GetPointData()->PassData(referencePolyData->GetPointData());
    m_CandidatePolyData->DeepCopy(candidatePolyData);
    m_CandidatePolyData->GetPointData()->PassData(candidatePolyData->GetPointData());

    m_EMICP= new ICPWrapper();
    this->m_RegistrationPerformed=false;
    this->ReadRegistrationParameters();

    this->ExtractPoints();
    //Array lenght and numberOfPoints are NOT EQUAL anymore!! CAVE!!!!
    this->SetEMICPParameters();

    m_InitialITKTransformPS->SetIdentity();

    this->m_RegistrationPerformed=true;

    if(this->m_UseOnlyActiveSet)
    {

        this->m_MDistances->SetNumberOfComponents(1);
        this->m_MDistances->SetNumberOfTuples(m_ReferencePolyData->GetNumberOfPoints());
        this->m_MDistances->FillComponent(0,FLT_MAX);

        //Hack to extract only active set. TODO: Change!!!
        m_ReferencePolyData=m_PointsExtractorReference->ExtractNPointsWithSmallestMahalanobisDistance(m_ReferencePolyData, m_MDistances,m_CandidatePolyData->GetNumberOfPoints(),true);
    }
    m_EMICP->doRegistration(m_ReferencePolyData,m_CandidatePolyData,m_RefPerct,m_CandPerct);

    m_InitialITKTransformPS->SetCenter(m_EMICP->GetFinalTransform()->GetCenter());
    m_InitialITKTransformPS->SetMatrix(m_EMICP->GetFinalTransform()->GetMatrix());
    m_InitialITKTransformPS->SetOffset(m_EMICP->GetFinalTransform()->GetOffset());

    delete m_EMICP;
    return m_InitialITKTransformPS;
}

template<class InputImageType,class GreyscaleImageType>
typename CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::ITKTransformPointer
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::doFilter(vtkSmartPointer<vtkPolyData> referencePolyData, vtkSmartPointer<vtkPolyData> candidatePolyData,float referenceDistance)
{

    bool useCoordinates=m_Ini->ReadValue<int>("Mahalanobis","UseCoordinates",1)>0;
    bool useCurvature=m_Ini->ReadValue<int>("Mahalanobis","UseCurvature",1)>0;
    bool useTexture=m_Ini->ReadValue<int>("Mahalanobis","UseTexture",0)>0;
    bool useNormals=m_Ini->ReadValue<int>("Mahalanobis","UseNormals",1)>0;

    m_ReferencePolyData->DeepCopy(referencePolyData);
    m_CandidatePolyData->DeepCopy(candidatePolyData);

    m_EMICP= new ICPWrapper();
    this->m_RegistrationPerformed=false;
    this->ReadRegistrationParameters();

    //Calculating Mahalanobis distance between ref and cand points
    std::cout<<"Evaluating mahalanobis distance"<<std::endl;
    ibia::ComputeMahalanobisDistance* mahalanobis = new ibia::ComputeMahalanobisDistance();
    m_MDistances->Initialize();

    this->SelectSafeCandidatePointsForDistanceCalculation(2);
    //Correct surface points by selecting largest connected region
    this->CorrectSurfaceClassification(this->m_CandidatePolyData);

    mahalanobis->SetReference(m_CandidatePolyData,useCoordinates,useTexture,useCurvature,useNormals);
    mahalanobis->Evaluate(m_ReferencePolyData,m_MDistances,false,this->m_UseActiveReferencePoints);
    delete mahalanobis;
    std::cout<<"Finished evaluating mahalanobis distance"<<std::endl;

    //this->ExtractPoints();

    //Get number of surface Points
    int candidateSurfPoints=0;
    if(this->m_CandidatePolyData->GetPointData()->GetArray("SurfacePointsClassifier"))
    {
        for (unsigned int s=0;s<this->m_CandidatePolyData->GetNumberOfPoints();s++)
        {
            if(this->m_CandidatePolyData->GetPointData()->GetArray("SurfacePointsClassifier")->GetTuple1(s)>0) /*&&
                 this->m_CandidatePolyData->GetPointData()->GetArray("ActivePoints")->GetTuple1(s)>0   )*/ ++candidateSurfPoints;
        }
    }
    else
        candidateSurfPoints=this->m_CandidatePolyData->GetNumberOfPoints();

    if(m_Ini->ReadValue<int>("Mahalanobis","ExtractPointsUsingSmallestDist",1)>0)
        m_ReferencePolyData=m_PointsExtractorReference->ExtractNPointsWithSmallestMahalanobisDistance(m_ReferencePolyData, m_MDistances,candidateSurfPoints,this->m_UseActiveReferencePoints);
    else if(this->m_UseOnlyActiveSet)
    {
        this->m_MDistances->FillComponent(0,FLT_MAX);
         m_ReferencePolyData=m_PointsExtractorReference->ExtractNPointsWithSmallestMahalanobisDistance(m_ReferencePolyData, m_MDistances,candidateSurfPoints,true);
    }

    this->EstimateSigmaP2Ini();
    this->SetEMICPParameters();

    this->m_MaxMahalanobisDistance=m_PointsExtractorReference->GetMaximumMahalnobisDistance();

    //Need to copy arrays since we copy this->m_ReferencePolyData from input (might not be necessary!!!)
    if(!referencePolyData->GetPointData()->GetArray("ClosestPoints"))
        referencePolyData->GetPointData()->AddArray(this->m_ReferencePolyData->GetPointData()->GetArray("ClosestPoints"));
    else
        referencePolyData->GetPointData()->GetArray("ClosestPoints")->DeepCopy(this->m_ReferencePolyData->GetPointData()->GetArray("ClosestPoints"));

    if(!referencePolyData->GetPointData()->GetArray(m_MDistances->GetName()))
        referencePolyData->GetPointData()->AddArray(m_MDistances);
    else
    {
        referencePolyData->GetPointData()->RemoveArray(m_MDistances->GetName());
        referencePolyData->GetPointData()->AddArray(m_MDistances);
    }

    candidatePolyData->GetPointData()->AddArray(this->m_CandidatePolyData->GetPointData()->GetArray("ActivePoints"));
    candidatePolyData->GetPointData()->GetArray("SurfacePointsClassifier")->DeepCopy(this->m_CandidatePolyData->GetPointData()->GetArray("SurfacePointsClassifier"));


    std::cout<<"Current maximum Mahalanobis distance:"<<this->m_MaxMahalanobisDistance;

    this->WriteReferencePointInformationToFile("ReferenceMahalanobis.txt",m_ReferencePolyData);

    m_InitialITKTransformPS->SetIdentity();

    if(this->m_SigmaP2Ini<referenceDistance ||referenceDistance==FLT_MAX)
    {
        this->m_RegistrationPerformed=true;
        this->ExtractPointsFromCandidate();
        m_EMICP->doRegistration(m_ReferencePolyData, m_CandidatePolyData,m_RefPerct,m_CandPerct);

        m_InitialITKTransformPS->SetCenter(m_EMICP->GetFinalTransform()->GetCenter());
        m_InitialITKTransformPS->SetMatrix(m_EMICP->GetFinalTransform()->GetMatrix());
        m_InitialITKTransformPS->SetOffset(m_EMICP->GetFinalTransform()->GetOffset());
    }
    m_ReferencePolyData->Reset();
    m_CandidatePolyData->Reset();
    delete m_EMICP;
    return m_InitialITKTransformPS;
}


template<class InputImageType,class GreyscaleImageType>
void
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::SelectSafeCandidatePointsForDistanceCalculation( float radius)
{
    vtkSmartPointer<vtkPolyData> crestPoly = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> crestPoints = vtkSmartPointer<vtkPoints>::New();

    for(unsigned int p=0;p<this->m_CandidatePolyData->GetNumberOfPoints();p++)
    {
        if(this->m_CandidatePolyData->GetPointData()->GetArray("CrestPoints")->GetTuple1(p)>0)
            crestPoints->InsertNextPoint(this->m_CandidatePolyData->GetPoint(p));
    }
    crestPoly->SetPoints(crestPoints);

    vtkSmartPointer<vtkIntArray> crestPolyActiveArray = vtkSmartPointer<vtkIntArray>::New();
    crestPolyActiveArray->SetName("ActivePoints");
    crestPolyActiveArray->SetNumberOfComponents(1);
    crestPolyActiveArray->SetNumberOfTuples(crestPoly->GetNumberOfPoints());
    crestPolyActiveArray->FillComponent(0,1);
    crestPoly->GetPointData()->AddArray(crestPolyActiveArray);

    m_ActiveCandidatePointSelector->SetReferencePolyData(crestPoly,1);
    m_ActiveCandidatePointSelector->GetPointsWithinRadius(this->m_CandidatePolyData,"SurfacePointsClassifier",radius,0);
}

template<class InputImageType,class GreyscaleImageType>
void
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::WriteReferencePointInformationToFile(std::string filename,vtkSmartPointer<vtkPolyData> referencePolyData)
{

    char line[1024];
    /*time_t ltime;
    time(&ltime);
    std::stringstream ss;
    ss << ltime;*/
    FILE* refout=fopen(filename.c_str(),"w");
    double pos[3];
    for (int i=0;i<referencePolyData->GetNumberOfPoints();i++)
    {
        float dist=m_MDistances->GetTuple1(i);
        if (dist<=this->m_MaxMahalanobisDistance)
        {
            referencePolyData->GetPoints()->GetPoint(i,pos);
            sprintf(line,"%6.4f  %6.4f  %6.4f %6.4f",pos[0],pos[1],pos[2],dist);
            sprintf(line, "%s\n", line);
            fputs(line,refout);
        }
    }
    fclose(refout);
}


template<class InputImageType,class GreyscaleImageType>
void
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::ReadRegistrationParameters()
{
    std::cout<<"Reading EMICP parameters"<<std::endl;

    this->m_ExtractCandidatePoints=m_Ini->ReadValue<int>("Initializer","ExtractCandidatePoints",1);
    this->m_ExtractReferencePoints=m_Ini->ReadValue<int>("Initializer","ExtractReferencePoints",0);

    this->m_ExtractCandidateCrest=m_Ini->ReadValue<int>("Initializer","ExtractCandidateCrest",0);
    this->m_ExtractReferenceCrest=m_Ini->ReadValue<int>("Initializer","ExtractReferenceCrest",0);
    this->m_CandPerct=m_Ini->ReadValue<int>("Initializer","PerctCand",90);
    this->m_RefPerct = m_Ini->ReadValue<int>("Initializer","PerctRef",90);
}

template<class InputImageType,class GreyscaleImageType>
void
CorrespondingPointsInitializer<InputImageType, GreyscaleImageType>::SetEMICPParameters()
{
    /*this->m_SigmaP2Ini=m_Ini->ReadValue<float>("EM-ICP","sigma_p2_ini",10);
    if( this->m_SigmaP2Ini==0)
        this->EstimateSigmaP2Ini();
    if(this->m_SigmaP2Ini>150||this->m_SigmaP2Ini!=this->m_SigmaP2Ini)
    {
        std::cout<<"SigmaP2Ini was reset to 150"<<std::endl;
        this->m_SigmaP2Ini=150;
    }*/

    registrationParameters para;

    para.sigma_p2=this->m_SigmaP2Ini<150?this->m_SigmaP2Ini : 150;
    para.sigma_inf=m_Ini->ReadValue<float>("EM-ICP","sigma_inf_ini",1);
    if(para.sigma_inf>para.sigma_p2) para.sigma_inf=1;
    para.sigma_factor=m_Ini->ReadValue<float>("EM-ICP","sigma_factor_ini",0.995);
    para.d_02=this->m_D02ini;//m_Ini->ReadValue<float>("EM-ICP","d_02_ini",10);
    m_EMICP->SetViewer(m_Ini->ReadValue<int>("Initializer","UseViewer",0)>0);
    m_EMICP->SetRegistrationParameters(para);
}



#endif
