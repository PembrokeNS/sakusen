#ifndef FUSEKI__NULL_CAST_H
#define FUSEKI__NULL_CAST_H

namespace fuseki {
template<typename T, typename U>
struct NullCaster {
  T& operator()(U&) const {
    Fatal("invalid null_cast");
#ifdef _MSC_VER
    //MSVC says this must return a value.
    T *t = new T;
    return *t;
#endif
  }
};

template<typename T>
struct NullCaster<T, T> {
  T& operator()(T& t) const {
    return t;
  }
};

/** \brief Cast implying a runtime assertion that the types T and U are equal.
 *
 * When T and U are the same type, null_cast returns a reference to it's
 * argument (and thus should amount to doing nothing at all).
 *
 * When they are not the same, the null_cast will abort at runtime.
 *
 * Constness is automatically added to T if it is present in U.
 *
 * An example of where this is useful is a generic method where you have a
 * variable i of type T and the runtime logic tells you that T is int, but this
 * method is also instantiated where T is some other type.  Then to use i as
 * an int, you can use null_cast<int>(i) to circumvent the compiler's
 * ignorance.
 */
template<typename T, typename U>
typename boost::mpl::if_<
    typename boost::is_const<U>::type, typename boost::add_const<T>::type, T
  >::type& null_cast(U& u) {
  return NullCaster<typename boost::mpl::if_<
      typename boost::is_const<U>::type, typename boost::add_const<T>::type, T
    >::type,U>()(u);
}

}

#endif // FUSEKI__NULL_CAST_H

