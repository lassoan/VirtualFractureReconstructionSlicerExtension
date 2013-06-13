

#include "ibiaMemIni.h"
#include <fstream>
#include "itksys/SystemTools.hxx"


namespace ibia 
{


MemIni
::MemIni()
{
  Init();
}

MemIni
::MemIni(std::string fileName)
{  
  Init();
  m_Filename = fileName;
  LoadValues();
}

MemIni
::~MemIni()
{
  DiscardChanges();
  for(unsigned int i = 0; i < m_Sections->size(); ++i)
    delete (*m_Sections)[i];
  m_Sections->clear();
  delete m_Sections;
  m_PreHeader->clear();
  delete m_PreHeader;
}

void 
MemIni
::AddPreHeader(const std::string newHeaderEntry)
{
  m_PreHeader->push_back(newHeaderEntry);
  m_ContentChanged = true;
}

bool 
MemIni
::DeleteComments(const std::string section)
{
  bool result = false;
  
  if (section.length() > 0) 
  { 
    for (unsigned int i = 0; i < m_Sections->size(); ++i)
    { 
      MemIniSectionEntry *sect = (*m_Sections)[i];
      if (sect->GetSectionName() == section)
      { 
        MemIniSectionEntry::IndentMapType *indents = sect->GetIndents(); 
        MemIniSectionEntry::IndentMapType::iterator it;
        for (it = indents->begin(); it != indents->end(); ++it)
        { 
          if (it->first[0] == '[')
          { 
            indents->erase(it);
            result = true;
          }
        }
        break;
      } 
    }
  }
  
  if (result)
    m_ContentChanged = true;
    
  return result;
}

bool 
MemIni
::DeleteAll()
{
  bool ret;
  std::vector<std::string> *secs;
  
  // pre-header:
  ret = DeletePreHeader();
  
  // sections:
  secs = GetSections();  
  if (secs->size() > 0)
    for (unsigned int i = 0; i < secs->size(); ++i)
      ret = ret || DeleteSection((*secs)[i]);
  delete secs;
  
  return ret;
}

bool MemIni::DeleteIndent(const std::string section, const std::string indent)
{
  bool result = false;

  if (section.length() > 0 && indent[0] != ';' && indent[0] != '/') 
  { 
    for (unsigned int i = 0; i < m_Sections->size(); ++i)
    { 
      MemIniSectionEntry *sect = (*m_Sections)[i];
      if (sect->GetSectionName() == section)
      { 
        MemIniSectionEntry::IndentMapType::iterator it = sect->GetIndents()->find(indent);
        if (it != sect->GetIndents()->end())
        { 
          sect->GetIndents()->erase(it);
          result = true;
        }
        break;
      } 
    }
  }
  
  if (result)
    m_ContentChanged = true;
    
  return result;
}

bool 
MemIni
::DeletePreHeader()
{
  if (m_PreHeader->size() > 0)
  { 
    m_PreHeader->clear();
    m_ContentChanged = true;
    
    return true;
  }
  else
    return false;
}

bool 
MemIni
::DeleteSection(const std::string section)
{
  bool result = false;
 
  if (section.length() > 0) 
  { 
    for (unsigned int i = 0; i < m_Sections->size(); ++i)
    { 
      MemIniSectionEntry *sect = (*m_Sections)[i];
      if (sect->GetSectionName() == section)
      { 
        m_Sections->erase(m_Sections->begin() + i);
        result = true;
        break;
      } 
    }
  }
  
  if (result)
    m_ContentChanged = true;
  
  return result;
}

void 
MemIni
::DiscardChanges()
{
  m_ContentChanged = false;
}

std::string 
MemIni
::GetFileName()
{
  return m_Filename;
}

std::vector<std::string> *
MemIni
::GetComments(const std::string section)
{ 
  std::vector<std::string> *result = new std::vector<std::string>;
  
  if (section.length() > 0) 
  { 
    for (unsigned int i = 0; i < m_Sections->size(); ++i)
    { 
      MemIniSectionEntry *sect = (*m_Sections)[i];
      
      if (sect->GetSectionName() == section)
      { 
        MemIniSectionEntry::IndentMapType *indents = sect->GetIndents();
        MemIniSectionEntry::IndentMapType::iterator it;
        for (it = indents->begin(); it != indents->end(); ++it)
          if (it->first[0] == '[')
            result->push_back(it->first.substr(1, it->first.length() - 1));
        break;
      } 
    }
  }
  
  return result;
}

std::vector<std::string> *
MemIni
::GetIndents(const std::string section)
{ 
  std::vector<std::string> *result = new std::vector<std::string>;
  
  if (section.length() > 0) 
  { 
    for (unsigned int i = 0; i < m_Sections->size(); ++i)
    { 
      MemIniSectionEntry *sect = (*m_Sections)[i];
      
      if (sect->GetSectionName() == section)
      { 
        MemIniSectionEntry::IndentMapType *indents = sect->GetIndents();
        MemIniSectionEntry::IndentMapType::iterator it;
        for (it = indents->begin(); it != indents->end(); ++it)
          if (it->first.length() > 0 && 
              it->first[0] != '[')
            result->push_back(it->first);
        break;
      } 
    }
  }
  
  return result;
}

std::vector<std::string> *
MemIni
::GetPreHeader()
{ 
  return new std::vector<std::string>(*m_PreHeader);
}

std::vector<std::string> *
MemIni
::GetSections()
{
  std::vector<std::string> *result = new std::vector<std::string>;
  
  for (unsigned int i = 0; i < m_Sections->size(); ++i)
  { 
    MemIniSectionEntry *sect = (*m_Sections)[i];
    result->push_back(sect->GetSectionName());
  }
  
  return result;
}

bool 
MemIni
::IsIndentExisting(std::string section, std::string indent)
{
  if (section.length() > 0)
  { 
    for (unsigned int i = 0; i < m_Sections->size(); ++i)
    { 
      MemIniSectionEntry *sect = (*m_Sections)[i];
      
      if (sect->GetSectionName() == section)
      { 
        MemIniSectionEntry::IndentMapType::iterator it = sect->GetIndents()->find(indent);
        
        if (it != sect->GetIndents()->end())
          return true;
        else
          return false;
      } 
    }
    
    return false; // obviously not found
  }
  else
    return false;
}

std::string 
MemIni
::ReadString(const std::string section, const std::string indent, 
  const std::string defaultValue)
{
  std::string result = defaultValue;
  
  if (section.length() > 0 && indent[0] != ';' && indent[0] != '/')
  { 
    for (unsigned int i = 0; i < m_Sections->size(); ++i)
    { 
      MemIniSectionEntry *sect = (*m_Sections)[i];
      if (sect->GetSectionName() == section)
      { 
        MemIniSectionEntry::IndentMapType::iterator it = sect->GetIndents()->find(indent);
        if (it != sect->GetIndents()->end())
          result = it->second;
        break;
      }
    }
  } 
  
  return result;
}

void 
MemIni
::SetFileName(std::string fileName)
{
  if (m_Filename != fileName) 
  { 
    m_Filename = fileName;
    m_ContentChanged = true;
  }
}

bool 
MemIni
::Update()
{
  if (m_ContentChanged) 
  { 
    if (!itksys::SystemTools::FileExists(m_Filename.c_str()))
    { 
      // make sure that folder exists:
      itksys_stl::string s = itksys::SystemTools::GetFilenamePath(m_Filename.c_str());
      if (s.length() > 0)
      { 
        if (!itksys::SystemTools::MakeDirectory(s.c_str()))
          return false;
      } // else: eventually in current application-folder
    }      
    std::ofstream ofile(m_Filename.c_str());
    
    PrintOut(ofile); 
    ofile.close();
    m_ContentChanged = false; // set back
    
    return true;
  }
  else
    return false;
}

bool 
MemIni
::WriteString(const std::string section, const std::string indent, 
  const std::string value)
{
  bool result = false;
  MemIniSectionEntry *sect = NULL;
  
  if (section.length() > 0) 
  { 
    for (unsigned int i = 0; i < m_Sections->size(); ++i)
    { 
      sect = (*m_Sections)[i];
      if (sect->GetSectionName() == section)
      { 
        if (indent.length() > 0) 
          sect->AddIndent(indent, value);
        result = true;
        break;
      } 
    }
    if (!result) // section does not exist yet
    { 
      sect = new MemIniSectionEntry(section);
      m_Sections->push_back(sect);
      if (indent.length() > 0)
        sect->AddIndent(indent, value);
      result = true;
    }
  }
  
  if (result)
    m_ContentChanged = true;
  
  return result;
}

void 
MemIni
::LoadValues() 
{ 
  std::string s;
  MemIniSectionEntry *sect = NULL;

  if (m_Filename == "")
    return;
  
  if (!itksys::SystemTools::FileExists(m_Filename.c_str())) // seems to be a new file
    return;
  
  m_Sections->clear();
  std::fstream ifile(m_Filename.c_str(), std::ios::in);
  for (std::string line; std::getline(ifile, line); )
  { 
    Trim(line);
    
    if (line.length() > 0 && 
        line[0] == '[' && 
        line[line.length() - 1] == ']') // section
    { 
      s = line.substr(1, line.length() - 2);
      sect = new MemIniSectionEntry(s);
      m_Sections->push_back(sect);
    }
    else // indent or comment
    { 
      if (sect == NULL)
      { 
        if (line.length() > 0)
          m_PreHeader->push_back(line);
        continue;
      }
            
      if (line[0] != ';' && 
          line.substr(0, 2) != "//" &&
          line.find("=", 0) > 0 && 
          line.find("=", 0) < line.length()) // indent [with value]
      { 
        sect->AddIndent(line.substr(0, line.find("=", 0)), 
                        line.substr(line.find("=", 0) + 1, line.length()));
      }
      else if (line.length() > 0) // comment
      { 
        sect->AddIndent("[" + line, "");
      }
    }
  }
  
  ifile.close();
}

void 
MemIni
::PrintOut(std::ostream &os)
{ 
  os << std::endl;
  for (unsigned int i = 0; i < m_PreHeader->size(); ++i)
    os << (*m_PreHeader)[i] << std::endl;
    
  for (unsigned int i = 0; i < m_Sections->size(); ++i)
  { 
    MemIniSectionEntry *section = (*m_Sections)[i];    
    os << std::endl << "[" << section->GetSectionName() << "]" << std::endl; 
    MemIniSectionEntry::IndentMapType *indents = section->GetIndents();
    MemIniSectionEntry::IndentMapType::iterator it;
    
    for (it = indents->begin(); it != indents->end(); ++it)
    { 
      if (it->first.length() <= 0)
        continue;
      if (it->first[0] != '[') // no comment
        os << it->first << "=" << it->second << std::endl;
      else // comment
        os << it->first.substr(1, it->first.length() - 1) << std::endl;
    }
  }
  
  os << std::endl;
}

void 
MemIni
::Init()
{
  m_Filename = "";
  m_ContentChanged = false;
  m_Sections = new std::vector<MemIniSectionEntry *>;
  m_PreHeader = new std::vector<std::string>;
}

  
}
