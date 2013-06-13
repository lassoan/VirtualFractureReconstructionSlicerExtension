/*
 * FileOutputWriter.h
 *
 *  Created on: 01.12.2011
 *      Author: kfritscher
 */

#ifndef FILEOUTPUTWRITER_H_
#define FILEOUTPUTWRITER_H_

#include <fstream>
#include "stdio.h"

using namespace std;

class FileOutputWriter
{
public:
  FileOutputWriter();


  static void WriteToFile(std::string filename, std::string output,bool append=true, bool newLine=true)
  {
    ofstream outfile;
    if(append)
      outfile.open(filename.c_str(), ios::out|ios::app);
    else
      outfile.open(filename.c_str(), ios::out|ios::trunc);
    outfile<<output.c_str();
    if(newLine)
      outfile<< "\n";
    else
      outfile<< " ";
    outfile.close();
  }

  static std::string ComposeFilename(std::string dir, std::string name)
  {
    char outname[300];
    sprintf(outname,"%s/%s",dir.c_str(),name.c_str());
    return outname;
  }
  virtual
  ~FileOutputWriter();
};

#endif /* FILEOUTPUTWRITER_H_ */
