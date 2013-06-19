#ifndef SheetnessImageCreator_txx_
#define SheetnessImageCreator_txx_

#include "DeepCopy.h"

/*
 * SheetnessImageCreator.cpp
 *
 *  Created on: 12.09.2011
 *      Author: kfritscher
 */

#include "SheetnessImageCreator.h"


template<class InputImageType>
SheetnessImageCreator<InputImageType>::SheetnessImageCreator(){

    //m_SheetnessFilter = SheetnessFilterType::New();
    //m_HessianFilter= HessianFilterType::New();
    //m_EigenAnalysis= EigenAnalysisFilterType::New();
    m_SheetnessImage = InputImageType::New();

}

template<class InputImageType>
SheetnessImageCreator<InputImageType>::~SheetnessImageCreator() {
	// TODO Auto-generated destructor stub
    m_SheetnessImage=NULL;
}


template<class InputImageType>
void SheetnessImageCreator<InputImageType>::doFilter(InputImagePointer input) {

    SheetnessFilterPointer sheetnessFilter = SheetnessFilterType::New();
    HessianFilterPointer hessianFilter= HessianFilterType::New();
    EigenAnalysisFilterPointer eigenAnalysis= EigenAnalysisFilterType::New();

    hessianFilter->SetInput(input);
    hessianFilter->Modified();

    eigenAnalysis->SetInput(hessianFilter->GetOutput());
    eigenAnalysis->SetDimension(3);
    eigenAnalysis->Modified();

    sheetnessFilter->SetInput(eigenAnalysis->GetOutput());
    sheetnessFilter->Modified();

    sheetnessFilter->SetDetectBrightSheets(true);
    sheetnessFilter->UpdateLargestPossibleRegion();

    typename ibia::DeepCopy<InputImageType>::Pointer clone = ibia::DeepCopy<InputImageType>::New();
    clone->CreateAndDeepCopyImage(m_SheetnessFilter->GetOutput(),m_SheetnessImage);

}

#endif
