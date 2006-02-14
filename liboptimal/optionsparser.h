#ifndef OPTIONSPARSER_H
#define OPTIONSPARSER_H

#include <string>
#include <list>
#include <ext/hash_map>

namespace optimal {

class OptionsParser {
  public:
    OptionsParser();
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
      optionType_string
    };
    __gnu_cxx::hash_map<std::string, optionType, StringHash> longOptionTypes;
    __gnu_cxx::hash_map<char, optionType> shortOptionTypes;
    __gnu_cxx::hash_map<std::string, bool*, StringHash> longBoolOptions;
    __gnu_cxx::hash_map<std::string, int*, StringHash> longIntOptions;
    __gnu_cxx::hash_map<std::string, std::string*, StringHash> longStringOptions;
    __gnu_cxx::hash_map<char, bool*> shortBoolOptions;
    __gnu_cxx::hash_map<char, int*> shortIntOptions;
    __gnu_cxx::hash_map<char, std::string*> shortStringOptions;
    std::list<std::string> errors;
  public:
    inline const std::list<std::string>& getErrors() { return errors; }

    void addOption(const std::string& longName, char shortName, bool* value);
    void addOption(const std::string& longName, char shortName, int* value);
    void addOption(const std::string& longName, char shortName, std::string* value);
    
    /** \brief Parse the given file (if it exists - ignore otherwise), and
     * command line.
     *
     * Return true if a problem occurs (which can be investigated
     * through errors) */
    bool Parse(
        const std::string& configFile,
        int argc,
        char const* const* argv
      );
};

}

#endif // OPTIONSPARSER_H

