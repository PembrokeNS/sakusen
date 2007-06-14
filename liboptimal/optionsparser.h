#ifndef OPTIONSPARSER_H
#define OPTIONSPARSER_H

#include <string>
#include <list>

#if defined(_MSC_VER)
  #include <hash_map>
  #define __gnu_cxx std
  /* This warning is about members of exported classes not being exported.
   * This is mostly to do with stuff from std, but in at least one case it 
   * was very important.  In the long run we should do without disabling 
   * this warning. */
  #pragma warning(disable: 4251)
  #ifdef LIBOPTIMAL_EXPORTS
    #define LIBOPTIMAL_API _declspec(dllexport)
  #else
    #define LIBOPTIMAL_API _declspec(dllimport)
  #endif
#elif defined(__GNUC__) && (__GNUC__ >= 4)
  #include <ext/hash_map>

  #define LIBOPTIMAL_API __attribute__ ((visibility ("default")))
#else
  #error Unknown compiler
#endif

namespace optimal {

/** \brief Facilitates parsing of program options in config files or on the
 * command line.
 *
 * An OptionsParser is initialised and then has various options added to it.
 * Once all the options are in place it can be used to parse program options
 * from various sources.
 *
 * An OptionsParser can also be used to parse suboptions as part of another
 * OptionsParser.
 */
class LIBOPTIMAL_API OptionsParser {
  public:
    /** \brief Standard constructor
     *
     * Creates an OptionsParser to which options can be added.
     *
     * \param newLine Character to use to separate options.  For a top-level
     *                OptionsParser this applies to reading the config file,
     *                and the default value of '\n' should suffice.  For
     *                suboptions parsers this will be what separates the
     *                suboptions in wither the config file or the cammand line,
     *                and thus should not be '\n'.  ',', '/' and ':' are
     *                popular choices.
     */
    OptionsParser(char newLine = '\n');
  private:
    /* A hash function for Strings */
    class StringHash {
      private:
        __gnu_cxx::hash<const char*> hasher;
      public:
        inline size_t operator() (const std::string& s) const {
          return hasher(s.c_str());
        }
    };
    enum optionType {
      optionType_bool,
      optionType_int,
      optionType_string,
      optionType_stringList,
      optionType_subopts
    };
    char newLine;
    char assignment;
    
    __gnu_cxx::hash_map<std::string, optionType, StringHash> longOptionTypes;
    __gnu_cxx::hash_map<char, optionType> shortOptionTypes;
    
    __gnu_cxx::hash_map<std::string, bool*, StringHash> longBoolOptions;
    __gnu_cxx::hash_map<std::string, int*, StringHash> longIntOptions;
    __gnu_cxx::hash_map<std::string, std::string*, StringHash> longStringOptions;
    __gnu_cxx::hash_map<std::string, std::pair<char,std::list<std::string>*>, StringHash> longStringListOptions;
    __gnu_cxx::hash_map<std::string, OptionsParser*, StringHash> longSuboptsOptions;
    
    __gnu_cxx::hash_map<char, bool*> shortBoolOptions;
    __gnu_cxx::hash_map<char, int*> shortIntOptions;
    __gnu_cxx::hash_map<char, std::string*> shortStringOptions;
    __gnu_cxx::hash_map<char, std::pair<char,std::list<std::string>*> > shortStringListOptions;
    __gnu_cxx::hash_map<char, OptionsParser*> shortSuboptsOptions;
    std::list<std::string> errors;
  public:
    /** \name Errors accessors
     *
     * If parse returns true to indicate error, human-readable error messages
     * for everything that went wrong can be obtained through getErrors().
     */
    //@{
    inline const std::list<std::string>& getErrors() const { return errors; }
    inline std::list<std::string>& getErrors() { return errors; }
    //@}

    /** \name Option registration functions
     *
     * Each of these functions allows a particular type of option to be added
     * to the parser; such options will then be recognised at parse time.
     *
     * Each function takes a long name ("" for none), short name ('\\0' for
     * none) to allow the user to specify the option, and a pointer to a place
     * where the value should be stored as options are parsed.  Caller must
     * ensure that this pointer remains valid until all parsing is complete.
     */
    //@{
    void addOption(const std::string& longName, char shortName, bool* value);
    void addOption(const std::string& longName, char shortName, int* value);
    void addOption(const std::string& longName, char shortName, std::string* value);
    void addOption(const std::string& longName, char shortName, std::list<std::string>* value, char separator);
    void addOption(const std::string& longName, char shortName, OptionsParser* parser);
    //@}

    bool parseStream(std::istream&, const std::string& errorPrefix);
    
    bool parse(
        const std::string& configFile,
        int argc,
        char const* const* argv
      );
};

}

#endif // OPTIONSPARSER_H

