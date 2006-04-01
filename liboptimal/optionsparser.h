#ifndef OPTIONSPARSER_H
#define OPTIONSPARSER_H

#include <string>
#include <list>

#ifdef _GNUC_
  #include <ext/hash_map>

  #define LIBOPTIMAL_API
#else
  #include <hash_map>
  #define __gnu_cxx std

  #ifdef LIBOPTIMAL_EXPORTS
    #define LIBOPTIMAL_API _declspec(dllexport)
  #else
    #define LIBOPTIMAL_API _declspec(dllexport)
  #endif
#endif

namespace optimal {

class LIBOPTIMAL_API OptionsParser {
  public:
    OptionsParser();
    OptionsParser(char newLine);
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
      optionType_subopts
    };
    char newLine;
    char assignment;
    
    __gnu_cxx::hash_map<std::string, optionType, StringHash> longOptionTypes;
    __gnu_cxx::hash_map<char, optionType> shortOptionTypes;
    
    __gnu_cxx::hash_map<std::string, bool*, StringHash> longBoolOptions;
    __gnu_cxx::hash_map<std::string, int*, StringHash> longIntOptions;
    __gnu_cxx::hash_map<std::string, std::string*, StringHash> longStringOptions;
    __gnu_cxx::hash_map<std::string, OptionsParser*, StringHash> longSuboptsOptions;
    
    __gnu_cxx::hash_map<char, bool*> shortBoolOptions;
    __gnu_cxx::hash_map<char, int*> shortIntOptions;
    __gnu_cxx::hash_map<char, std::string*> shortStringOptions;
    __gnu_cxx::hash_map<char, OptionsParser*> shortSuboptsOptions;
    std::list<std::string> errors;
  public:
    inline const std::list<std::string>& getErrors() const { return errors; }
    inline std::list<std::string>& getErrors() { return errors; }

    void addOption(const std::string& longName, char shortName, bool* value);
    void addOption(const std::string& longName, char shortName, int* value);
    void addOption(const std::string& longName, char shortName, std::string* value);
    void addOption(const std::string& longName, char shortName, OptionsParser* parser);

    bool parseStream(std::istream&, const std::string& errorPrefix);
    
    /** \brief Parse the given file (if it exists - ignore otherwise), and
     * command line.
     *
     * Return true if a problem occurs (which can be investigated
     * through errors) */
    bool parse(
        const std::string& configFile,
        int argc,
        char const* const* argv
      );
};

}

#endif // OPTIONSPARSER_H

