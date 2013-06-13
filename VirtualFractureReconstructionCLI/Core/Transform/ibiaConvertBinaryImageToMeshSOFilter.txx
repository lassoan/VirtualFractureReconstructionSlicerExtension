#include "ibiaConvertBinaryImageToMeshSOFilter.h"

using namespace ibia;


/**
* \brief Constructor
*
* \author  K. Fritscher
**/
template < typename TImageType>
ConvertBinaryImageToMeshSOFilter<TImageType>::ConvertBinaryImageToMeshSOFilter() 
{
	m_Converter = MeshConverterType::New();
	m_MeshResult = 	MeshType::New();
	m_MeshSOResult = MeshSpatialObjectType::New();
}

/**
* \brief destructor
*
* \author  K. Fritscher
**/
template < typename TImageType>
ConvertBinaryImageToMeshSOFilter<TImageType>::~ConvertBinaryImageToMeshSOFilter() 
{
}

/**
* \brief Converts Binary image to MeshSpatialObject
*
*	\param binaryImage -InputImageTypePointer, binray image to convert
* \param object value - short, intensity value inside object
*
* \return MeshSpatialObjectTypePointer - resulting MeshSpatialObject
* \author  K. Fritscher
**/
template <typename TImageType>
typename ConvertBinaryImageToMeshSOFilter<TImageType>::MeshSpatialObjectTypePointer 
ConvertBinaryImageToMeshSOFilter<TImageType>::doFilter(InputImageTypePointer binaryImage, short objectValue)
{
	m_Converter->SetInput(binaryImage);
	m_Converter->SetObjectValue(objectValue);
	m_Converter->Update();
	m_MeshResult=m_Converter->GetOutput();
	m_MeshSOResult->SetMesh(m_MeshResult);
	return m_MeshSOResult;
}
