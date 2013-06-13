#ifndef __ibiaConvertBinaryImageToMeshSOFilter_h
#define __ibiaConvertBinaryImageToMeshSOFilter_h

#include "itkLightObject.h"
#include "itkBinaryMask3DMeshSource.h"
#include "itkMeshSpatialObject.h"
#include "itkMesh.h"
#include "itkDefaultDynamicMeshTraits.h"



/**
* \class ConvertBinaryImageToMeshSOFilter
*
* \author Klaus Fritscher
**/

namespace ibia
{

	template <class TImageType>
	class ITK_EXPORT ConvertBinaryImageToMeshSOFilter:
		public itk::LightObject
	{
	public:
		typedef ConvertBinaryImageToMeshSOFilter Self;
		typedef itk::LightObject Superclass;
		typedef itk::SmartPointer< Self > Pointer;
		typedef itk::SmartPointer< const Self > ConstPointer;
		typedef itk::WeakPointer<const Self>  ConstWeakPointer;

		typedef TImageType InputImageType;
		typedef typename InputImageType::Pointer InputImageTypePointer;
    typedef typename itk::DefaultDynamicMeshTraits<float,3,3> MeshTrait;
		typedef typename itk::Mesh<float,3,MeshTrait> MeshType;
		typedef typename itk::MeshSpatialObject<MeshType> MeshSpatialObjectType;
		typedef typename MeshType::Pointer MeshTypePointer;
		typedef typename MeshSpatialObjectType::Pointer MeshSpatialObjectTypePointer;
		typedef typename itk::BinaryMask3DMeshSource<InputImageType,MeshType> MeshConverterType;
		typedef typename MeshConverterType::Pointer MeshConverterTypePointer;

		/** Dimension constant. */
		itkStaticConstMacro( ImageDimension, unsigned int, TImageType::ImageDimension);

		itkNewMacro(Self);
		itkTypeMacro(Self, Superclass);
		///Get method that returns mesh
		MeshTypePointer GetMesh()
		{
			return m_MeshResult;
		}
		///Get method that returns meshSO (=result of doFilter)
		MeshSpatialObjectTypePointer GetMeshSO()
		{
			return m_MeshSOResult;
		}

		ConvertBinaryImageToMeshSOFilter();
		~ConvertBinaryImageToMeshSOFilter(void);
		MeshSpatialObjectTypePointer doFilter(InputImageTypePointer binaryImage,short objectValue);

	protected:
		MeshConverterTypePointer m_Converter;
		MeshTypePointer m_MeshResult;
		MeshSpatialObjectTypePointer m_MeshSOResult;

	private:
		ConvertBinaryImageToMeshSOFilter(ConvertBinaryImageToMeshSOFilter&); // no copy constructor
	};
}// namespace


#ifndef ITK_MANUAL_INSTANTIATION
#include "ibiaConvertBinaryImageToMeshSOFilter.txx"
#endif
#endif // __ibiaConvertBinaryImageToMeshSOFilter_h
