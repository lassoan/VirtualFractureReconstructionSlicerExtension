/*
 * ComputeMahalanobisDistance.cpp
 *
 *  Created on: 23.02.2012
 *      Author: kfritscher
 */

#include "ComputeMahalanobisDistance.h"
#include "vtkWeakPointer.h"



namespace ibia
{

ComputeMahalanobisDistance::ComputeMahalanobisDistance()
{
  // TODO Auto-generated constructor stub  
  m_ReferenceVectorSize=0;
  m_CovarianceCalculator=CovarianceCalculatorType::New();
  m_DistanceCalculator= DistanceMetricType::New();
  m_ReferenceData=SampleType::New();
  m_UseCurvature=false;
  m_UseTextureFeatures=false;
  m_UseSurfaceNormals=false;
  m_UsePointCoordinates=true;
}

ComputeMahalanobisDistance::~ComputeMahalanobisDistance()
{
  // TODO Auto-generated destructor stub
}


double ComputeMahalanobisDistance::Evaluate(vtkPoints * candidate, unsigned int index)
{
    MeasurementVectorType arr2(3);
    arr2.SetData(candidate->GetPoint(index));
    std::cout << "Sample: " << arr2 << std::endl;
    std::cout <<"Dist: "<<m_DistanceCalculator->Evaluate(arr2)<<std::endl;
    return m_DistanceCalculator->Evaluate(arr2);
}


void ComputeMahalanobisDistance::Evaluate(vtkPoints * candidate,vtkDataArray* distances)
{
    distances->Allocate(candidate->GetNumberOfPoints());
    MeasurementVectorType arr2(3);
    for (unsigned int i=0;i<candidate->GetNumberOfPoints();i++)
    {
       arr2.SetData(candidate->GetPoint(i));
       distances->SetTuple1(i,m_DistanceCalculator->Evaluate(arr2));
    }
}

void ComputeMahalanobisDistance::Evaluate(vtkSmartPointer<vtkPolyData> candidate,vtkDataArray* distances,bool useSurfaceClassArray,bool useActivePointsArray)
{
    char line[1024];
    FILE* candout=fopen("MahalOut_cand+Dist.txt","w");

    unsigned int pointSize=3*this->m_UsePointCoordinates;
    unsigned int texSize=0,curvSize=0;
    unsigned int normalsSize=3*this->m_UseSurfaceNormals;

    vtkWeakPointer<vtkDataArray> texArray=candidate->GetPointData()->GetArray("HaralickFeatures");
    vtkWeakPointer<vtkDataArray> curvArray=candidate->GetPointData()->GetArray("SurfaceMeanCurvature");
    vtkWeakPointer<vtkDataArray> surfPointsClassArray=NULL;
    if(useSurfaceClassArray) surfPointsClassArray=candidate->GetPointData()->GetArray("SurfacePointsClassifier");

    vtkWeakPointer<vtkDataArray> activePointsArray=NULL;
    if(useActivePointsArray) activePointsArray=candidate->GetPointData()->GetArray("ActivePoints");;


    if(texArray)  texSize=texArray->GetNumberOfComponents()*this->m_UseTextureFeatures;
    if(curvArray)  curvSize=curvArray->GetNumberOfComponents()*this->m_UseCurvature;

    unsigned int measurementVectorSize=pointSize+texSize+curvSize+normalsSize;

    double* featureVector= new double[measurementVectorSize];

    std::cout<<"\nMeasurementVectorSizeCandidate= "<<measurementVectorSize<<std::endl;

    MeasurementVectorType arr(measurementVectorSize);
    unsigned int numValidRefPoints=0;

    char arrayname[200];
    sprintf(arrayname,"MD%i%i%i%i",this->m_UsePointCoordinates,this->m_UseCurvature,this->m_UseSurfaceNormals,this->m_UseTextureFeatures);

    distances->Initialize();
    distances->SetNumberOfComponents(1);
    distances->SetNumberOfTuples(candidate->GetNumberOfPoints());
    distances->SetName(arrayname);
    distances->FillComponent(0,std::numeric_limits<float>::max());

    for (unsigned int i=0;i<candidate->GetNumberOfPoints();i++)
    {
        for (unsigned int c=0;c<pointSize;c++)
            featureVector[c]=candidate->GetPoint(i)[c];
        if(texArray && this->m_UseTextureFeatures)
        {
            double*  temp=new double[texArray->GetNumberOfComponents()];
            texArray->GetTuple(i,temp);
            /* if(temp[0]==1)
             {
                 distances->InsertNextTuple1(1000000);
                 continue;
             }*/
            for(unsigned int t=pointSize;t<pointSize+texSize;t++)
                featureVector[t]=temp[t-pointSize];

            delete temp;
        }

        if(this->m_UseCurvature&&curvArray)
        {
            double* temp=new double[curvArray->GetNumberOfComponents()];
            curvArray->GetTuple(i,temp);

            for(unsigned int t=pointSize+texSize;t<pointSize+texSize+curvSize;t++)
                featureVector[t]=temp[t-(pointSize+texSize)];
            delete temp;
        }

        if(this->m_UseSurfaceNormals)
        {
        double*  tempNorm=new double[3];
        candidate->GetPointData()->GetNormals()->GetTuple(i,tempNorm);
        for(unsigned int n=pointSize+texSize+this->m_UseCurvature;n<pointSize+texSize+this->m_UseCurvature+3;n++)
            featureVector[n]=tempNorm[n-(pointSize+texSize+this->m_UseCurvature)];
        delete tempNorm;
        }

        if((surfPointsClassArray && surfPointsClassArray->GetTuple1(i)==0) || (activePointsArray && activePointsArray->GetTuple1(i)==0)  )
            distances->SetTuple1(i,std::numeric_limits<float>::max());
        else
        {
           arr.SetData(featureVector);
           float dist=m_DistanceCalculator->Evaluate(arr);
           sprintf(line, "%6.4f ",featureVector[0]);
           for (unsigned int f=1;f<measurementVectorSize;f++)
                sprintf(line, "%s %6.4f ",line,featureVector[f]);
           sprintf(line, "%s %6.4f ",line,dist);
           if(surfPointsClassArray)
               sprintf(line,"%s %i",line,(int)surfPointsClassArray->GetTuple1(i));
           sprintf(line, "%s\n", line);
           fputs(line,candout);
           distances->SetTuple1(i,m_DistanceCalculator->Evaluate(arr));
           ++numValidRefPoints;
        }
    }
    fclose(candout);
    std::cout<<"Number of valid candidate (MAHAL) points"<<numValidRefPoints<<std::endl;

}

void ComputeMahalanobisDistance::SetReference(vtkSmartPointer<vtkPolyData> reference, bool usePointCoordinates,bool useTexture, bool useCurvature,bool useNormals,bool useSurfaceClassifierArray)
{
    std::cout<<"\nSetting Reference for Mahalanobis "<<std::endl;
    this->SetUsePointCoordinates(usePointCoordinates);
    this->SetUseTextureFeatures(useTexture);
    this->SetUseCurvature(useCurvature);
    this->SetUseSurfaceNormals(useNormals);

    unsigned int pointSize=3*this->m_UsePointCoordinates;
    unsigned int texSize=0,curvSize=0;
    unsigned int normalsSize=3*this->m_UseSurfaceNormals;

    vtkWeakPointer<vtkDataArray> texArray=reference->GetPointData()->GetArray("HaralickFeatures");
    vtkWeakPointer<vtkDataArray> curvArray=reference->GetPointData()->GetArray("SurfaceMeanCurvature");
    vtkWeakPointer<vtkDataArray> surfPointsClassArray=NULL;
    if(useSurfaceClassifierArray) surfPointsClassArray=reference->GetPointData()->GetArray("SurfacePointsClassifier");
    vtkWeakPointer<vtkDataArray> activePointsArray=reference->GetPointData()->GetArray("ActivePoints");

    if(texArray)  texSize=texArray->GetNumberOfComponents()*this->m_UseTextureFeatures;
    if(curvArray)  curvSize=curvArray->GetNumberOfComponents()*this->m_UseCurvature;
/*
    for (unsigned int i=0;i<reference->GetNumberOfPoints();i++)
    {
        for(int j=0;j<reference->GetPointData()->GetArray("HaralickFeatures")->GetNumberOfComponents();j++)
            std::cout<<reference->GetPointData()->GetArray("HaralickFeatures")->GetTuple(i)[j]<<" ";
        std::cout<<std::endl;
    }*/

    char line[1024];
    FILE* refout=fopen("MahalOut_ref.txt","w");

    unsigned int measurementVectorSize=pointSize+texSize+curvSize+normalsSize;

    m_ReferenceData->SetMeasurementVectorSize( measurementVectorSize );
    double* featureVector= new double[measurementVectorSize];

    MeasurementVectorType arr(measurementVectorSize);
    for (unsigned int i=0;i<reference->GetNumberOfPoints();i++)
    {
        if((surfPointsClassArray && surfPointsClassArray->GetTuple1(i)==0) || (activePointsArray && activePointsArray->GetTuple1(i)==0)) continue;

        for (unsigned int c=0;c<pointSize;c++)
            featureVector[c]=reference->GetPoint(i)[c];

        if(texArray&&this->m_UseTextureFeatures)
        {
            //texSize=texArray->GetNumberOfComponents();
            double*  temp=new double[texArray->GetNumberOfComponents()];
            texArray->GetTuple(i,temp);
           if(temp[0]==0)
            {
                delete temp;
                continue;
            }
            for(unsigned int t=pointSize;t<pointSize+texSize;t++)
                featureVector[t]=temp[t-pointSize];

            delete temp;
        }

        if(useCurvature &&curvArray)
        {
            double* temp=new double[curvArray->GetNumberOfComponents()];
            curvArray->GetTuple(i,temp);

            for(unsigned int t=pointSize+texSize;t<pointSize+texSize+curvSize;t++)
                featureVector[t]=temp[t-(pointSize+texSize)];
            delete temp;
        }
        if(useNormals)
        {
            double*  tempNorm=new double[3];
            reference->GetPointData()->GetNormals()->GetTuple(i,tempNorm);
            for(unsigned int n=pointSize+texSize+useCurvature;n<pointSize+texSize+useCurvature+3;n++)
               featureVector[n]=tempNorm[n-(pointSize+texSize+useCurvature)];
            delete tempNorm;
        }

       arr.SetData(featureVector);
       m_ReferenceData->PushBack(arr);

       sprintf(line, "%6.4f ",featureVector[0]);
       for (unsigned int f=1;f<measurementVectorSize;f++)
            sprintf(line, "%s %6.4f ",line,featureVector[f]);
       if(surfPointsClassArray)
           sprintf(line,"%s %i",line,(int)surfPointsClassArray->GetTuple1(i));
       sprintf(line, "%s\n", line);
       fputs(line,refout);

    }
    std::cout<<"\nSetting Reference for Mahalanobis done"<<std::endl;
    fclose(refout);
    this->CalculateMeanAndCovariance();
    delete featureVector;

}

//void ComputeMahalanobisDistance::SetReference(vtkPoints * reference,alglib::integer_1d_array indices, bool posValue)
//{
//    this->CalculateMeanAndCovariance(reference,indices, posValue);
//}


void ComputeMahalanobisDistance::SetReference(vtkPoints * reference)
{
    this->CalculateMeanAndCovariance(reference);
}

//void ComputeMahalanobisDistance::CalculateMeanAndCovariance(vtkPoints * reference,alglib::integer_1d_array indices, bool posValue)
//{

//    m_ReferenceData->Clear();
//    m_ReferenceData->SetMeasurementVectorSize( 3 );

//    MeasurementVectorType arr(3);
//    for (unsigned int i=0;i<reference->GetNumberOfPoints();i++)
//    {
//        if(indices(i)==posValue)
//        {
//            arr.SetData(reference->GetPoint(i));
//            m_ReferenceData->PushBack(arr);
//        }
//    }
//    this->CalculateMeanAndCovariance();
//}


void ComputeMahalanobisDistance::CalculateMeanAndCovariance(vtkPoints* reference)
{
    m_ReferenceData->Clear();
    m_ReferenceData->SetMeasurementVectorSize( 3 );

    MeasurementVectorType arr(3);
    for (unsigned int i=0;i<reference->GetNumberOfPoints();i++)
    {
       arr.SetData(reference->GetPoint(i));
       m_ReferenceData->PushBack(arr);
    }
    this->CalculateMeanAndCovariance();
}

void ComputeMahalanobisDistance::CalculateMeanAndCovariance()
{
    m_CovarianceCalculator->SetInput(m_ReferenceData);
    m_CovarianceCalculator->Update();

    //Set Mean and Covariance Matrix

    const CovarianceCalculatorType::MatrixDecoratedType * decorator = m_CovarianceCalculator->GetCovarianceMatrixOutput();
    DistanceMetricType::CovarianceMatrixType    covariance  = decorator->Get().GetVnlMatrix();

    //std::cout << "Covariance Matrix:\n " << covariance << std::endl;
    const DistanceMetricType::MeanVectorType   mean = m_CovarianceCalculator->GetMean();

    m_DistanceCalculator->SetCovariance(covariance);
    m_DistanceCalculator->SetMean(mean);
    std::cout << "Covariance Matrix:\n " << m_DistanceCalculator->GetCovariance() << std::endl;
    std::cout << "Mean: " <<m_DistanceCalculator->GetMean() << std::endl;
    std::cout<<"Number of reference points (MAHAL) used:"<<this->m_ReferenceData->Size()<<std::endl;
}




} /* namespace ibia */
