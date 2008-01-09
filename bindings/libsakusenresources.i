%module sakusenresources
%naturalvar;
%feature("autodoc","1");
%import "libsakusen.i"
%include "libsakusen-resources-global.h"
%{
#include "libsakusen-resources-global.h"
%}
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
      typedef bool (*name_check)( const std::string & name );

      // compiler generates copy constructor, copy assignment, and destructor

      path(){}

      path( const std::string & src );
      path( const char * src );

      path( const std::string & src, name_check checker );
      path( const char * src, name_check checker );

      // append operations:
      path & operator /=( const path & rhs );
      /*path operator /( const path & rhs ) const
        { return path( *this ) /= rhs; }*/

      // modification functions:
      path & normalize();

      // conversion functions:
      const std::string & string() const { return m_path; }
      std::string native_file_string() const;
      std::string native_directory_string() const;

      // decomposition functions:
      path         root_path() const;
      std::string  root_name() const;
      std::string  root_directory() const;
      path         relative_path() const;
      std::string  leaf() const;
      path         branch_path() const;

      // query functions:
      bool empty() const { return m_path.empty(); } // name consistent with std containers

      bool is_complete() const;

      bool has_root_path() const;
      bool has_root_name() const;
      bool has_root_directory() const;
      bool has_relative_path() const;
      bool has_leaf() const { return !m_path.empty(); }
      bool has_branch_path() const;

      // default name_check mechanism:
      static bool default_name_check_writable(); 
      static void default_name_check( name_check new_check );
      static name_check default_name_check();

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
      void m_replace_leaf( const char * new_leaf );*/
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
    bool no_check( const std::string & name );   // always returns true
    bool native( const std::string & name );
      // native(name) must return true for any name which MIGHT be valid
      // on the native platform.

  } // namespace filesystem


template<class T> class shared_ptr{
  public:
    shared_ptr(shared_ptr<T> const &);
    T* get() const;
    T* operator-> () const;
};

template<
    typename T0,
    typename T1 = boost::tuples::null_type,
    typename T2 = boost::tuples::null_type,
    typename T3 = boost::tuples::null_type,
    typename T4 = boost::tuples::null_type,
    typename T5 = boost::tuples::null_type,
    typename T6 = boost::tuples::null_type
  >
class tuple {
  public:
    T0 get_head();
    tuple<T1, T2, T3, T4, T5, T6> get_tail();
};

template<typename T0>
class tuple<
    T0,
    boost::tuples::null_type,
    boost::tuples::null_type,
    boost::tuples::null_type,
    boost::tuples::null_type,
    boost::tuples::null_type,
    boost::tuples::null_type
  >
{
  public:
    T0 get_head();
    boost::tuples::null_type get_tail();
};

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

%template(tupleUniverseConstPtrResourceSearchResultString) tuple<sakusen::Universe::ConstPtr,sakusen::ResourceSearchResult,String>;
%template(tupleUniversePtrResourceSearchResultString) tuple<sakusen::Universe::Ptr,sakusen::ResourceSearchResult,String>;
%template(tupleResourceSearchResultString) tuple<sakusen::ResourceSearchResult,String>;
%template(tupleString) tuple<String>;


} // namespace boost
%{
#include "boost/filesystem/path.hpp"
%}
%include "fileutils.h"
%{
#include "fileutils.h"
%}
%include "fileresourceinterface.h"
%{
#include "fileresourceinterface.h"
%}
