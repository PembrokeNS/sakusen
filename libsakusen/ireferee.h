#ifndef IREFEREE_H
#define IREFEREE_H

namespace sakusen {

class IRef;

class IReferee {
  protected:
    IReferee() {}
    IReferee(const IReferee&) {}
  public:
    virtual ~IReferee() {}
    virtual void supplyRef(const IRef&) {}
};

}

#endif // IREFEREE_H

