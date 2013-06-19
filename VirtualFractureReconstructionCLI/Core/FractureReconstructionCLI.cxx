/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $HeadURL: http://svn.slicer.org/Slicer4/trunk/Modules/CLI/GradientAnisotropicDiffusion/GradientAnisotropicDiffusion.cxx $
  Language:  C++
  Date:      $Date: 2011-12-06 15:49:19 -0500 (Di, 06. Dez 2011) $
  Version:   $Revision: 18864 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"

#include "itkGradientAnisotropicDiffusionImageFilter.h"


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTransformFileWriter.h"
#include "itkTransformFileReader.h"

#include "vtkXMLPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"

#include "ibiaCorrespondingPointsIdentifier.h"

#include "itkVectorContainer.h"
#include "FileOutputWriter.h"
#include "ibiaMemIni.h"


#include "ibiaComputeSignedDistanceMapFilter.h"
#include "vtksys/SystemTools.hxx"

#include "MultiFragmentRegistrator.h"

#include "FractureReconstructionCLICLP.h"


int
main(int argc, char **argv)
{
    PARSE_ARGS;
    typedef itk::Image<float, 3> InputImageType;
    typedef itk::Image<unsigned char, 3> LabelType;

    typedef itk::ImageFileReader<LabelType> ReaderType;
    typedef itk::ImageFileReader<InputImageType> ImageReaderType;
    typedef itk::ImageFileWriter<LabelType> WriterType;
    typedef itk::ImageFileWriter<InputImageType> ImageWriterType;
    typedef itk::ImageFileWriter<InputImageType> ImageWriterType;

    typedef itk::Image<float, 3> DistImageType;
    typedef itk::ImageFileReader<InputImageType> DistReaderType;

    typedef itk::CastImageFilter<LabelType,InputImageType> CastImageFilterType;
    typedef CastImageFilterType::Pointer CastImageFilterPointer;

    ImageReaderType::Pointer readerImageR = ImageReaderType::New();
    ImageReaderType::Pointer readerImageC = ImageReaderType::New();
    ReaderType::Pointer readerLabelC = ReaderType::New();
    ReaderType::Pointer readerLabelR = ReaderType::New();

    /*WriterType::Pointer writer = WriterType::New();

  typedef itk::SpatialObjectWriter<> SPWriterType;
  SPWriterType::Pointer spwriter = SPWriterType::New();
  typedef itk::GroupSpatialObject<> GroupType;
  typedef itk::ScalableAffineTransform<double, 3> SPTransformType;
  typename GroupType::Pointer grp = GroupType::New();
  typename SPTransformType::Pointer sptransform = SPTransformType::New();*/

    typedef itk::CastImageFilter<InputImageType, LabelType > CastFilterTypeItoL;

    typedef itk::ScalableAffineTransform<double, 3> ITKTransformType;
    typedef ITKTransformType::Pointer ITKTransformPointer;
    typedef itk::TransformFileWriter TransformWriterType;
    TransformWriterType::Pointer outputTransformWriter = TransformWriterType::New();

    MemIni* m_Ini = new MemIni();

    //std::string settingsFileName=argv[22];

    if(TempFilePath.compare("")==0) TempFilePath=itksys::SystemTools::GetCurrentWorkingDirectory();
    if(SettingsFileName.compare("")==0)
    {
        char temp[300];
        std::string tempString=TempFilePath;
        sprintf(temp,"%s/_Settings.ini",tempString.c_str());
        SettingsFileName=temp;
    }
    m_Ini->SetFileName(SettingsFileName);
    m_Ini->LoadValues();
    m_Ini->Update();
    if(!m_Ini)
    {
        std::cout<<"Ini file"<< SettingsFileName<<" not found!!!"<<std::endl;
        return 0;
    }

    ///Write parameters to ini file
    m_Ini->WriteValue<int>("Mahalanobis","UseCoordinates",useCoordinates);
    m_Ini->WriteValue<int>("Mahalanobis","UseCurvature",useCurvature);
    m_Ini->WriteValue<int>("Mahalanobis","UseNormals",useNormals);
    m_Ini->WriteValue<int>("Mahalanobis","UseTexture",useTexture);

    m_Ini->WriteValue<float>("General","DecimationFactorCandidate",decimationFactor);
    m_Ini->WriteValue<float>("General","DecimationFactorReference",decimationFactor);

    m_Ini->WriteValue<unsigned int>("General","NumberOfBinsHaralick",histogramBins);
    m_Ini->WriteValue<float>("Texture","CylinderLength",cylinderLength);
    m_Ini->WriteValue<float>("Texture","CylinderRadius",cylinderRadius);

    //Update LoadReferenceFromFile after first fragment
    m_Ini->WriteValue<int>("General","LoadReferencePolyDataFromFile",UseReference);

    m_Ini->Update();

    if(fineTuning)
    {
        MultiFragmentRegistrator* multi = new MultiFragmentRegistrator();
        std::vector<ITKTransformPointer> inputTransformContainer;
        for (unsigned int f=0;f<fragmentModelFineTuning.size();f++)
        {
            std::string currentModelName=fragmentModelFineTuning[f];
            vtkSmartPointer<vtkPolyDataReader> r= vtkSmartPointer<vtkPolyDataReader>::New();
            r->SetFileName(currentModelName.c_str());
            r->Update();

            ///Reading initial tranforms
            typedef itk::TransformFileReader TransformReaderType;
            TransformReaderType::Pointer inputTransformReader = TransformReaderType::New();
            inputTransformReader->SetFileName(fineTuningInputTransforms[f]);
            inputTransformReader->Update();

            typedef itk::TransformFileReader::TransformListType * TransformListType;
            TransformListType transforms = inputTransformReader->GetTransformList();
            itk::TransformFileReader::TransformListType::const_iterator it = transforms->begin();

            ITKTransformPointer prepTransform;
            if((!strcmp((*it)->GetNameOfClass(),"AffineTransform"))||(!strcmp((*it)->GetNameOfClass(),"ScalableAffineTransform")))
            {
                prepTransform = static_cast<ITKTransformType*>((*it).GetPointer());
                prepTransform->Print(std::cout);
                inputTransformContainer.push_back(prepTransform);
            }
            ///Add fragment with transform
            multi->AddFragment(r->GetOutput(),prepTransform);
        }
        multi->SetIniFileName(SettingsFileName);
        multi->StartMultiFragmentAlignment();
        //Write output transforms

        for(unsigned int t=0;t<fineTuningOutputTransforms.size();t++)
        {
            ITKTransformPointer invTransformIn = ITKTransformType::New();
            ITKTransformPointer invTransformOut = ITKTransformType::New();
            //inputTransformContainer[t+1]->GetInverse(invTransformIn);
            multi->GetFinalTransformContainer()[t]->GetInverse(invTransformOut);
            invTransformOut->Compose(inputTransformContainer[t+1]);
            std::string fname=fineTuningOutputTransforms[t];
            outputTransformWriter->SetFileName(fname);
            outputTransformWriter->SetInput(invTransformOut);
            outputTransformWriter->Update();

            outputTransformWriter->SetFileName("/tmp/Slicer/transFinal.txt");
            outputTransformWriter->Update();
        }
        return 0;
    }





    ibia::CorrespondingPointsIdentifier<LabelType,InputImageType>* finder = new ibia::CorrespondingPointsIdentifier<LabelType, InputImageType>(m_Ini);
    //finder->SetParameterFileInstance();


    /*if(m_Ini->ReadValue<int>("General","LoadReferencePolyDataFromFile",0)>0 && m_Ini->ReadString("General","ReferenceFileName","").compare("")!=0)
  {
      finder->SetReferenceFileName(m_Ini->ReadString("General","ReferenceFileName",""));
      finder->SetLoadReferenceDataFromFile(true);
  }*/

    if(!referenceModelInput.compare("")==0)
    {
        vtkSmartPointer<vtkXMLPolyDataReader> refModelReader= vtkSmartPointer<vtkXMLPolyDataReader>::New();
        refModelReader->SetFileName(referenceModelInput.c_str());
        refModelReader->Update();
        if(refModelReader->GetOutput())
            finder->SetReferencePolyData(refModelReader->GetOutput());
    }

    if(!candidateModelInput.compare("")==0)
    {
        vtkSmartPointer<vtkXMLPolyDataReader> candModelReader= vtkSmartPointer<vtkXMLPolyDataReader>::New();
        candModelReader->SetFileName(candidateModelInput.c_str());
        candModelReader->Update();
        if(candModelReader->GetOutput())
            finder->SetCandidatePolyData(candModelReader->GetOutput());
    }

    std::cout<<argv[1]<<std::endl;
    readerImageR->SetFileName(referenceImage);
    readerImageR->Update();
    readerImageC->SetFileName(fragmentImage);
    readerImageC->Update();

    readerLabelR->SetFileName(referenceLabelMap);
    readerLabelR->Update();
    readerLabelC->SetFileName(fragmentLabelMap);
    readerLabelC->Update();

    std::cout<<"ALL images read"<<std::endl;

    char dir[300];
    if(OutputDirectory.compare("")==0)
    {
        OutputDirectory=TempFilePath;
        std::string tempString=TempFilePath;
        sprintf(dir,"%s",tempString.c_str());
    }
    else
    {
        sprintf(dir,"%s",OutputDirectory.c_str());
        itksys::SystemTools::MakeDirectory(dir);
    }
    finder->SetOutputDirectory(dir);
    finder->SetSpecialSuffix(outputSuffix);

    finder->SetGreyscaleCandidateImage(readerImageC->GetOutput());
    finder->SetGreyscaleReferenceImage(readerImageR->GetOutput());

    finder->SetExtractReferenceFeatures( m_Ini->ReadValue<int>("Mahalanobis","UseTexture",0)>0);
    finder->SetFlipReference(m_Ini->ReadValue<int>("PreProcessing","FlipReference",0)>0);

    ITKTransformPointer prepTransform;
    if(inputTransform.compare("")!=0)
    {
        typedef itk::TransformFileReader TransformReaderType;
        TransformReaderType::Pointer inputTransformReader = TransformReaderType::New();
        inputTransformReader->SetFileName(inputTransform);
        inputTransformReader->Update();

        typedef itk::TransformFileReader::TransformListType * TransformListType;
        TransformListType transforms = inputTransformReader->GetTransformList();
        itk::TransformFileReader::TransformListType::const_iterator it = transforms->begin();
        if((!strcmp((*it)->GetNameOfClass(),"AffineTransform"))||(!strcmp((*it)->GetNameOfClass(),"ScalableAffineTransform")))
        {
            prepTransform = static_cast<ITKTransformType*>((*it).GetPointer());
            prepTransform->Print(std::cout);
        }
        //For some reason Slicer returns inverse transform
        ITKTransformPointer invTransform = ITKTransformType::New();
        prepTransform->GetInverse(invTransform);
        finder->SetPrepTransform(invTransform);
    }

    finder->SetPerformCorrespondingPointSearch(false);

    LabelType::Pointer outputLabel=finder->doFilter(readerLabelR->GetOutput(),readerLabelC->GetOutput());

    if(outputFragment.compare("")!=0)
    {
        WriterType::Pointer outputWriter = WriterType::New();
        outputWriter->SetFileName(outputFragment);

        outputWriter->SetInput(outputLabel);
        outputWriter->Update();
    }

    outputTransformWriter->SetFileName( outputTransform );
    if(prepTransform)
    {
        ITKTransformPointer invTransformP = ITKTransformType::New();
        ITKTransformPointer invTransform = ITKTransformType::New();
        prepTransform->GetInverse(invTransformP);
        finder->GetCurrentTransform()->GetInverse(invTransform);
        invTransform->Compose(prepTransform);
        outputTransformWriter->SetInput(invTransform);
    }
    else
        outputTransformWriter->SetInput( finder->GetCurrentTransform());

    try
    {
        outputTransformWriter->Update();
    }
    catch( itk::ExceptionObject & err )
    {
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    vtkSmartPointer<vtkXMLPolyDataWriter> outputFragmentPolyWriter = vtkSmartPointer< vtkXMLPolyDataWriter>::New();
    outputFragmentPolyWriter->SetFileName(fragmentModelOutput.c_str());
    outputFragmentPolyWriter->SetInput(finder->GetFragmentPolyData());
    outputFragmentPolyWriter->Update();
    try
    {
        outputFragmentPolyWriter->Update();
    }
    catch( itk::ExceptionObject & err )
    {
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }


    vtkSmartPointer<vtkXMLPolyDataWriter> referencePolyWriter = vtkSmartPointer< vtkXMLPolyDataWriter>::New();
    referencePolyWriter->SetFileName(referenceModelOutput.c_str());
    referencePolyWriter->SetInput(finder->GetReferencePolyData());
    try
    {
        referencePolyWriter->Update();
    }
    catch( itk::ExceptionObject & err )
    {
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    //delete finder;
    return EXIT_SUCCESS;
}
