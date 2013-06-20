/*
 * DeepCopy.cpp
 *
 *  Created on: 02.09.2011
 *      Author: kfritscher
 */

#include "DeepCopy.h"


namespace ibia{
template<class ImageType>
DeepCopy<ImageType>::DeepCopy() {
	// TODO Auto-generated constructor stub

}


template<class ImageType>
DeepCopy<ImageType>::~DeepCopy() {
	// TODO Auto-generated destructor stub
}


template<class ImageType>
void
DeepCopy<ImageType>::DeepCopyImage(ImagePointer input,ImagePointer output) {

  ConstIteratorType inputIterator(input, input->GetLargestPossibleRegion());
  IteratorType outputIterator(output, output->GetLargestPossibleRegion());

  while(!inputIterator.IsAtEnd())
  {
	  outputIterator.Set(inputIterator.Get());
	  ++inputIterator;
	  ++outputIterator;
  }

}


template<class ImageType>
void DeepCopy<ImageType>::CreateAndDeepCopyImage(ImagePointer input,ImagePointer output) {

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
}

