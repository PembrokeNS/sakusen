#ifndef OPTIONSPARSER_H
#define OPTIONSPARSER_H

#include <string>
#include <list>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/tokenizer.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/home/qi/parse.hpp>
#include <boost/spirit/home/qi/numeric/uint.hpp>

#if defined(_MSC_VER)
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
#elif (defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__ICC)
  #define LIBOPTIMAL_API __attribute__ ((visibility ("default")))
#else
  #error Unknown compiler
#endif

namespace optimal {

namespace detail {

/* Metafunctions to extract the underlying type of some generalized
 * pointer.
 * TypeUnderOptional strips off any boost::optional<> wrapper and converts
 * tribool to bool;
 * TypeUnderPointer strips off first a layer of pointer and then applies
 * TypeUnderOptional. */
template<typename T>
struct TypeUnderOptional {
  typedef T type;
};
template<typename T>
struct TypeUnderOptional<boost::optional<T> > {
  typedef T type;
};
template<>
struct TypeUnderOptional<boost::logic::tribool> {
  typedef bool type;
};

template<typename T>
struct TypeUnderPointer : public TypeUnderOptional<typename T::element_type> {};
template<typename T>
struct TypeUnderPointer<T*> : public TypeUnderOptional<T> {};

} /* Back into namespace optimal */

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
     * \param assignment Character to use for assigment, i.e. to delimit
     *                options and their values in config file or on command
     *                line (for the command line it is also always possible to
     *                use separate arguments rather than one with an
     *                assignment).  The default of '=' should suffice for most
     *                circumstances.
     * \param comment Character to use to indicate comments in config file.
     *                Anything from one of these characters to a \a newLine
     *                will be ignored.  The default of '#' should suffice for
     *                most circumstances.
     */
    explicit OptionsParser(
        char newLine = '\n',
        char assignment = '=',
        char comment = '#'
      );
  private:
    char newLine;
    char assignment;
    char comment;

    class Option : private boost::noncopyable {
      public:
        typedef boost::shared_ptr<Option> Ptr;

        virtual ~Option() {}

        std::string getLongName() const { return longName; }
        char getShortName() const { return shortName; }
        virtual bool isBoolean() const = 0;
        virtual void setBoolean(bool) = 0;
        virtual std::list<std::string> setString(
            const std::string&,
            const std::string& errorPrefix
          ) = 0;
      protected:
        Option(const std::string& lN, char sN) :
          longName(lN), shortName(sN)
        {}
      private:
        std::string longName;
        char shortName;
    };

    template<typename StoragePointer>
    class ValueOption : public Option {
      public:
        ValueOption(
            const std::string& longName,
            char shortName,
            StoragePointer v
          ) :
          Option(longName, shortName),
          value(v)
        {}
      private:
        typedef typename detail::TypeUnderPointer<StoragePointer>::type T;
        StoragePointer value;

        bool isBoolean() const { return boost::is_same<T, bool>::value; }

        /* The template parameters U will always equal T, but we need it to be
         * here to take advantage of SFINAE */
        template<typename U>
        void internalSetBoolean(
            bool b,
            typename boost::enable_if<boost::is_same<U, bool>, int>::type = 0
          ) const {
          *value = b;
        }
        template<typename U>
        void internalSetBoolean(
            bool,
            typename boost::disable_if<boost::is_same<U, bool>, int>::type = 0
          ) const {
          throw std::logic_error("using setBoolean on non-boolean option");
        }
        void setBoolean(bool b) { internalSetBoolean<T>(b); }
        
        template<typename U>
        std::list<std::string> internalSetString(
            const std::string& s,
            const std::string& errorPrefix,
            typename
              boost::enable_if<boost::is_same<U, OptionsParser>, int>::type = 0
          ) const {
          std::istringstream is(s);
          (*value).parseStream(is, errorPrefix);
          return (*value).getErrors();
        }
        template<typename U>
        std::list<std::string> internalSetString(
            const std::string& s,
            const std::string& errorPrefix,
            typename
              boost::enable_if<boost::is_unsigned<U>, int>::type = 0
          ) const {
          namespace qi = boost::spirit::qi;
          std::list<std::string> errors;
          U val;
          std::string::const_iterator st = s.begin();
          qi::uint_spec<U, 10, 1, -1> parser;
          if (qi::parse(st, s.end(), parser, val) && st == s.end()) {
            *value = val;
          } else {
            errors.push_back(
                "couldn't interpret '"+s+"' as a value of the required type"
              );
          }
          return errors;
        }
        template<typename U>
        std::list<std::string> internalSetString(
            const std::string& s,
            const std::string& /*errorPrefix*/,
            typename
              boost::disable_if<
                typename boost::mpl::or_<
                  boost::is_same<U, OptionsParser>,
                  boost::is_unsigned<U>
                >::type, int
              >::type =0
          ) const {
          std::list<std::string> errors;
          try {
            *value = boost::lexical_cast<T>(s);
          } catch (boost::bad_lexical_cast&) {
            errors.push_back(
                "couldn't interpret '"+s+"' as a value of the required type"
              );
          }
          return errors;
        }
        std::list<std::string> setString(
            const std::string& s,
            const std::string& errorPrefix
          ) {
          return internalSetString<T>(s, errorPrefix);
        }
    };

    template<typename StoragePointer>
    class ListOption : public Option {
      public:
        ListOption(
            const std::string& longName,
            char shortName,
            StoragePointer v,
            const std::string& s,
            bool a,
            boost::empty_token_policy eP
          ) :
          Option(longName, shortName),
          value(v),
          separators(s),
          append(a),
          emptyPolicy(eP)
        {}
      private:
        typedef typename detail::TypeUnderPointer<StoragePointer>::type T;
        StoragePointer value;
        std::string separators;
        bool append;
        boost::empty_token_policy emptyPolicy;

        bool isBoolean() const { return boost::is_same<T, bool>::value; }
        void setBoolean(bool) {
          throw std::logic_error("using setBoolean on non-boolean option");
        }
        std::list<std::string> setString(
            const std::string& s,
            const std::string& /*errorPrefix*/
          ) {
          boost::tokenizer<boost::char_separator<char> > tokenizer(
              s, boost::char_separator<char>(separators.c_str(),"",emptyPolicy)
            );
          if (!append) {
            (*value).clear();
          }
          std::copy(tokenizer.begin(), tokenizer.end(), back_inserter(*value));
          return std::list<std::string>();
        }
    };

    struct ShortNameTag;
    
    typedef boost::multi_index_container<
        Option::Ptr,
        boost::multi_index::indexed_by<
          boost::multi_index::ordered_non_unique<
            BOOST_MULTI_INDEX_CONST_MEM_FUN(Option, std::string, getLongName)
          >,
          boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<ShortNameTag>,
            BOOST_MULTI_INDEX_CONST_MEM_FUN(Option, char, getShortName)
          >
        >
      > OptionContainer;

    OptionContainer options;
    std::list<std::string> errors;

    void checkOptionNamesSanity(
        const std::string& longName,
        char shortName
      ) const;
  public:
    /** \brief Errors accessor
     *
     * If parse returns true to indicate error, human-readable error messages
     * for everything that went wrong can be obtained through getErrors().
     */
    inline const std::list<std::string>& getErrors() const { return errors; }

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
    template<typename StoragePointer>
    void addOption(
        const std::string& longName,
        char shortName,
        StoragePointer value
      );
    template<typename StoragePointer>
    void addOption(
        const std::string& longName,
        char shortName,
        StoragePointer value,
        const std::string& separators,
        bool append = false,
        boost::empty_token_policy emptyPolicy = boost::drop_empty_tokens
      );
    //@}

    bool parseStream(std::istream&, const std::string& errorPrefix);
    
    bool parse(
        const boost::filesystem::path& configFile,
        int argc,
        char const* const* argv
      );

    bool parse(int argc, char const* const* argv) {
      return parse(boost::filesystem::path(), argc, argv);
    }
};

template<typename StoragePointer>
void OptionsParser::addOption(
    const std::string& longName,
    char shortName,
    StoragePointer value
  )
{
  checkOptionNamesSanity(longName, shortName);

  Option::Ptr option(
      new ValueOption<StoragePointer>(longName, shortName, value)
    );

  options.insert(option);
}

template<typename StoragePointer>
void OptionsParser::addOption(
    const std::string& longName,
    char shortName,
    StoragePointer value,
    const std::string& separators,
    bool append,
    boost::empty_token_policy emptyPolicy
  )
{
  checkOptionNamesSanity(longName, shortName);

  Option::Ptr option(new ListOption<StoragePointer>(
        longName, shortName, value, separators, append, emptyPolicy
    ));

  options.insert(option);
}

}

#endif // OPTIONSPARSER_H

