#include "optionsparser.h"
#define BOOST_TEST_MODULE OptionsParser test
#include <boost/test/unit_test.hpp>
#include <boost/optional.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace optimal;

template<typename T, size_t size>
inline size_t lenof(T(&) [size]) {
  return size;
}

template<size_t size>
void checkParseGivesErrors(
    const boost::filesystem::path& file,
    const char* (&argv)[size],
    OptionsParser& p,
    const string& message,
    size_t numErrors = 1
  )
{
  bool result = p.parse(file, size, argv);

  BOOST_CHECK_MESSAGE(result, message << ": returned false");
  BOOST_CHECK_MESSAGE(
      p.getErrors().size() == numErrors, message << ": wrong number of errors"
    );
}

BOOST_AUTO_TEST_CASE(options_parser_errors)
{
  {
    string s;
    OptionsParser p;
    BOOST_CHECK_THROW(p.addOption("", '-', &s), std::invalid_argument);
  }
  {
    string s;
    OptionsParser p;
    BOOST_CHECK_THROW(p.addOption("no-foo", '\0', &s), std::invalid_argument);
  }
  {
    const char* argv[] = { "foo", "-x" };
    
    OptionsParser p;
    checkParseGivesErrors("", argv, p, "detecting invalid short option");
  }
  {
    const char* argv[] = { "foo", "-x-" };
    
    OptionsParser p;
    checkParseGivesErrors(
        "", argv, p, "detecting invalid negated short option", 2
      );
  }
  {
    const char* argv[] = { "foo", "--x" };
    
    OptionsParser p;
    checkParseGivesErrors("", argv, p, "detecting invalid long option");
  }
  {
    const char* argv[] = { "foo", "--x=y" };
    
    OptionsParser p;
    checkParseGivesErrors(
        "", argv, p, "detecting invalid long option with argument"
      );
  }
  {
    const char* argv[] = { "foo", "--b=y" };
    bool b;
    
    OptionsParser p;
    p.addOption("b", '\0', &b);
    checkParseGivesErrors(
        "", argv, p, "detecting boolean long option with argument"
      );
  }
  {
    const char* argv[] = { "foo", "--no-x" };
    
    OptionsParser p;
    checkParseGivesErrors("", argv, p, "detecting invalid negated long option");
  }
  {
    const char* argv[] = { "foo", "--no-s" };
    string s;
    
    OptionsParser p;
    p.addOption("s", '\0', &s);
    checkParseGivesErrors(
        "", argv, p, "detecting invalid negation of long option"
      );
  }
  {
    const char* argv[] = { "foo", "-s" };
    string s;
    
    OptionsParser p;
    p.addOption("", 's', &s);
    checkParseGivesErrors("", argv, p, "detecting argumentless short option");
  }
  {
    const char* argv[] = { "foo", "--s" };
    string s;
    
    OptionsParser p;
    p.addOption("s", '\0', &s);
    checkParseGivesErrors("", argv, p, "detecting argumentless long option");
  }
}

BOOST_AUTO_TEST_CASE(bool_option_test)
{
  for (int test = 0; test<3; ++test) {
    bool bool0 = false;
    bool bool1 = true;
    bool bool2 = false;
    bool bool3 = true;
    boost::logic::tribool tribool4 = boost::logic::indeterminate;
    boost::logic::tribool tribool5 = boost::logic::indeterminate;
    boost::logic::tribool tribool6 = boost::logic::indeterminate;

    OptionsParser p;
    p.addOption("bool0", '0', &bool0);
    p.addOption("bool1", '1', &bool1);
    p.addOption("bool2", '2', &bool2);
    p.addOption("bool3", '3', &bool3);
    p.addOption("tribool4", '4', &tribool4);
    p.addOption("tribool5", '5', &tribool5);
    p.addOption("tribool6", '6', &tribool6);

    bool result = true;

    switch (test) {
      case 0:
        {
          const char* argv[] = {
            "foo", "-2", "-3-", "-5", "-6-"
          };
          result = p.parse("", lenof(argv), argv);
        }
        break;
      case 1:
        {
          const char* argv[] = {
            "foo", "--bool2", "--no-bool3", "--tribool5", "--no-tribool6"
          };
          result = p.parse("", lenof(argv), argv);
        }
        break;
      case 2:
        {
          result = p.parse("bool_option_test", 0, NULL);
        }
        break;
      default:
        throw logic_error("unexpected test number");
    }

    BOOST_CHECK_EQUAL(result, false);
    BOOST_CHECK_EQUAL(bool0, false);
    BOOST_CHECK_EQUAL(bool1, true);
    BOOST_CHECK_EQUAL(bool2, true);
    BOOST_CHECK_EQUAL(bool3, false);
    BOOST_CHECK(boost::logic::indeterminate(tribool4));
    BOOST_CHECK(tribool5);
    BOOST_CHECK(!tribool6);
  }
}

BOOST_AUTO_TEST_CASE(string_option_test)
{
  for (int test = 0; test<4; ++test) {
    const string initialValue = "initial";
    string setValue = "--A new value";
    string string0 = initialValue;
    string string1 = initialValue;
    boost::optional<string> optString2;
    boost::optional<string> optString3;

    OptionsParser p;
    p.addOption("string0", '0', &string0);
    p.addOption("string1", '1', &string1);
    p.addOption("optString2", '2', &optString2);
    p.addOption("optString3", '3', &optString3);

    bool result = true;

    switch (test) {
      case 0:
        {
          const char* argv[] = {
            "foo", "-1", setValue.c_str(), "-3", setValue.c_str()
          };
          result = p.parse("", lenof(argv), argv);
        }
        break;
      case 1:
        {
          const char* argv[] = {
            "foo", "--string1", setValue.c_str(),
            "--optString3", setValue.c_str()
          };
          result = p.parse("", lenof(argv), argv);
        }
        break;
      case 2:
        {
          string arg1 = "--string1="+setValue;
          string arg2 = "--optString3="+setValue;
          const char* argv[] = {
            "foo", arg1.c_str(), arg2.c_str()
          };
          result = p.parse("", lenof(argv), argv);
        }
        break;
      case 3:
        {
          result = p.parse("string_option_test", 0, NULL);
        }
        break;
      default:
        throw logic_error("unexpected test number");
    }

    BOOST_REQUIRE(!result == p.getErrors().empty());

    {
      /* Construct this error in silly way to only do it when
       * !p.getErrors().empty() */
      ostringstream error;
      if (result) {
        error << "In subtest " << test << ", failed with error " <<
          p.getErrors().front();
      }
      BOOST_CHECK_MESSAGE(result == false, error.str());
    }

    BOOST_CHECK_EQUAL(string0, initialValue);
    BOOST_CHECK_MESSAGE(
        string1 == setValue, "In subtest "<<test<<", string1 set to "<<string1
      );
    BOOST_CHECK(!optString2);
    BOOST_REQUIRE(optString3);
    BOOST_CHECK_EQUAL(*optString3, setValue);
  }
}

template<typename Integer>
void checkIntegerOptions()
{
  BOOST_STATIC_ASSERT(boost::is_integral<Integer>::value);

  for (int test = 0; test<3; ++test) {
    for (int setValueIndex = 0; setValueIndex<6; ++setValueIndex) {
      const Integer initialValue = 1;
      string setValue;
      
      switch (setValueIndex) {
        case 0:
          setValue = "0";
          break;
        case 1:
          setValue = "-1";
          break;
        case 2:
          setValue = "256";
          break;
        case 3:
          setValue = "32768"; // 2^15
          break;
        case 4:
          setValue = "-2147483649"; // -2^31-1
          break;
        case 5:
          setValue = "18446744073709551616"; // 2^64
          break;
        default:
          throw logic_error("unexpected value index");
      }
      
      Integer int0 = initialValue;
      Integer int1 = initialValue;
      boost::optional<Integer> optInt2;
      boost::optional<Integer> optInt3;

      OptionsParser p;
      p.addOption("int0", '0', &int0);
      p.addOption("int1", '1', &int1);
      p.addOption("optInt2", '2', &optInt2);
      p.addOption("optInt3", '3', &optInt3);

      bool result = true;

      switch (test) {
        case 0:
          {
            const char* argv[] = {
              "foo", "-1", setValue.c_str(), "-3", setValue.c_str()
            };
            result = p.parse("", lenof(argv), argv);
          }
          break;
        case 1:
          {
            const char* argv[] = {
              "foo", "--int1", setValue.c_str(),
              "--optInt3", setValue.c_str()
            };
            result = p.parse("", lenof(argv), argv);
          }
          break;
        case 2:
          {
            string arg1 = "--int1="+setValue;
            string arg2 = "--optInt3="+setValue;
            const char* argv[] = { "foo", arg1.c_str(), arg2.c_str() };
            result = p.parse("", lenof(argv), argv);
          }
          break;
        default:
          throw logic_error("unexpected test number");
      }

      bool shouldSucceed;

      try {
        shouldSucceed = (setValue ==
            boost::lexical_cast<string>(boost::lexical_cast<Integer>(setValue))
          );
      } catch (boost::bad_lexical_cast&) {
        shouldSucceed = false;
      }

      BOOST_REQUIRE(!result == p.getErrors().empty());

      if (shouldSucceed) {
        {
          /* Construct this error in silly way to only do it when
           * !p.getErrors().empty() */
          ostringstream error;
          if (result) {
            error << "failed subtest " << test << " with Integer=" <<
              typeid(Integer).name() << " and setValue=" << setValue <<
              " with error: " << p.getErrors().front();
          }
          BOOST_CHECK_MESSAGE(result == false, error.str());
        }

        BOOST_CHECK_EQUAL(int0, initialValue);
        BOOST_CHECK_EQUAL(boost::lexical_cast<string>(int1), setValue);
        BOOST_CHECK(!optInt2);
        BOOST_REQUIRE(optInt3);
        BOOST_CHECK_EQUAL(boost::lexical_cast<string>(*optInt3), setValue);
      } else {
        BOOST_CHECK_EQUAL(result, true);
        BOOST_CHECK_EQUAL(int0, initialValue);
        BOOST_CHECK_EQUAL(int1, initialValue);
        BOOST_CHECK(!optInt2);
        BOOST_CHECK(!optInt3);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE(int_option_test)
{
  checkIntegerOptions<uint16_t>();
  checkIntegerOptions<int16_t>();
  checkIntegerOptions<uint32_t>();
  checkIntegerOptions<int32_t>();
  checkIntegerOptions<uint64_t>();
  checkIntegerOptions<int64_t>();
}

