%module sakusen_resources
%naturalvar;
#ifdef _WIN32
  %include <windows.i>
#endif

%feature("autodoc","1");
%{
#include <string>
%}
%{
/* Needed because libsakusen.i includes a weapontypes traits fragment */
namespace sakusen{
class WeaponType;
}
%}
%import "libsakusen.i"
%include "sakusen/resources/global.h"
%{
#include "sakusen/resources/global.h"
%}
%{
#include "pcrecpp.h"
%}
%{
#include "boost/filesystem/path.hpp"
%}
/*The following code snippet might be GPLed, if it's substantial enough (independently of
boost itself) to be copyrightable at all. It was taken from the ArX sources; the ArX COPYING
file claims that detailed authorship information is given in the copyright statement of each
file, but the file in question has no copyright statement. */
/* This software is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as
published by the Free Software Foundation. */
/* We have to include our own version of boost/filesystem/path.hpp
 because the included definition has a nested class.*/

namespace boost
{
  namespace filesystem
  {
    class directory_iterator;


  //  path -------------------------------------------------------------------//

    class path
    {
    public:
      // compiler generates copy constructor, copy assignment, and destructor

      path(){}

      path( const std::string & src );

      // append operations:
      path & operator /=( const path & rhs );
      /*path operator /( const path & rhs ) const
        { return path( *this ) /= rhs; }*/

      // modification functions:
      path & normalize();

      // conversion functions:
      const std::string & string() const { return m_path; }
      
      // decomposition functions:
      path         root_path() const;
      path  root_name() const;
      path  root_directory() const;
      path         relative_path() const;
      path  filename() const;

      // query functions:
      bool empty() const { return m_path.empty(); } // name consistent with std containers

      bool has_root_path() const;
      bool has_root_name() const;
      bool has_root_directory() const;
      bool has_relative_path() const;
      bool has_filename() const { return !m_path.empty(); }
      bool has_parent_path() const;

    private:
      // Note: This is an implementation for POSIX and Windows, where there
      // are only minor differences between generic and system-specific
      // constructor input formats.  Private members might be quite different
      // in other implementations, particularly where there were wide
      // differences between generic and system-specific argument formats,
      // or between native_file_string() and native_directory_string() formats.

      std::string  m_path;

      friend class directory_iterator;
      // Was qualified; como433beta8 reports:
      //    warning #427-D: qualified name is not allowed in member declaration 
    /*public: // should be private, but friend functions dont work for me
      void m_path_append( const std::string & src, name_check checker );
      void m_replace_filename( const char * new_filename );*/
    };

  //  path non-member functions  ---------------------------------------------//

    /*inline path operator / ( const char * lhs, const path & rhs )
      { return path( lhs ) /= rhs; }

    inline path operator / ( const std::string & lhs, const path & rhs )
      { return path( lhs ) /= rhs; }*/
   
  //  path::name_checks  ---------------------------------------------------//

    bool portable_posix_name( const std::string & name );
    bool windows_name( const std::string & name );
    bool portable_name( const std::string & name );
    bool portable_directory_name( const std::string & name );
    bool portable_file_name( const std::string & name );
    bool native( const std::string & name );
      // native(name) must return true for any name which MIGHT be valid
      // on the native platform.

  } // namespace filesystem

/*GPL code ends here (apart from the line with the closing brace for namespace boost) */

#ifdef SWIGPYTHON
%pythoncode %{
def g(self,key):
  """Helper function to get item from a tuple"""
  w=self
  for i in range(key):
    w=w.get_tail()
    if(not 'get_head' in dir(w)):
      raise IndexError()
  return w.get_head()
%}
#endif

  %template(tupleUniverseConstPtrResourceSearchResultString) tuple<sakusen::Universe::ConstPtr,sakusen::ResourceSearchResult,sakusen::String>;
  %template(tupleUniversePtrResourceSearchResultString) tuple<sakusen::Universe::Ptr,sakusen::ResourceSearchResult,sakusen::String>;
  %template(tupleResourceSearchResultString) tuple<sakusen::ResourceSearchResult,sakusen::String>;
  %template(tupleString) tuple<sakusen::String>;


} // namespace boost
%include "sakusen/resources/fileresourceinterface.h"
%{
#include "sakusen/resources/fileresourceinterface.h"
%}

