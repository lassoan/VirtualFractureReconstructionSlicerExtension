#include "ibiaCorrespondingPointsIdentifier.h"

namespace ibia {

/**
 * \brief Implementation of static "NEW" method from itk to be able to usa IBIA filter pipeline
 * \author KD Fritscher
 **/
/*
template<class LabelType, class InputImageType>
typename CorrespondingPointsIdentifier<LabelType, InputImageType>::Pointer CorrespondingPointsIdentifier<
LabelType, InputImageType>::New(bool preRegistration,
                                unsigned int adjStart, unsigned int adjacencyLevels,
                                unsigned int initialVertIndex, unsigned int smoothingIterations,
                                float maxDist, unsigned int stepSize, bool refMode,bool greyscaleStatistics, bool calculateCorrelation, bool performGreyscaleReg,
                                bool extICP, bool calcDiffVectors) {
    Pointer smartPtr = CorrespondingPointsIdentifier::New();
    smartPtr->m_AdjacencyLevels = adjacencyLevels;
    smartPtr->m_SmoothingIterations = smoothingIterations;
    smartPtr->m_VertIndex = initialVertIndex;
    smartPtr->m_MaxDist = maxDist;
    smartPtr->m_PreRegistration = preRegistration;
    smartPtr->m_StepSize = stepSize;
    smartPtr->m_AdjacencyLevelsStart = adjStart;
    smartPtr->m_ReferenceMode = refMode;
    smartPtr->m_MaxPointDistance = 10.0;
    smartPtr->m_UseGreyscaleStatistics=greyscaleStatistics;
    smartPtr->m_CalculateCorrelation=calculateCorrelation;
    smartPtr->m_PerformGreyscaleRegistration=performGreyscaleReg;
    smartPtr->m_FinalMetricValue=0;
    smartPtr->m_UseExtendedICP=extICP;
    smartPtr->m_CalculateDifferenceVectors=calcDiffVectors;
    smartPtr->m_RefOutFinished=false;
    return smartPtr;
}*/

template<class LabelType, class InputImageType>
CorrespondingPointsIdentifier<LabelType, InputImageType>::CorrespondingPointsIdentifier(MemIni* iniFile)
{

    m_ReferencePolyData = NULL;
    m_CandidatePolyData = NULL;
    m_CreatePolyDataReference =
            new ibia::VTKMarchingCubesFilter<LabelType>();
    m_CreatePolyDataCandidate =
            new ibia::VTKMarchingCubesFilter<LabelType>();

    m_ResultPolyData = vtkSmartPolyDataPointer::New();

    m_CurrentAdjacencyLevel = 0;
    m_CurrentCandidatePoint = 0;
    m_NumberOfReferenceVerts = 0;

    m_IdentifiedCandidatePoints = vtkPoints::New();
    m_IdentifiedReferencePoints = vtkPoints::New();

    m_IdentifiedCandidatePointsGrey = vtkPoints::New();
    m_IdentifiedReferencePointsGrey = vtkPoints::New();

    m_ExtractedCandidateSurfacePoints = vtkPoints::New();
    m_ExtractedReferenceSurfacePoints = vtkPoints::New();

    m_CandidatePointsVector=PointSetType::New();
    m_ReferencePointsVector=PointSetType::New();

    m_CandidatePointsVectorGrey=PointSetType::New();
    m_ReferencePointsVectorGrey=PointSetType::New();

    m_Clone = LabelImageCloneType::New();
    m_ImageClone = ImageCloneType::New();

    m_CurrentMetric = 1000;
    m_InitialITKTransformDM = ITKTransformType::New();
    m_InitialITKTransformPS = ITKTransformType::New();
    m_PrepTransform=NULL;

    m_MCSigma=0.25;

    m_CurrentTransform= ITKTransformType::New();

    m_ICPMovingPointsVector = PointSetTypeICP::New();
    m_ICPFixedPointsVector = PointSetTypeICP::New();

    m_TransformInitializer = TransformInitializerType::New();
    m_Emicp= new ICPWrapper();
    m_ExtractReferencePoints=false;
    m_ExtractReferenceFeatures=true;
    m_PerformCorrespondingPointSearch=false;
    m_FlipReference=false;

    m_SpecialSuffix="";
    m_ReferenceFileName="";
    m_LoadReferencePolyDataFromFile=false;

    m_Ini=iniFile;
    m_Ini->LoadValues();
    this->m_PreRegistration=m_Ini->ReadValue<int>("General","PreRegistration",0)>0;
    this->m_MCSigma=m_Ini->ReadValue<float>("General","SigmaMarchingCubes",0.5);
    m_Ini->ReadValue<float>("General","DecimationFactorCandidate",0.25);
}






/**
 * \brief Destructor
 * \author KD Fritscher
 **/
template<class LabelType, class InputImageType>
CorrespondingPointsIdentifier<LabelType, InputImageType>::~CorrespondingPointsIdentifier() {
    m_CreatePolyDataCandidate=NULL;
    delete m_CreatePolyDataCandidate;
    m_CreatePolyDataReference=NULL;
    delete m_CreatePolyDataReference;
    delete m_Emicp;
}



/**
 * \brief Method that executes the whole pipeline for registering candidate and reference polydata (TODO: HAS TO BE CLEANED UP!!)
 * \author KD Fritscher
 **/
template<class LabelType, class InputImageType>
typename CorrespondingPointsIdentifier<LabelType, InputImageType>::LabelPointer
CorrespondingPointsIdentifier<LabelType, InputImageType>::doFilter(LabelPointer referenceImage, LabelPointer candidateImage)
{

    typename itk::ImageFileWriter<LabelType>::Pointer writer = itk::ImageFileWriter<LabelType>::New();
    typename itk::ImageFileWriter<InputImageType>::Pointer imageWriter = itk::ImageFileWriter<InputImageType>::New();

    typename SampleExtractor<InputImageType>::Pointer samplerR = SampleExtractor<InputImageType>::New();
    typename SampleExtractor<InputImageType>::Pointer samplerC = SampleExtractor<InputImageType>::New();
    LabelPointer currentImage = LabelType::New();
    LabelPointer transformedBinaryImage = LabelType::New();
    m_ResultGreyImage = InputImageType::New();

    this->m_MCSigma=m_Ini->ReadValue<float>("General","SigmaMarchingCubes",0.5);


    if(!this->m_CandidatePolyData)
    {
        m_CreatePolyDataCandidate->SetDecimationFactor(m_Ini->ReadValue<float>("General","DecimationFactorCandidate",0.25));
        m_CandidatePolyData = vtkSmartPolyDataPointer::New();
        m_CandidatePolyData->DeepCopy(m_CreatePolyDataCandidate->MarchingCubesFilter(candidateImage,this->m_MCSigma));
    }

    vtkSmartPointer<vtkPolyDataReader> polyreaderReference = vtkSmartPointer<vtkPolyDataReader>::New();

    if((!this->m_LoadReferencePolyDataFromFile||this->m_ReferenceFileName=="") && !this->m_ReferencePolyData)
    {
        m_CreatePolyDataReference->SetDecimationFactor(m_Ini->ReadValue<float>("General","DecimationFactorReference",0.25));
        m_ReferencePolyData = vtkSmartPolyDataPointer::New();
        m_ReferencePolyData->DeepCopy(m_CreatePolyDataReference->MarchingCubesFilter(referenceImage,this->m_MCSigma));
    }
    else
    {
        std::cout<<"Loading reference polydata from file"<<std::endl;
        if(!this->m_ReferencePolyData)
        {
            polyreaderReference->SetFileName(this->m_ReferenceFileName.c_str());
            polyreaderReference->ReadAllFieldsOn();
            polyreaderReference->Update();
            m_ReferencePolyData->DeepCopy(polyreaderReference->GetOutput());
            m_ReferencePolyData->GetPointData()->PassData(polyreaderReference->GetOutput()->GetPointData());
        }

        //polyreader->GetOutput()->ReadPointData(m_ReferencePolyData->GetPointData(),m_ReferencePolyData->GetNumberOfPoints());
    }

    if(!m_ReferencePolyData->GetPointData()->GetArray("ActivePoints"))
    {
        vtkSmartPointer<vtkIntArray>activePoints=vtkSmartPointer<vtkIntArray>::New();
        activePoints->SetName("ActivePoints");
        activePoints->SetNumberOfComponents(1);
        activePoints->SetNumberOfTuples(this->m_ReferencePolyData->GetNumberOfPoints());
        activePoints->FillComponent(0,1);
        this->m_ReferencePolyData->GetPointData()->AddArray(activePoints);
    }

    //m_ImageClone->CreateAndDeepCopyImage(this->m_GreyscaleCandidateImage,initialGreyScaleImage );

    if (m_PreRegistration)
    {

        PerformPreProcessing(referenceImage, candidateImage,this->m_FlipReference,m_Ini->ReadValue<int>("PreProcessing","RegisterUsingDistanceMaps",0)>0);

        currentImage->CopyInformation(m_PreProcessedLabel);
        currentImage->SetRegions(m_PreProcessedLabel->GetLargestPossibleRegion());
        currentImage->Allocate();
        currentImage->SetPixelContainer(m_PreProcessedLabel->GetPixelContainer());

        candidateImage=m_PreProcessedLabel;


        writer->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"BostonIni-prereg.mhd"));
        writer->SetInput(candidateImage);
        writer->Update();
    }
    else
    {
        currentImage->CopyInformation(candidateImage);
        currentImage->SetRegions(candidateImage->GetLargestPossibleRegion());
        currentImage->Allocate();
        currentImage->SetPixelContainer(candidateImage->GetPixelContainer());
    }

    if(this->m_PrepTransform)
    {
        this->TransformCandidateObjectEnsemble(candidateImage,m_PrepTransform,this->m_FlipReference);

        currentImage->CopyInformation(m_PreProcessedLabel);
        currentImage->SetRegions(m_PreProcessedLabel->GetLargestPossibleRegion());
        currentImage->Allocate();
        currentImage->SetPixelContainer(m_PreProcessedLabel->GetPixelContainer());

        candidateImage=m_PreProcessedLabel;
    }

    InputImagePointer initialGreyScaleImage = InputImageType::New();
    initialGreyScaleImage->CopyInformation(this->m_GreyscaleCandidateImage);
    initialGreyScaleImage->SetRegions(this->m_GreyscaleCandidateImage->GetLargestPossibleRegion());
    initialGreyScaleImage->Allocate();
    initialGreyScaleImage->SetPixelContainer(this->m_GreyscaleCandidateImage->GetPixelContainer());


    transformedBinaryImage->CopyInformation(currentImage);
    transformedBinaryImage->SetRegions(currentImage->GetLargestPossibleRegion());
    transformedBinaryImage->Allocate();
    transformedBinaryImage->FillBuffer(0);

    m_ResultGreyImage->CopyInformation(currentImage);
    m_ResultGreyImage->SetRegions(currentImage->GetLargestPossibleRegion());
    m_ResultGreyImage->Allocate();
    m_ResultGreyImage->FillBuffer(0);

    this->m_CurrentTransform->SetIdentity();

    /*float lastTrans[3];
    lastTrans[0]=this->m_CurrentTransform->GetTranslation()[0];
    lastTrans[1]=this->m_CurrentTransform->GetTranslation()[1];
    lastTrans[2]=this->m_CurrentTransform->GetTranslation()[2];*/

    m_CurrentCandidateIndex = m_VertIndex;

    //////
    std::string suffcand="-cand"+m_SpecialSuffix;
    std::string suffref="-ref"+m_SpecialSuffix;

    //    vtkSmartPointer<vtkPolyDataWriter> polywriter1 =
    //            vtkSmartPointer<vtkPolyDataWriter>::New();

    //    typename itk::ImageFileWriter<LabelType>::Pointer labelWriter =
    //            itk::ImageFileWriter<LabelType>::New();
    //    labelWriter->SetFileName("FeatureLabel.mhd");
    //    labelWriter->SetInputData(candidateImage);
    //    labelWriter->Update();

    //    imageWriter->SetFileName("InitialImage.mhd");
    //    imageWriter->SetInputData(initialGreyScaleImage);
    //    imageWriter->Update();

    //    polywriter1->SetFileName("InitialPoly.vtk");
    //    polywriter1->SetInputData(m_CandidatePolyData);
    //    polywriter1->Update();

    samplerC->SetExtractTextureFeatures(m_ExtractReferenceFeatures);
    samplerC->SetInputImage(initialGreyScaleImage);
    samplerC->SetTemplateImage(candidateImage);
    samplerC->SetInputPolyData(m_CandidatePolyData);
    samplerC->SetSuffix(suffcand);
    samplerC->PerformKMeans(false);
    samplerC->SetExtractSurfacePoints(m_Ini->ReadValue<unsigned int>("Initializer","ExtractCandidatePoints",1)>0);

    samplerC->SetNumberOfBinsForTextureFeatureCalc(m_Ini->ReadValue<int>("General","NumberOfBinsHaralick",20));
    samplerC->SetCylinderAxisLength(m_Ini->ReadValue<float>("Texture","CylinderLength",4));
    samplerC->SetCylinderRadius(m_Ini->ReadValue<float>("Texture","CylinderRadius",2));

    samplerC->ExtractFeatures(3,3);

    samplerR->SetNumberOfBinsForTextureFeatureCalc(m_Ini->ReadValue<int>("General","NumberOfBinsHaralick",20));
    samplerR->SetCylinderAxisLength(m_Ini->ReadValue<float>("Texture","CylinderLength",4));
    samplerR->SetCylinderRadius(m_Ini->ReadValue<float>("Texture","CylinderRadius",2));

    if(!this->m_ReferencePolyData->GetPointData()->GetArray("HaralickFeatures") ||
            (this->m_ReferencePolyData->GetPointData()->GetArray("HaralickFeatures") &&
             this->m_ReferencePolyData->GetPointData()->GetArray("HaralickFeatures")->GetNumberOfComponents()!=
             this->m_CandidatePolyData->GetPointData()->GetArray("HaralickFeatures")->GetNumberOfComponents() ))
        samplerR->SetExtractTextureFeatures(this->m_ExtractReferenceFeatures);
    else
        samplerR->SetExtractTextureFeatures(false);
    samplerR->PerformKMeans(false);

    //Sampling grey values along point normals from greyval. images
    samplerR->SetInputImage(m_GreyscaleReferenceImage);
    samplerR->SetTemplateImage(referenceImage);
    samplerR->SetInputPolyData(m_ReferencePolyData);

    samplerR->SetSuffix(suffref);
    samplerR->SetExtractSurfacePoints(m_ExtractReferencePoints);

    if(m_ReferenceMode && !m_ExtractReferencePoints && this->m_ExtractReferenceFeatures)
    {
        samplerR->PerformKMeans(false);
        samplerR->ExtractFeatures(4,4);
    }
    else if(this->m_ExtractReferenceFeatures)
    {
        samplerR->ExtractFeatures(4,4);
    }


    //Starting Transform Initializer
    m_TransformInitializer->SetParameters(this->m_Ini);
    unsigned int numberOfComponents=m_Ini->ReadValue<int>("General","AdjacencyLevelsMax",1)-m_Ini->ReadValue<int>("General","AdjacencyLevelsStart",1)+1;
    m_Ini->WriteValue("General","NumberOfCurvatureLevels",numberOfComponents);
    m_Ini->Update();

    vtkSmartPointer<vtkDoubleArray> curvatureCandidate =  vtkSmartPointer<vtkDoubleArray>::New();
    vtkSmartPointer<vtkDoubleArray> curvatureReference =  vtkSmartPointer<vtkDoubleArray>::New();
    curvatureCandidate->SetName("SurfaceMeanCurvature");
    curvatureCandidate->SetNumberOfComponents(numberOfComponents);
    curvatureCandidate->SetNumberOfTuples(this->m_CandidatePolyData->GetNumberOfPoints());

    curvatureReference->SetName("SurfaceMeanCurvature");
    curvatureReference->SetNumberOfComponents(numberOfComponents);
    curvatureReference->SetNumberOfTuples(this->m_ReferencePolyData->GetNumberOfPoints());

    unsigned int comp=0;

    for (int c=m_Ini->ReadValue<int>("General","AdjacencyLevelsStart",1);c<=m_Ini->ReadValue<int>("General","AdjacencyLevelsMax",2);c++)
    {
        m_ReferenceGeometry = vtkComputeLocalGeometry::New();
        m_ReferenceGeometry->SetInputData(m_ReferencePolyData);
        m_ReferenceGeometry->Set_adj_levels(c);
        m_ReferenceGeometry->Set_H_smooth_its(m_Ini->ReadValue<int>("General","CurvatureSmoothingIterations",5));
        m_ReferenceGeometry->UpdateWholeExtent();

        for(int r=0;r<m_ReferencePolyData->GetNumberOfPoints();r++)
        {
            curvatureReference->SetComponent(r,comp,m_ReferenceGeometry->GetOutput()->GetPointData()->GetArray("SurfaceMeanCurvature")->GetTuple1(r));
        }

        m_CandidateGeometry = vtkComputeLocalGeometry::New();
        m_CandidateGeometry->SetInputData(m_CandidatePolyData);
        m_CandidateGeometry->Set_adj_levels(c);
        m_CandidateGeometry->Set_H_smooth_its(m_Ini->ReadValue<int>("General","CurvatureSmoothingIterations",5));
        m_CandidateGeometry->UpdateWholeExtent();

        for(int cand=0;cand<m_CandidatePolyData->GetNumberOfPoints();cand++)
        {
            curvatureCandidate->SetComponent(cand,comp,m_CandidateGeometry->GetOutput()->GetPointData()->GetArray("SurfaceMeanCurvature")->GetTuple1(cand));
        }
        comp++;
    }

    //Setting greyscale images to extract candidate points

    m_CandidatePolyData->GetPointData()->AddArray(curvatureCandidate);
    m_ReferencePolyData->GetPointData()->AddArray(curvatureReference);
    m_TransformInitializer->SetCandidateGreyscaleImage(this->m_GreyscaleCandidateImage);
    m_TransformInitializer->SetReferenceGreyscaleImage(this->m_GreyscaleReferenceImage);


    //    vtkSmartPointer<vtkPolyDataWriter> polywriter = vtkSmartPointer< vtkPolyDataWriter>::New();

    //    polywriter->SetFileName("PolyRef.vtk");
    //    polywriter->SetInputData(m_ReferencePolyData);
    //    polywriter->Update();

    //    polywriter->SetFileName("PolyCand.vtk");
    //    polywriter->SetInputData(m_CandidatePolyData);
    //    polywriter->Update();

    std::string tempSuffix=this->m_SpecialSuffix;
    float lastMeanMahalDistance=std::numeric_limits<float>::max();
    int iter=0;

    this->m_CurrentTransform->SetIdentity();

    std::string polyOutput=this->m_OutputDirectory+"/Poly";
    itksys::SystemTools::MakeDirectory(polyOutput.c_str());

    //Set sigma_inf to 5 for first iteration
    float tempSigma=m_Ini->ReadValue<float>("EM-ICP","sigma_inf_ini",1);

    m_Ini->WriteValue<float>("EM-ICP","sigma_inf_ini",4);
    m_Ini->Update();

    vtkSmartPointer<vtkPolyDataWriter> polywriter = vtkSmartPointer< vtkPolyDataWriter>::New();

    do
    {

        if(iter>0)
        {
            lastMeanMahalDistance=m_TransformInitializer->GetSigmaP2Init();

        }
        if(iter==m_Ini->ReadValue<int>("Initializer","ViewerIteration",100))
        {
            m_Ini->WriteValue<int>("Initializer","UseViewer",1);
        }
        else
            m_Ini->WriteValue<int>("Initializer","UseViewer",0);
        m_Ini->Update();

        this->m_SpecialSuffix = this->m_SpecialSuffix+"_"+static_cast<ostringstream*>( &(ostringstream() << (iter+1)) )->str();

        if(iter==0)
            this->m_InitialITKTransformPS=m_TransformInitializer->doFilter(m_ReferencePolyData, m_CandidatePolyData,lastMeanMahalDistance);
        else
            this->m_InitialITKTransformPS=m_TransformInitializer->doFilter(m_ReferencePolyData, m_CandidatePolyData,lastMeanMahalDistance);

        if(m_TransformInitializer->GetRegistrationPerformed())
        {
            vtkPolyData* temp=this->TransformPolyData(m_CandidatePolyData,  m_InitialITKTransformPS);
            this->m_CandidatePolyData->DeepCopy(temp);

            /*std::string filename=FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"/Poly/PolyOut-IniEM"+this->m_SpecialSuffix+".vtk");
            polywriter->SetFileName(filename.c_str());
            polywriter->SetInputData(m_CandidatePolyData);
            polywriter->Update();

            polywriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"/Poly/ReferencePolyData"+this->m_SpecialSuffix+".vtk").c_str());
            polywriter->SetInputData(this->m_ReferencePolyData);
            polywriter->Update();*/

            //Update ReferenceFileName in Ini file
            m_Ini->WriteString("General","ReferenceFileName",FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"Poly/ReferencePolyData"+this->m_SpecialSuffix+".vtk").c_str());
            m_Ini->Update();

            std::cout<<"PolyOut written!!"<<std::endl;

            this->m_SpecialSuffix=tempSuffix;
            iter++;
            //Use coordinates after first iteration
            m_Ini->WriteValue<int>("Mahalanobis","UseCoordinates",1);
            m_Ini->Update();

            if (iter==1)
            {
                //Reset sigma to user defined values
                m_Ini->WriteValue<float>("EM-ICP","sigma_inf_ini",tempSigma);
                m_Ini->Update();
                m_TransformInitializer->SetMaximumMahalanobisDistance(10000);
                m_TransformInitializer->SetUseActiveReferencePointsForMahalanobis(this->m_Ini->ReadValue("General","UseActivePointsForMahalanobisAfterIter1",0));
            }
            this->m_CurrentTransform->Compose(this->m_InitialITKTransformPS);

        }
    }
    while (m_TransformInitializer->GetRegistrationPerformed()&&m_TransformInitializer->GetMaximumMahalnobisDistance()<FLT_MAX);
    this->m_SpecialSuffix=tempSuffix;

    ///Save updated polydata to File

    /*polywriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"/Poly/FinalCandidatePolyData"+this->m_SpecialSuffix+".vtk").c_str());
    polywriter->SetInputData(this->m_CandidatePolyData);
    polywriter->Update();*/

    //Selecting active points
    ActivePointSelector* selector  =new ActivePointSelector();
    selector->SetReferencePolyData(this->m_CandidatePolyData,0);
    selector->MarkInactivePoints(this->m_ReferencePolyData);
    /*polywriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"/Poly/ReferencePolyData"+this->m_SpecialSuffix+"final.vtk").c_str());
    polywriter->SetInputData(this->m_ReferencePolyData);
    polywriter->Update();

    polywriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"/Poly/FinalCandidatePolyData_After_"+this->m_SpecialSuffix+"_final.vtk").c_str());
    polywriter->SetInputData(this->m_CandidatePolyData);
    polywriter->Update();*/

    //Update ReferenceFileName in Ini file
    m_Ini->WriteString("General","ReferenceFileName",FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"Poly/ReferencePolyData"+this->m_SpecialSuffix+"final.vtk").c_str());
    m_Ini->Update();



    AlglibArrayWriter::Write1DVTKArray(this->m_ReferencePolyData->GetPointData()->GetArray("ActivePoints"),"ActiveReferencePoints.txt","");
    std::cout<<"Initialization finished!! "<<iter<<" iterations performed"<<std::endl;

    m_Ini->WriteValue<int>("Mahalanobis","UseCoordinates",1);////////!!!!!!!!!!!!!!!!
    m_Ini->Update();

    //Save label and image data
    //    std::string labelOutput=this->m_OutputDirectory+"/ImagesAndLabels";
    //    itksys::SystemTools::MakeDirectory(labelOutput.c_str());

    //    transformedBinaryImage=this->TransformBinaryImage(currentImage,this->m_CurrentTransform,true,this->m_GreyscaleReferenceImage);

    //    labelWriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"/ImagesAndLabels/FinalLabel"+this->m_SpecialSuffix+".mhd").c_str());
    //    labelWriter->SetInputData(transformedBinaryImage);
    //    labelWriter->Update();

    //    imageWriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"/ImagesAndLabels/FinalGreyscaleImage"+this->m_SpecialSuffix+".mhd").c_str());
    //    imageWriter->SetInputData(this->TransformGreyImage(this->m_GreyscaleCandidateImage,this->m_CurrentTransform,true,this->m_GreyscaleReferenceImage));
    //    imageWriter->Update();

    ////////
    return transformedBinaryImage;
}


/**
 * \brief Method that executes preregistration based on ICP and/or registration of distance maps using MI metric
 * \author KD Fritscher
 **/
template<class LabelType, class InputImageType>
typename CorrespondingPointsIdentifier<LabelType, InputImageType>::LabelPointer
CorrespondingPointsIdentifier<LabelType, InputImageType>::PerformPreProcessing(LabelPointer referenceImage, LabelPointer candidateImage, bool flipReference,bool performDMReg)
{

    typename SPTransformType::Pointer sptransform = SPTransformType::New();
    typename GroupType::Pointer grp = GroupType::New();


    TransformInitializerPointer transformPreprocessor = TransformInitializerType::New();


    SPWriterType::Pointer spwriter = SPWriterType::New();

    typedef MomentRegistrator< LabelType >      MassRegistratorType;
    typedef typename MassRegistratorType::TransformType
            MassRegTransformType;

    typename MassRegistratorType::Pointer registrator =
            MassRegistratorType::New();

    // Clone candidate ensemble to apply combined transform(MomentRegistration+Initializer) after pre-processing
    InputImagePointer tempCandidateGrey = InputImageType::New();
    LabelPointer tempCandidate = LabelType::New();
    vtkSmartPointer<vtkPolyData> tempCandidatePoly = vtkSmartPointer<vtkPolyData>::New();

    m_ImageClone->CreateAndDeepCopyImage(this->m_GreyscaleCandidateImage,tempCandidateGrey);
    m_Clone->CreateAndDeepCopyImage(candidateImage,tempCandidate);
    tempCandidatePoly->DeepCopy(this->m_CandidatePolyData);
    tempCandidatePoly->GetPointData()->PassData(this->m_CandidatePolyData->GetPointData());

    registrator->SetFixedImage(referenceImage  ) ;
    registrator->SetNumberOfMoments(1);

    FlipAxesArrayType flip;

    MemIni* prepIni = new MemIni();
    prepIni->SetFileName(m_Ini->ReadString("General","PPConfigFile","_PrepSettings.ini"));
    prepIni->LoadValues();

    m_InitialITKTransformPS->SetIdentity();

    flip[0]=prepIni->ReadValue<int>("PreProcessing","FlipX",1);
    flip[1]=prepIni->ReadValue<int>("PreProcessing","FlipY",0);
    flip[2]=prepIni->ReadValue<int>("PreProcessing","FlipZ",0);
    if(m_Ini->ReadValue<int>("PreProcessing","UseImageMoments",0)>0)
    {
        if(!flipReference)
        {
            registrator->SetMovingImage(candidateImage) ;
        }
        else
        {
            registrator->SetMovingImage(FlipLabel(candidateImage,flip));
        }

        registrator->Update();

        m_InitialITKTransformPS->SetCenter(registrator->GetTypedTransform()->GetCenter());
        m_InitialITKTransformPS->SetOffset(registrator->GetTypedTransform()->GetOffset());

    }
    this->TransformCandidateObjectEnsemble(candidateImage,m_InitialITKTransformPS,flipReference);

    if(m_Ini->ReadValue<int>("PreProcessing","ImageMomentsOnly",0)==0)
    {

        transformPreprocessor->SetParameters(prepIni);
        //Setting greyscale images to extract candidate points
        transformPreprocessor->SetReferenceGreyscaleImage(this->m_GreyscaleReferenceImage);
        transformPreprocessor->SetCandidateGreyscaleImage(this->m_GreyscaleCandidateImage);
        if(!this->m_LoadReferencePolyDataFromFile||!this->m_ReferencePolyData)
            transformPreprocessor->AlignLabels(referenceImage, this->m_PreProcessedLabel,performDMReg,this->m_MCSigma);
        else
        {
            VTKMarchingCubesFilter<LabelType>* mc = new VTKMarchingCubesFilter<LabelType>();
            mc->SetDecimationFactor(m_Ini->ReadValue<float>("General","DecimationFactorCandidate",0.25));
            transformPreprocessor->AlignPolyData(this->m_ReferencePolyData, mc->MarchingCubesFilter(this->m_PreProcessedLabel,this->m_MCSigma));
            delete mc;
        }
        if(performDMReg)
        {
            m_InitialITKTransformDM->SetIdentity();
            m_InitialITKTransformDM->SetCenter(registrator->GetTypedTransform()->GetCenter());
            m_InitialITKTransformDM->SetOffset(registrator->GetTypedTransform()->GetOffset());
            ThresholdLabelInPlace(referenceImage);
            ThresholdLabelInPlace(m_PreProcessedLabel);
            this->m_InitialITKTransformDM->Compose(transformPreprocessor->GetInitialITKTransformDM());

            sptransform->SetCenter(m_InitialITKTransformDM->GetCenter());
            sptransform->SetMatrix(m_InitialITKTransformDM->GetMatrix());
            sptransform->SetOffset(m_InitialITKTransformDM->GetOffset());
            grp->SetObjectToParentTransform(sptransform.GetPointer());

            spwriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"InitialTransformDM.tfm"));
            spwriter->SetInput(grp);
            spwriter->Update();
            std::cout << "Distance map registered..." << std::endl;

            this->TransformCandidateObjectEnsemble(candidateImage,this->m_InitialITKTransformDM,flipReference);

        }

        m_InitialITKTransformPS->Compose(transformPreprocessor->GetInitialITKTransformPS());

        sptransform->SetCenter(m_InitialITKTransformPS->GetCenter());
        sptransform->SetMatrix(m_InitialITKTransformPS->GetMatrix());
        sptransform->SetOffset(m_InitialITKTransformPS->GetOffset());
        grp->SetObjectToParentTransform(sptransform.GetPointer());

        spwriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"InitialTransformPS.tfm"));
        spwriter->SetInput(grp);
        spwriter->Update();
        candidateImage=tempCandidate;
        this->m_GreyscaleCandidateImage=tempCandidateGrey;
        this->m_CandidatePolyData=tempCandidatePoly;
        this->TransformCandidateObjectEnsemble(candidateImage,this->m_InitialITKTransformPS,flipReference);
    }

    TransformWriter::WriteTransform(m_InitialITKTransformPS,FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"PreProcessingTransform"));
    m_Ini->WriteString("General","InitialTransformName",FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"PreProcessingTransform")+".tfm");
    m_Ini->Update();

    return m_PreProcessedLabel;
}


template<class LabelType, class InputImageType>
void
CorrespondingPointsIdentifier<LabelType, InputImageType>::TransformCandidateObjectEnsemble(LabelPointer candidateImage,ITKTransformPointer transform, bool flip)
{
    typename itk::ImageFileWriter<InputImageType>::Pointer imageWriter =
            itk::ImageFileWriter<InputImageType>::New();

    vtkSmartPointer<vtkPolyDataWriter> polywriter =
            vtkSmartPointer<vtkPolyDataWriter>::New();

    typename itk::ImageFileWriter<LabelType>::Pointer labelWriter =
            itk::ImageFileWriter<LabelType>::New();
    m_PreProcessedLabel = LabelType::New();
    m_PreProcessedLabel->CopyInformation(this->m_GreyscaleReferenceImage);
    m_PreProcessedLabel->SetRegions(this->m_GreyscaleReferenceImage->GetLargestPossibleRegion());
    m_PreProcessedLabel->Allocate();
    m_PreProcessedLabel->FillBuffer(0);


    FlipAxesArrayType flipaxes;

    flipaxes[0]=m_Ini->ReadValue<int>("PreProcessing","FlipX",1);
    flipaxes[1]=m_Ini->ReadValue<int>("PreProcessing","FlipY",0);
    flipaxes[2]=m_Ini->ReadValue<int>("PreProcessing","FlipZ",0);

    if(!flip)
        m_GreyscaleCandidateImage=this->TransformGreyImage(m_GreyscaleCandidateImage,transform,1,m_GreyscaleReferenceImage);
    else
        m_GreyscaleCandidateImage=this->TransformGreyImage(FlipImage(m_GreyscaleCandidateImage,flipaxes),transform,1,m_GreyscaleReferenceImage);
    std::cout<<"Writing image"<<std::endl;

    imageWriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"IniGrey.mhd"));
    imageWriter->SetInput(m_GreyscaleCandidateImage);
    imageWriter->Update();

    if(!flip)
        m_PreProcessedLabel->SetPixelContainer(this->TransformBinaryImage(candidateImage,transform, true,m_GreyscaleReferenceImage)->GetPixelContainer());
    else
        m_PreProcessedLabel->SetPixelContainer(this->TransformBinaryImage(FlipLabel(candidateImage,flipaxes), transform, true,m_GreyscaleReferenceImage)->GetPixelContainer());

    labelWriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"IniLab.mhd"));
    labelWriter->SetInput(m_PreProcessedLabel);
    labelWriter->Update();
    if(!flip)
        m_CandidatePolyData->DeepCopy(this->TransformPolyData(m_CandidatePolyData, transform));
    else
        m_CandidatePolyData->DeepCopy(this->TransformPolyData(m_CreatePolyDataCandidate->MarchingCubesFilter(FlipLabel(candidateImage,flipaxes),this->m_MCSigma),transform));

    polywriter->SetFileName(FileOutputWriter::ComposeFilename(this->m_OutputDirectory,"IniPoly.vtk").c_str());
    polywriter->SetInputData(m_CandidatePolyData);
    polywriter->Update();
}


/**
 * \brief Method that tranforms greyscale images according to given transformation
 * \author KD Fritscher
 **/
template<class LabelType, class InputImageType>
typename CorrespondingPointsIdentifier<LabelType, InputImageType>::InputImagePointer CorrespondingPointsIdentifier<
LabelType, InputImageType>::TransformGreyImage(
        InputImagePointer greyImage, ITKTransformPointer transform,
        bool invert,InputImagePointer referenceImage) {
    ibia::ApplyTransformationFromMatrix *trans =
            new ibia::ApplyTransformationFromMatrix();
    InputImagePointer transformedImage =InputImageType::New();

    this->m_ImageClone->CreateAndDeepCopyImage(trans->doFilter(greyImage, transform,referenceImage, invert),transformedImage);

    trans=NULL;
    delete trans;

    return transformedImage;
}

/**
 * \brief Method that tranforms greyscale images according to given transformation
 * \author KD Fritscher
 **/
template<class LabelType, class InputImageType>
typename CorrespondingPointsIdentifier<LabelType, InputImageType>::LabelPointer
CorrespondingPointsIdentifier<LabelType, InputImageType>::TransformBinaryImage(LabelPointer binaryImage, ITKTransformPointer transform,bool invert,InputImagePointer referenceImage)
{
    ibia::ApplyTransformationFromMatrix *trans = new ibia::ApplyTransformationFromMatrix();
    LabelPointer transformedImage = LabelType::New();
    this->m_Clone->CreateAndDeepCopyImage(trans->doFilterBinary(binaryImage, transform, referenceImage, invert),transformedImage);

    trans=NULL;
    delete trans;

    return transformedImage;
}



/**
 * \brief Method that executes preregistration based on ICP and/or registration of distance maps using MI metric
 * \author KD Fritscher
 **/
template<class LabelType, class InputImageType>
typename CorrespondingPointsIdentifier<LabelType, InputImageType>::vtkSmartPolyDataPointer
CorrespondingPointsIdentifier<LabelType, InputImageType>::TransformPolyData(vtkSmartPolyDataPointer polyData, ITKTransformPointer transform, bool invert) {

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
    transformFilter->SetInputData(polyData);
    if(invert) vtkTrans->Inverse();
    transformFilter->SetTransform(vtkTrans);
    transformFilter->Update();

    /*vtkSmartPointer<vtkPolyDataWriter> polywriter = vtkSmartPointer<
                        vtkPolyDataWriter>::New();
        char finalNamePoly[255];
        sprintf(finalNamePoly, "PolyOut.vtk");
        polywriter->SetFileName(finalNamePoly);
        polywriter->SetInputData(transformFilter->GetOutput());
        polywriter->Update();*/
    std::cout<<"Polydata transformed"<<std::endl;
    return transformFilter->GetOutput();
}


/**
 * \brief Method to extract all points from polydata
 * \author KD Fritscher
 **/
template<class LabelType, class InputImageType>
void CorrespondingPointsIdentifier<LabelType, InputImageType>::ExtractPointsFromPolyData(vtkPolyData* polyData, vtkPoints* pointSet)
{
    pointSet->Reset();
    for(unsigned int point=0;point<polyData->GetNumberOfPoints();point++)
    {
        pointSet->InsertNextPoint(polyData->GetPoint(point));
    }
}

/**
 * \brief Method to extract all Points from polydata
 * \author KD Fritscher
 **/
template<class LabelType, class InputImageType>
void CorrespondingPointsIdentifier<LabelType, InputImageType>::ExtractPointsFromPolyData(vtkPolyData* polyData, PointSetPointer pointSet)
{
    itk::Point<float,3> iPoint;

    pointSet->Initialize();
    for(unsigned int point=0;point<polyData->GetNumberOfPoints();point++)
    {
        for (int i=0;i<3;i++) iPoint[i]=polyData->GetPoint(point)[i];
        pointSet->SetPoint(point,iPoint);
    }
}


template<class LabelType, class InputImageType>
void CorrespondingPointsIdentifier<LabelType, InputImageType>::ThresholdLabelInPlace(LabelPointer label, int lowerThresh, int upperThresh,int in, int out)
{
    m_BinaryThresholdFilter->SetInput(label);
    m_BinaryThresholdFilter->SetOutsideValue(out);
    m_BinaryThresholdFilter->SetInsideValue(in);
    m_BinaryThresholdFilter->SetLowerThreshold(lowerThresh);
    m_BinaryThresholdFilter->SetUpperThreshold(upperThresh);
    m_BinaryThresholdFilter->InPlaceOn();
    m_BinaryThresholdFilter->UpdateLargestPossibleRegion();
}

template<class LabelType, class InputImageType>
typename CorrespondingPointsIdentifier<LabelType, InputImageType>::LabelPointer
CorrespondingPointsIdentifier<LabelType, InputImageType>::FlipLabel(LabelPointer label, FlipAxesArrayType flipaxes)
{
    typedef itk::FlipImageFilter < LabelType > FlipImageFilterType;
    typename FlipImageFilterType::Pointer flip = FlipImageFilterType::New();
    flip->SetInput (label);
    flip->SetFlipAxes(flipaxes);
    flip->Update ();
    return flip->GetOutput();
}

template<class LabelType, class InputImageType>
typename CorrespondingPointsIdentifier<LabelType, InputImageType>::InputImagePointer
CorrespondingPointsIdentifier<LabelType, InputImageType>::FlipImage(InputImagePointer image, FlipAxesArrayType flipaxes)
{
    typedef itk::FlipImageFilter < InputImageType > FlipImageFilterType;
    typename FlipImageFilterType::Pointer flip = FlipImageFilterType::New();
    flip->SetInput (image);
    flip->SetFlipAxes(flipaxes);
    flip->Update ();
    return flip->GetOutput();
}


}

