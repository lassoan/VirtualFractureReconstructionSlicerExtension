

#ifndef IBIAMEMINISECTIONENTRY_H
#define IBIAMEMINISECTIONENTRY_H


#include <string>
#include <cstring>
#include <map>


namespace ibia 
{


/** \class MemIniSectionEntry
 *  \brief This class implements the abstraction of an INI file section.
 * 
 * <p>This class is implemented to abstract from typical (Windows) INI file
 * sections and provides access to them. It more or less encapsulates the
 * indents with their values and/or optional comments of a section.</p>
 *
 * @author Philipp Steininger <philipp.steininger@umit.at>
 * @version 1.0
 */
class MemIniSectionEntry
{
public:
  /** Structure with central operator for sorting the indents within the
    * internal map. **/
  struct AscStringComp
  { 
    /** Just an ordinary string-literal-comparator. **/
    bool operator()(const std::string s1, const std::string s2) const
    {
      return (strcmp(s1.c_str(), s2.c_str()) < 0);
    }
  };
  /** Map-type of the internal map used for indent-/comment-management. **/
  typedef std::map<std::string, std::string, AscStringComp> IndentMapType;

  /**
    * Construct new logical INI file section instance.
    * @param sectionName name of the section (it is expected to be unique
    * within the INI file)
    */
  MemIniSectionEntry(std::string sectionName);  
  /**
    * Destroy this logical INI file section instance.
    */
  ~MemIniSectionEntry();
  
  /**
    * Adds an indent (or comment) to the logical space of this section.
    * @param indent name of the new indent (indents starting with the [ 
    * character are comments - then value parameter is ignored and the rest
    * of indent parameter after [ character is expected to be the comment
    * itself)
    * @param value value of the indent (can also be the empty string)
    */
  void AddIndent(std::string indent, std::string value);
  /**
    * Deletes the specified indent (or comment) from the logical space of this
    * section.
    * @param indent the indent to be deleted (if found in logical section space)
    */
  void DeleteIndent(std::string indent);
  /**
    * @return returns a pointer to internal indents-map containing the indents
    * together with their values and optional comments
    */
  IndentMapType *GetIndents();
  /**
    * @return returns the name of this logical section space as string
    */
  std::string GetSectionName();

private:
  /** holds the name of this logical section space **/  
  std::string m_SectionName;
  /** internal indents-map containing the indents, their values and comments **/
  IndentMapType *m_Indents;    
};


}


#endif
