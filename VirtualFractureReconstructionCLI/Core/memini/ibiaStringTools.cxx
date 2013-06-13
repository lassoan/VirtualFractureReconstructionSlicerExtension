

#ifndef IBIASTRINGTOOLS_TXX
#define IBIASTRINGTOOLS_TXX


#include "ibiaStringTools.h"


namespace ibia 
{


void 
Trim(std::string &s)
{
  std::string::size_type pos = s.find_last_not_of(' ');
  if(pos != std::string::npos) 
  {
    s.erase(pos + 1);
    pos = s.find_first_not_of(' ');
    if(pos != std::string::npos) s.erase(0, pos);
  }
  else 
    s.erase(s.begin(), s.end());
}

std::string 
TrimF(std::string s)
{ 
  Trim(s);
  return s;
}

void 
ReplaceString(std::string &inputStr, std::string searchStr, 
  std::string replaceStr)
{
  std::string::size_type pos = 0;
  
  pos = inputStr.find(searchStr);  
  while (pos != std::string::npos)
  { 
    inputStr.replace(pos, searchStr.length(), replaceStr);
    pos = inputStr.find(searchStr, pos + 1);
  }
}

void 
EnsureStringEndsWith(std::string &s, const std::string endWith)
{
  int ewl = endWith.length();
  int sl = s.length();
  
  if (ewl > 0 && ewl <= sl && s.substr(sl - ewl, ewl) != endWith)
    s += endWith;
}

void 
EnsureStringNotEndsWith(std::string &s, const std::string notEndWith)
{
  int newl = notEndWith.length();
  int sl = s.length();
  
  if (newl > 0 && newl <= sl && s.substr(sl - newl, newl) == notEndWith)
    s = s.substr(0, sl - newl);
}


}


#endif
