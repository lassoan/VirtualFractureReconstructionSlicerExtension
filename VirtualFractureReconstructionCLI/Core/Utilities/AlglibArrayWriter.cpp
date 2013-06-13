/*
 * ${file_base}.cpp
 *
 *  Created on: 02.09.2011
 *      Author: kfritscher
 */




#include "AlglibArrayWriter.h"


using namespace std;


AlglibArrayWriter::AlglibArrayWriter() {
	// TODO Auto-generated constructor stub

}


AlglibArrayWriter::~AlglibArrayWriter() {
	// TODO Auto-generated destructor stub
}



//void
//AlglibArrayWriter::Write2DArray(real_2d_array matrix, std::string filename, std::string suffix,bool blank)
//{
//  char filename_final[200];
//  sprintf(filename_final,"%s%s.txt",filename.c_str(),suffix.c_str());
//  ofstream matrixfile(filename_final, ios::out);
//  for (unsigned int rows = 0; rows < matrix.rows(); rows++)
//  {
//    for (unsigned int col = 0; col < matrix.cols(); col++)
//    {
//      matrixfile << matrix(rows, col) << " ";
//    }
//    matrixfile << "\n";
//    if (blank) matrixfile << "\n";
//  }
//  matrixfile.close();
//}

void
AlglibArrayWriter::Write2DArray(ContainerMatrixPointer matrix, std::string filename, std::string suffix,bool blank)
{
  char filename_final[200];
  unsigned int numCols=matrix->GetElement(0).size();
  unsigned int numRows=matrix->Size();
  sprintf(filename_final,"%s%s.txt",filename.c_str(),suffix.c_str());
  ofstream matrixfile(filename_final, ios::out);
  for (unsigned int rows = 0; rows < numRows; rows++)
  {
    for (unsigned int col = 0; col < numCols; col++)
    {
        matrixfile << matrix->GetElement(rows)[col] << " ";
    }
    matrixfile << "\n";
    if (blank) matrixfile << "\n";
  }
  matrixfile.close();
}



//void AlglibArrayWriter::Append2DArray(real_2d_array matrix, std::string filenameIn,std::string filenameOut,std::string suffix) {

//    FILE *fp;
//    FILE *fout;
//    FILE *tmp = tmpfile();
//    char *p;
//    char line[1024];
//    /* First put contents of file into temporary file */
//    fp = fopen(filenameIn.c_str(), "r");
//    while ((p = fgets(line, 1024, fp))!=NULL) {
//        fputs(line, tmp);
//    }
//    rewind(tmp);
//    fclose(fp);

//    char filenameOut_final[200];
//    sprintf(filenameOut_final,"%s%s.txt",filenameOut.c_str(),suffix.c_str());
//    /* Reopen file now with write permissions */
//    fout=fopen(filenameOut_final, "w");
//    fp = fopen(filenameIn.c_str(), "r");
//    unsigned int row=0;
//    while (( p = fgets(line, 1024, tmp))!=NULL) {
//        line[strlen(line)-1] = '\0'; /* Clear away newline */
//        for (unsigned int col=0;col<matrix.cols();col++)
//        	sprintf(line, "%s %4.2f", line, matrix(row,col));
//        sprintf(line, "%s\n", line);
//        fputs(line, fout);
//        row++;
//    }
//    fclose(fp);
//    fclose(tmp);
//    fclose(fout);
//}


void AlglibArrayWriter::Append2DArray(ContainerMatrixPointer matrix, std::string filenameIn,std::string filenameOut,std::string suffix) {

    FILE *fp;
    FILE *fout;
    FILE *tmp = tmpfile();
    char *p;
    char line[1024];
    unsigned int numCols=matrix->GetElement(0).size();
    /* First put contents of file into temporary file */
    fp = fopen(filenameIn.c_str(), "r");
    while ((p = fgets(line, 1024, fp))!=NULL) {
        fputs(line, tmp);
    }
    rewind(tmp);
    fclose(fp);

    char filenameOut_final[200];
    sprintf(filenameOut_final,"%s%s.txt",filenameOut.c_str(),suffix.c_str());
    /* Reopen file now with write permissions */
    fout=fopen(filenameOut_final, "w");
    fp = fopen(filenameIn.c_str(), "r");
    unsigned int row=0;
    while (( p = fgets(line, 1024, tmp))!=NULL) {
        line[strlen(line)-1] = '\0'; /* Clear away newline */
        for (unsigned int col=0;col<numCols;col++)
            sprintf(line, "%s %4.2f", line, matrix->GetElement(row)[col]);
        sprintf(line, "%s\n", line);
        fputs(line, fout);
        row++;
    }
    fclose(fp);
    fclose(tmp);
    fclose(fout);
}




