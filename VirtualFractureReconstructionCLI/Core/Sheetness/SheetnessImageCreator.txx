#ifndef SheetnessImageCreator_txx_
#define SheetnessImageCreator_txx_

/*
 * SheetnessImageCreator.cpp
 *
 *  Created on: 12.09.2011
 *      Author: kfritscher
 */

#include "SheetnessImageCreator.h"


template<class ImageType>
SheetnessImageCreator<ImageType>::SheetnessImageCreator(){

	m_SheetnessFilter = SheetnessFilterType::New();
	m_HessianFilter= HessianFilterType::New();
	m_EigenAnalysis= EigenAnalysisFilterType::New();

}

template<class ImageType>
SheetnessImageCreator<ImageType>::~SheetnessImageCreator() {
	// TODO Auto-generated destructor stub
}


template<class ImageType>
void SheetnessImageCreator<ImageType>::doFilter(InputImagePointer input) {

	m_HessianFilter->SetInput(input);
    m_HessianFilter->Modified();
	m_EigenAnalysis->SetInput(m_HessianFilter->GetOutput());
    m_EigenAnalysis->Modified();
	m_SheetnessFilter->SetInput(m_EigenAnalysis->GetOutput());
    m_SheetnessFilter->Modified();

	m_SheetnessFilter->SetDetectBrightSheets(true);

    /*

	m_HessianFilter->SetSigma(atof(argv[4]));

	m_SheetnessFilter->SetSheetnessNormalization(atof(argv[5]));

	m_SheetnessFilter->SetBloobinessNormalization(atof(argv[6]));

	m_SheetnessFilter->SetNoiseNormalization(atof(argv[7]));*/

	m_EigenAnalysis->SetDimension(3);

    m_SheetnessFilter->UpdateLargestPossibleRegion();
	m_SheetnessImage=m_SheetnessFilter->GetOutput();

}

#endif
