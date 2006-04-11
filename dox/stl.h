/** \file
 *
 * This file contains dummy definitions of various classes from the
 * standard libraries to help Doxygen figure out what's what, particularly in
 * matching up function declarations with definitions. */

namespace std {

class string
{
};

class istream
{
};

class ostream
{
};

class ifstream
{
};

class ofstream
{
};

template<typename T>
class list {
  public:
    T item;
};

}

namespace __gnu_cxx {

template<typename T>
class hash;

template<typename T, typename THash = hash<T> >
class hash_set {
  public:
    T item;
};

}

