#include "optionsparser.h"

#include <string.h>

#include <fstream>

using namespace std;

using namespace optimal;

OptionsParser::OptionsParser() :
  /* Initialize hash_maps with few buckets because we are unlikely to need many
   * */
  longOptionTypes(10),
  shortOptionTypes(10),
  longBoolOptions(10),
  longIntOptions(10),
  longStringOptions(10),
  shortBoolOptions(10),
  shortIntOptions(10),
  shortStringOptions(10)
{
}

void OptionsParser::addOption(
    const std::string& longName,
    char shortName,
    bool* value
  )
{
  assert(0 == longOptionTypes.count(longName));
  assert(0 == shortOptionTypes.count(shortName));

  longOptionTypes[longName] = optionType_bool;
  shortOptionTypes[shortName] = optionType_bool;

  longBoolOptions[longName] = value;
  shortBoolOptions[shortName] = value;
}

void OptionsParser::addOption(
    const string& longName,
    char shortName,
    int* value
  )
{
  assert(0 == longOptionTypes.count(longName));
  assert(0 == shortOptionTypes.count(shortName));

  longOptionTypes[longName] = optionType_int;
  shortOptionTypes[shortName] = optionType_int;

  longIntOptions[longName] = value;
  shortIntOptions[shortName] = value;
}

void OptionsParser::addOption(
    const string& longName,
    char shortName,
    string* value
  )
{
  assert(0 == longOptionTypes.count(longName));
  assert(0 == shortOptionTypes.count(shortName));

  longOptionTypes[longName] = optionType_string;
  shortOptionTypes[shortName] = optionType_string;

  longStringOptions[longName] = value;
  shortStringOptions[shortName] = value;
}

void trim(std::string& s)
{
  while (!s.empty() && string(" \n\t").find(s[0]) != string::npos) {
    s.erase(s.begin());
  }
  while (!s.empty() && string(" \n\t").find(s[s.length()-1]) != string::npos) {
    s.erase(--s.end());
  }
}

bool OptionsParser::Parse(
    const string& configFileName,
    int argc,
    char const* const* argv
  )
{
  errors.clear();
  ifstream configFile(configFileName.c_str());
  if (configFile.is_open()) {
    string line;
    getline(configFile, line);

    while (!configFile.eof()) {
      /* TODO: line numbers in error messages */
      /*printf("parser processing line: %s\n", line.c_str());*/
      string::size_type commentPos;
      if (string::npos != (commentPos = line.find('#', 0))) {
        line = line.substr(0, commentPos);
      }
      if (line.length() > 0) {
        string::size_type equalsPos = line.find('=', 0);
        if (equalsPos == string::npos) {
          errors.push_back(configFileName+": no '=' character found on line");
        } else {
          string optionName = line.substr(0, equalsPos);
          string optionValue = line.substr(equalsPos + 1);
          trim(optionName);
          trim(optionValue);
          if (longOptionTypes.count(optionName)) {
            switch(longOptionTypes[optionName]) {
              case optionType_bool:
                assert(longBoolOptions.count(optionName));
                if (optionValue == "yes" || optionValue == "true" ||
                    optionValue == "1") {
                  *longBoolOptions[optionName] = true;
                } else if (optionValue == "no" || optionValue == "false" ||
                    optionValue == "0") {
                  *longBoolOptions[optionName] = false;
                } else {
                  errors.push_back(
                      configFileName+": option value '"+optionValue+
                      "' not recognised as a boolean value"
                    );
                }
                break;
              default:
                abort();
            }
          } else {
            errors.push_back(
                configFileName+": unrecognized option: '"+optionName+"'"
              );
          }
        }
      }
      getline(configFile, line);
    }
  }

  /* Now we process the command line */
  for (int i = 1; i<argc; i++) {
    const char* arg = argv[i];

    if (arg[0] == '-') {
      if (arg[1] == '-') {
        /* long option */
        if (strlen(arg+2) > 3 && 0 == strncmp(arg+2, "no-", 3)) {
          /* negated boolean option */
          string optionName(arg+5);
          if (longBoolOptions.count(optionName)) {
            *longBoolOptions[optionName] = false;
          } else {
            errors.push_back(
                string("unrecognized boolean long option: '")+optionName+"'"
              );
          }
        } else {
          string optionName(arg+2);
          if (longOptionTypes.count(optionName)) {
            switch (longOptionTypes[optionName]) {
              case optionType_bool:
                assert(longBoolOptions.count(optionName));
                *longBoolOptions[optionName] = true;
                break;
              case optionType_int:
                assert(longIntOptions.count(optionName));
                if (++i < argc) {
                  *longIntOptions[optionName] = atoi(argv[++i]);
                } else {
                  errors.push_back(
                      string("trailing long option '")+optionName+
                      "' requires argument"
                    );
                }
                break;
              case optionType_string:
                assert(longStringOptions.count(optionName));
                if (++i < argc) {
                  *longStringOptions[optionName] = atoi(argv[++i]);
                } else {
                  errors.push_back(
                      string("trailing long option '")+optionName+
                      "' requires argument"
                    );
                }
                break;
              default:
                abort();
            }
          } else {
            errors.push_back(
                string("unrecognized long option: '")+optionName+"'"
              );
          }
        }
      } else {
        /* short options */
        for (const char* optionChar=arg+1; *optionChar; optionChar++) {
          if (shortOptionTypes.count(*optionChar)) {
            switch (shortOptionTypes[*optionChar]) {
              case optionType_bool:
                assert(shortBoolOptions.count(*optionChar));
                if (optionChar[1] == '-') {
                  *shortBoolOptions[*optionChar] = false;
                  ++optionChar;
                } else {
                  *shortBoolOptions[*optionChar] = true;
                }
                break;
              case optionType_int:
                assert(shortIntOptions.count(*optionChar));
                if (optionChar[1] == '\0' && ++i < argc) {
                  *shortIntOptions[*optionChar] = atoi(argv[++i]);
                } else {
                  errors.push_back(
                      string("short option '")+*optionChar+
                      "' requires argument"
                    );
                }
                break;
              case optionType_string:
                assert(shortStringOptions.count(*optionChar));
                if (optionChar[1] == '\0' && ++i < argc) {
                  *shortStringOptions[*optionChar] = atoi(argv[++i]);
                } else {
                  errors.push_back(
                      string("short option '")+*optionChar+
                      "' requires argument"
                    );
                }
                break;
              default:
                abort();
            }
          } else {
            errors.push_back(
                string("unrecognized short option character: '")+*optionChar+"'"
              );
          }
        }
      }
    } else {
      errors.push_back(string("argument '")+arg+"' not an option");
    }
  }

  return !errors.empty();
}

