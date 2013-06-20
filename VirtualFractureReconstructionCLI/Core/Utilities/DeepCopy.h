/*
 * DeepCopy.h
 *
 *  Created on: 02.09.2011
 *      Author: kfritscher
 */

#include "itkLightObject.h"
#include "itkImageRegionConstIterator.h"

#include "itkImageRegionIterator.h"

#ifndef DEEPCOPY_H_
#define DEEPCOPY_H_


namespace ibia
{
/**
 * \class CorrespondingPointsIdentifier
 *
 * \brief
 *
 * \author
 */

  template<class ImageType>
      class DeepCopy : public itk::LightObject
      {
        public:
          typedef ibia::DeepCopy<ImageType> Self;
          typedef itk::LightObject Superclass;
          typedef itk::SmartPointer< Self > Pointer;
          typedef itk::SmartPointer< const Self > ConstPointer;
          typedef itk::WeakPointer<const Self>  ConstWeakPointer;

          typedef typename ImageType::Pointer ImagePointer;

          typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
          typedef itk::ImageRegionIterator<ImageType> IteratorType;

          itkNewMacro(Self);
          itkTypeMacro(Self, Superclass);



    public:

		/** Internal structure used for passing image data into the threading library */

          DeepCopy();
          void DeepCopyImage(ImagePointer input,ImagePointer output);
          void CreateAndDeepCopyImage(ImagePointer input,ImagePointer output);

          static void StaticCreateAndDeepCopyImage(ImagePointer input,ImagePointer output) {

            //Copying information and allocating space
            output->CopyInformation(input);
            output->SetRegions(input->GetLargestPossibleRegion());
            output->Allocate();

            ConstIteratorType inputIterator(input, input->GetLargestPossibleRegion());
            IteratorType outputIterator(output, output->GetLargestPossibleRegion());

            while(!inputIterator.IsAtEnd())
            {
                outputIterator.Set(inputIterator.Get());
                ++inputIterator;
                ++outputIterator;
            }

          }

	      ~DeepCopy();



  	private:
      DeepCopy(DeepCopy&); // prevent default copy constructor

  }; // class CorrespondingPointsIdentifier

} // namespace ibia
#ifndef ITK_MANUAL_INSTANTIATION
#include "DeepCopy.txx"
#endif
#endif // __ibiaCorrespondingPointsIdentifier_h
