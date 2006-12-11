#ifndef TYPEMUNGER_H
#define TYPEMUNGER_H

namespace sakusen {

template<typename T>
class TypeMunger {
  public:
    typedef const T consted;
    typedef T unconsted;
};

template<typename T>
class TypeMunger<const T> {
  public:
    typedef const T consted;
    typedef T unconsted;
};

}

#endif // TYPEMUNGER_H

