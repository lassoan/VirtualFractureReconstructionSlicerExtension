/*
 * AlglibArrayWriter.h
 *
 *  Created on: 08.09.2011
 *      Author: kfritscher
 */

#ifndef AlglibArrayWriter_H_
#define AlglibArrayWriter_H_


#include <fstream>
#include "stdio.h"

#include "vtkDataArray.h"
#include "itkVectorContainer.h"

  /** \class SampleExtractor
  *
  * \brief
  *
  * \author K. Fritscher
  **/

class AlglibArrayWriter
{
public:

    typedef itk::VectorContainer<unsigned int, std::vector<double> > ContainerMatrixType;
    typedef ContainerMatrixType::Pointer ContainerMatrixPointer;

AlglibArrayWriter();
~AlglibArrayWriter(void);

//void Write2DArray(alglib::real_2d_array matrix, std::string filename,std::string suffix="",bool blank=false);
void Write2DArray(ContainerMatrixPointer matrix, std::string filename,std::string suffix="",bool blank=false);
//void Append2DArray(alglib::real_2d_array matrix, std::string filenameIn,std::string filenameOut,std::string suffix="");
void Append2DArray(ContainerMatrixPointer matrix, std::string filenameIn,std::string filenameOut,std::string suffix="");

//template<class ArrayType>
//void
//Write1DArray(ArrayType matrix, std::string filename, std::string suffix)
//{
//    char filenameOut_final[200];
//    sprintf(filenameOut_final,"%s%s.txt",filename.c_str(),suffix.c_str());
//    std::ofstream matrixfile(filenameOut_final, std::ios::out);
//    for (unsigned int rows = 0; rows < matrix.length(); rows++)
//    {
//        matrixfile << matrix(rows) << "\n";
//    }
//    matrixfile.close();
//}



static void Write1DVTKArray(vtkDataArray* array, std::string filename, std::string suffix)
{
    char filenameOut_final[200];
    sprintf(filenameOut_final,"%s%s.txt",filename.c_str(),suffix.c_str());
    std::ofstream arrayfile(filenameOut_final, std::ios::out);
    for (unsigned int index = 0; index < array->GetNumberOfTuples(); index++)
    {
        arrayfile << *array->GetTuple(index) << "\n";
    }
    arrayfile.close();
}

//template<class ArrayType>
//void
//Append1DArray(ArrayType array1D, std::string filenameIn, std::string filenameOut, std::string suffix)
//{
//    alglib::real_2d_array temparray;
//    temparray.setlength(array1D.length(), 1);
//    for (unsigned int rows = 0; rows < array1D.length(); rows++)
//    {
//        temparray(rows, 0) = array1D(rows);
//    }
//    this->Append2DArray(temparray, filenameIn, filenameOut,suffix.c_str());
//}
};

#endif /* AlglibArrayWriter_H_ */

