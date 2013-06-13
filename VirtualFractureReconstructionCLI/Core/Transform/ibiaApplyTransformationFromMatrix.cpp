#include "ibiaApplyTransformationFromMatrix.h"
#include "ibiaConvertBinaryImageToMeshSOFilter.h"

using namespace ibia;

using namespace std;
/**
* \brief Constructor, initializes member variables
*
* \author  K. Fritscher
**/
ApplyTransformationFromMatrix::ApplyTransformationFromMatrix()
{
  m_Quality=1;
  m_BinaryThresholdFilter = BinaryThresholdFilterType::New();
  m_GaussianBlurFilter = GaussianFilterType::New();
}

/**
* \brief Destructor
*
* \author K. Fritscher
**/
ApplyTransformationFromMatrix::~ApplyTransformationFromMatrix()
{
}

/**
* \brief Applies a transformation read from file
*
* \param IPointer - Pointer to input image
*
* \return IPointer - Pointer to filter output(= transformed image)
* \author K. Fritscher
**/
ApplyTransformationFromMatrix::IPointer ApplyTransformationFromMatrix::doFilter(IPointer image,std::string matrixFile,bool invert)
{
  ResFilterType::Pointer resFilter = ResFilterType::New();
  InterpolatorType::Pointer interpol = InterpolatorType::New();
  GroupReaderType::Pointer transformReader = GroupReaderType::New();
  TransformType::Pointer transform = TransformType::New();
  std::cout << "Loading Transform: " << matrixFile << std::endl;
  if(itksys::SystemTools::GetFilenameExtension(matrixFile)==".tfm")
  {
    transformReader->SetFileName(matrixFile.c_str());
    transformReader->Update();
    GroupType::Pointer group = transformReader->GetGroup();
    GroupType::TransformType::Pointer transformSp;
    transformSp = group->GetObjectToParentTransform();
    transform->SetCenter(transformSp->GetCenter());
    transform->SetMatrix(transformSp->GetMatrix());
    transform->SetOffset(transformSp->GetOffset());
    if(invert)
    {
      TransformType::Pointer invertedTransform = TransformType::New();
      transformSp->GetInverse(invertedTransform);
      transform->SetCenter(invertedTransform->GetCenter());
      transform->SetMatrix(invertedTransform->GetMatrix());
      transform->SetOffset(invertedTransform->GetOffset());
    }
  }
  else
  {
    fstream parameters(matrixFile.c_str(),ios::in|ios::binary);
    std::vector<float> param;
    int i=0;
    char  tag[15];
    while (!parameters.eof() && !parameters.fail())
    { if(i<3 ||((i+1)%3)==0)
    parameters.getline(tag,15);
    else
      parameters.getline(tag,15,' ');
    param.push_back(atof(tag));
    i++;
    }
    parameters.close();
    itkMatrix matrix;
    TransformType::OffsetType offset;
    offset[0]=param[0];
    offset[1]=param[1];
    offset[2]=param[2];
    matrix[0][0]=param[3];
    matrix[0][1]=param[4];
    matrix[0][2]=param[5];
    matrix[1][0]=param[6];
    matrix[1][1]=param[7];
    matrix[1][2]=param[8];
    matrix[2][0]=param[9];
    matrix[2][1]=param[10];
    matrix[2][2]=param[11];
    for (int i=0;i<12;i++) std::cout<<param[i]<<std::endl;
    transform->SetMatrix(matrix);
    transform->SetOffset(offset);
    if(invert)
    {
      TransformType::Pointer invertedTransform = TransformType::New();
      transform->GetInverse(invertedTransform);
      transform->SetCenter(invertedTransform->GetCenter());
      transform->SetMatrix(invertedTransform->GetMatrix());
      transform->SetOffset(invertedTransform->GetOffset());
    }
  }
  resFilter->SetTransform(transform);
  if (m_Quality>1)
  {
    resFilter->SetInterpolator(interpol);
  }
  resFilter->SetInput(image);
  resFilter->SetOutputSpacing(image->GetSpacing());
  resFilter->SetSize(image->GetLargestPossibleRegion().GetSize());
	resFilter->SetDefaultPixelValue(-10);
  resFilter->UpdateLargestPossibleRegion();
  return resFilter->GetOutput();
}


ApplyTransformationFromMatrix::TransformType::Pointer ApplyTransformationFromMatrix::GetTransformFromFile(std::string matrixFile)
{
    itkMatrix matrix;
     typedef itk::SpatialObjectReader<3,float>      GroupReaderType;
    typedef GroupReaderType::GroupType      GroupType;
    GroupReaderType::Pointer transformReader = GroupReaderType::New();
    TransformType::Pointer transform = TransformType::New();
    std::cout << "Loading Transform: " << matrixFile << std::endl;
    if(itksys::SystemTools::GetFilenameExtension(matrixFile)==".tfm")
    {
      transformReader->SetFileName(matrixFile.c_str());
      transformReader->Update();
      GroupType::Pointer group = transformReader->GetGroup();
      GroupType::TransformType::Pointer transformSp = GroupType::TransformType::New();
      transformSp = group->GetObjectToParentTransform();
      transform->SetCenter(transformSp->GetCenter());
      transform->SetMatrix(transformSp->GetMatrix());
      transform->SetOffset(transformSp->GetOffset());
    }
    else
    {
      fstream parameters(matrixFile.c_str(),ios::in|ios::binary);
      std::vector<float> param;
      int i=0;
      char  tag[15];
      while (!parameters.eof() && !parameters.fail())
      { if(i<3 ||((i+1)%3)==0)
      parameters.getline(tag,15);
      else
        parameters.getline(tag,15,' ');
      param.push_back(atof(tag));
      i++;
      }
      parameters.close();
      TransformType::OffsetType offset;
      offset[0]=param[0];
      offset[1]=param[1];
      offset[2]=param[2];
      matrix[0][0]=param[3];
      matrix[0][1]=param[4];
      matrix[0][2]=param[5];
      matrix[1][0]=param[6];
      matrix[1][1]=param[7];
      matrix[1][2]=param[8];
      matrix[2][0]=param[9];
      matrix[2][1]=param[10];
      matrix[2][2]=param[11];
      for (int i=0;i<12;i++) std::cout<<param[i]<<std::endl;
      transform->SetMatrix(matrix);
      transform->SetOffset(offset);
    }
    return transform;
}

/**
* \brief Applies a transformation read from file
*
* \param IPointer - Pointer to input image
*
* \return IPointer - Pointer to filter output(= transformed image)
* \author K. Fritscher
**/
ApplyTransformationFromMatrix::IPointer ApplyTransformationFromMatrix::doFilter(IPointer image,std::string matrixFile,IPointer referenceImage,bool invert,bool binary)
{
 TransformType::Pointer transform=this->GetTransformFromFile(matrixFile);
  if(!binary)
    return this->doFilter(image,transform, referenceImage, invert);
  else
  {
      ImageToLabelCastFilterPointer caster = ImageToLabelCastFilterType::New();
      caster->SetInput(image);
      caster->Update();
      LabelToImageCastFilterPointer backCaster = LabelToImageCastFilterType::New();
      backCaster->SetInput(this->doFilterBinary(caster->GetOutput(),transform, referenceImage, invert));
      backCaster->Update();
      return backCaster->GetOutput();
  }
}



ApplyTransformationFromMatrix::LabelPointer
ApplyTransformationFromMatrix::doFilterBinary(LabelPointer image,
       TransformType::Pointer transform, IPointer referenceImage, bool invert)
{

    //TODO:Remove mix of itk filters and IBIA wrapper classes
    SignedDistanceMapFilterPointer distanceMap = SignedDistanceMapFilterType::New();

    ImageToLabelCastFilterPointer caster =  ImageToLabelCastFilterType::New();
    caster->SetInput(this->doFilter(distanceMap->doFilter(image), transform,referenceImage, invert));
    caster->Update();

    m_GaussianBlurFilter->SetSigma(1);

    m_BinaryThresholdFilter->SetInput(m_GaussianBlurFilter->doFilter(this->doFilter(distanceMap->doFilter(image), transform,referenceImage, invert)));
    m_BinaryThresholdFilter->SetOutsideValue(0);
    m_BinaryThresholdFilter->SetInsideValue(1);
    m_BinaryThresholdFilter->SetLowerThreshold(-0.5);
    m_BinaryThresholdFilter->SetUpperThreshold(1000);
    m_BinaryThresholdFilter->UpdateLargestPossibleRegion();

    LabelPointer result = m_BinaryThresholdFilter->GetOutput();
     return result;
}

 ApplyTransformationFromMatrix::IPointer ApplyTransformationFromMatrix::doFilter(IPointer image,TransformType::Pointer transform, IPointer referenceImage, bool invert) {

	typedef itk::ResampleImageFilter<InputImageType, InputImageType> ResFilterType;
	ResFilterType::Pointer resFilter = ResFilterType::New();
	InterpolatorType::Pointer interpol = InterpolatorType::New();
	m_Transform = transform;

    if (invert)
    {
		TransformType::Pointer invertedTransform = TransformType::New();
		transform->GetInverse(invertedTransform);

		m_Transform = invertedTransform;
	}

    if (m_Quality > 1)
    {
        resFilter->SetInterpolator(interpol);
    }

    resFilter->SetInput(image);
    resFilter->SetOutputSpacing(referenceImage->GetSpacing());
    resFilter->SetSize(referenceImage->GetLargestPossibleRegion().GetSize());
    resFilter->SetOutputOrigin(referenceImage->GetOrigin());
    resFilter->SetDefaultPixelValue(-10);
    resFilter->SetTransform(m_Transform);
    resFilter->Update();

    std::cout << "Matrix:" << m_Transform->GetMatrix() << std::endl;
    std::cout << "Offset:" << m_Transform->GetOffset() << std::endl;
    std::cout << "Center:" << m_Transform->GetCenter() << std::endl;


    return resFilter->GetOutput();
}


