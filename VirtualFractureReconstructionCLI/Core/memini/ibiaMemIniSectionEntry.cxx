

#ifndef IBIAMEMINISECTIONENTRY_CXX
#define IBIAMEMINISECTIONENTRY_CXX


#include "ibiaMemIniSectionEntry.h"
#include <iostream>


namespace ibia 
{


MemIniSectionEntry
::MemIniSectionEntry(std::string sectionName)
{
  m_SectionName = sectionName;
  m_Indents = new IndentMapType;
}

MemIniSectionEntry
::~MemIniSectionEntry()
{
  delete m_Indents;
}

void 
MemIniSectionEntry
::AddIndent(std::string indent, std::string value)
{
  (*m_Indents)[indent] = value;
}

void 
MemIniSectionEntry
::DeleteIndent(std::string indent)
{
  IndentMapType::iterator cursor = m_Indents->find(indent);
  m_Indents->erase(cursor);
}

MemIniSectionEntry::IndentMapType *
MemIniSectionEntry
::GetIndents()
{
  return m_Indents;
}

std::string 
MemIniSectionEntry
::GetSectionName()
{
  return m_SectionName;
}


}


#endif
