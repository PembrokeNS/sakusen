#include <optimal/optionsparser.h>

#include <algorithm>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace std;

namespace optimal {

OptionsParser::OptionsParser(char nl, char a, char c) :
  newLine(nl),
  assignment(a),
  comment(c)
{
}

void OptionsParser::checkOptionNamesSanity(
    const std::string& longName,
    char shortName
  ) const
{
  if (longName != "") {
    if (options.count(longName)) {
      throw std::logic_error(string("repeated long option name '"+longName+"'"));
    }

    if (boost::algorithm::starts_with(longName, "no-")) {
      throw std::invalid_argument(string("long option name cannot start 'no-'"));
    }
  }

  if (shortName != '\0') {
    if (options.get<1>().count(shortName)) {
      throw std::logic_error(
          string("repeated short option name '")+shortName+"'"
        );
    }

    if (shortName == '-') {
      throw std::invalid_argument(string("short option name cannot be '-'"));
    }
  }
}

/** \brief Parse the given stream
 *
 * \note This method is used by parse to parse the config file and to parse
 * suboptions, and it probably isn't needed for other purposes, but it is
 * provided in case it is needed (it could, for example, be used to parse
 * options in an environment variable)
 *
 * \param stream      The stream to parse
 * \param errorPrefix String with which to prefic errors (should give a hint to
 *                    the user as to the place where the stream came from, e.g.
 *                    a file name)
 *
 * \return true iff an error occurs (Use getErrors to see what the error(s)
 *         were).
 */
bool OptionsParser::parseStream(istream& stream, const string& errorPrefix)
{
  string line;

  while (getline(stream, line, newLine)) {
    /** \todo Line numbers in error messages */
    /*printf("parser processing line: %s\n", line.c_str());*/
    string::size_type commentPos;
    if (string::npos != (commentPos = line.find(comment, 0))) {
      line = line.substr(0, commentPos);
    }
    boost::trim(line);
    if (line.length() > 0) {
      string::size_type equalsPos = line.find(assignment, 0);
      if (equalsPos == string::npos) {
        /* We assume this should be a boolean option which is being set to
         * true */
        string optionName = line;
        OptionContainer::iterator optionIt = options.find(optionName);
        if (optionIt != options.end()) {
          if ((*optionIt)->isBoolean()) {
            (*optionIt)->setBoolean(true);
          } else {
            errors.push_back(
                errorPrefix+": no '"+assignment+"' character found on line "
                "and option not a boolean option"
              );
          }
        } else {
          errors.push_back(
              errorPrefix+": no '"+assignment+"' character found on line "
              "and not an option name"
            );
        }
      } else {
        string optionName = line.substr(0, equalsPos);
        string optionValue = line.substr(equalsPos + 1);
        boost::trim(optionName);
        boost::trim(optionValue);
        if (optionName.empty()) {
          errors.push_back(
              errorPrefix+": no option name found before '"+assignment+
              "' character"
            );
        } else {
          OptionContainer::iterator optionIt = options.find(optionName);

          if (optionIt != options.end()) {
            if ((*optionIt)->isBoolean()) {
              if (optionValue == "yes" || optionValue == "true" ||
                  optionValue == "1" || optionValue == "y") {
                (*optionIt)->setBoolean(true);
              } else if (optionValue == "no" || optionValue == "false" ||
                  optionValue == "0" || optionValue == "f") {
                (*optionIt)->setBoolean(false);
              } else {
                errors.push_back(
                    errorPrefix+": option value '"+optionValue+
                    "' not recognised as a boolean value"
                  );
              }
            } else {
              list<string> subErrors =
                (*optionIt)->setString(optionValue, errorPrefix);
              errors.splice(errors.end(), subErrors);
            }
          } else {
            errors.push_back(
                errorPrefix+": unrecognized option: '"+optionName+"'"
              );
          }
        }
      }
    }
  }

  if (!stream.eof()) {
    errors.push_back(errorPrefix+": error reading stream");
  }
  return !errors.empty();
}

/** \brief Parse the given file (if it exists - ignore otherwise), and
 * command line.
 *
 * \return true if a problem occurs (which can be investigated
 * through getErrors) */
bool OptionsParser::parse(
    const boost::filesystem::path& configFilePath,
    int argc,
    char const* const* argv
  )
{
  errors.clear();
  boost::filesystem::ifstream configFile(configFilePath);
  if (configFile.is_open()) {
    parseStream(configFile, configFilePath.native_file_string());
  }

  /* Now we process the command line */
  for (int i = 1; i<argc; i++) {
    const char* arg = argv[i];

    if (arg[0] == '-') {
      if (arg[1] == '-') {
        /* long option */
        if (boost::algorithm::starts_with(arg+2, "no-")) {
          /* negated boolean option */
          string optionName(arg+5);
          if (optionName.empty()) {
            errors.push_back("no option name after '--no-'");
          } else {
            OptionContainer::iterator optionIt = options.find(optionName);

            if (optionIt != options.end() && (*optionIt)->isBoolean()) {
              (*optionIt)->setBoolean(false);
            } else {
              errors.push_back(
                  "unrecognized boolean long option: '"+optionName+"'"
                );
            }
          }
        } else {
          string optionName(arg+2);
          if (optionName.empty()) {
            /** \todo this indicates non-option arguments */
            errors.push_back("no option name after '--'");
          } else {
            OptionContainer::iterator optionIt = options.find(optionName);

            if (optionIt != options.end()) {
              if ((*optionIt)->isBoolean()) {
                (*optionIt)->setBoolean(true);
              } else {
                if (++i < argc) {
                  list<string> subErrors = (*optionIt)->setString(
                      argv[i], "<command line>: --" + optionName
                    );
                  errors.splice(errors.end(), subErrors);
                } else {
                  errors.push_back(
                      string("trailing long option '")+optionName+
                      "' requires argument"
                    );
                }
              }
            } else {
              /* See if it's of the form --option=value */
              string::iterator assignIt =
                find(optionName.begin(), optionName.end(), assignment);

              if (assignIt == optionName.end()) {
                errors.push_back(
                    string("unrecognized long option: '")+optionName+"'"
                  );
              } else {
                string optionValue(assignIt+1, optionName.end());
                optionName.erase(assignIt, optionName.end());
                optionIt = options.find(optionName);

                if (optionIt != options.end()) {
                  if ((*optionIt)->isBoolean()) {
                    errors.push_back(
                        "boolean option '"+optionName+"'given argument"
                      );
                  } else {
                    list<string> subErrors = (*optionIt)->setString(
                        optionValue, "<command line>: --" + optionName
                      );
                    errors.splice(errors.end(), subErrors);
                  }
                } else {
                  errors.push_back(
                      string("unrecognized long option: '")+optionName+"'"
                    );
                }
              }
            }
          }
        }
      } else {
        /* short options */
        for (const char* optionChar=arg+1; *optionChar; optionChar++) {
          OptionContainer::index_iterator<ShortNameTag>::type optionIt =
            options.get<ShortNameTag>().find(*optionChar);
          if (optionIt != options.get<ShortNameTag>().end()) {
            if ((*optionIt)->isBoolean()) {
              if (optionChar[1] == '-') {
                (*optionIt)->setBoolean(false);
                ++optionChar;
              } else {
                (*optionIt)->setBoolean(true);
              }
            } else {
              if (optionChar[1] == '\0' && ++i < argc) {
                list<string> subErrors = (*optionIt)->setString(
                    argv[i], "<command line>: -" + *optionChar
                  );
                errors.splice(errors.end(), subErrors);
              } else {
                errors.push_back(
                    string("short option '")+*optionChar+
                    "' requires argument"
                  );
              }
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

}

