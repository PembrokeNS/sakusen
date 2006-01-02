#ifndef OARCHIVE_H
#define OARCHIVE_H

#include "libsakusen-global.h"

#include "stringutils.h"
#include "point.h"
#include "changeownerreason.h"
#include "updatetype.h"
#include "ordercondition.h"
#include "weapontype.h"

/* This class represents an archive into which we put objects for tranmission
 * over the network or saving to disk. */
class LIBSAKUSEN_API OArchive {
  public:
    OArchive();
    OArchive(size_t startCapacity);
    OArchive(const OArchive& copy);
    ~OArchive();
    OArchive& operator=(const OArchive& copy);
  private:
    uint8* buffer; /* owned by this */
    size_t length;
    size_t capacity;
    void expand();
    inline void ensureSpace(size_t space) {
      while(capacity-length < space) {
        expand();
      }
    }

    template<typename T>
    inline void store(const T&);

  public:
    inline size_t getLength() const { return length; }
    inline const uint8* getBytes() const { return buffer; }
      /* Danger: we're revealing a pointer to memory we own!  Possibly we
       * should copy it instead. */
    inline const size_t getBytesLength() const { return length; }
    inline String getSecureHashAsString() const {
      return stringUtils_getSecureHashAsString(buffer, length);
    }
    inline OArchive& operator<<(const bool& i) {
      ensureSpace(sizeof(uint8));
      buffer[length] = i;
      length += sizeof(uint8);
      return *this;
    }
    OArchive& operator<<(const uint8& i) {
      ensureSpace(sizeof(uint8));
      buffer[length]=i;
      length+=sizeof(uint8);
      return *this;
    }
    OArchive& operator<<(const sint8& i) {
      ensureSpace(sizeof(sint8));
      buffer[length] = i;
      length+=sizeof(sint8);
      return *this;
    }
    OArchive& operator<<(const uint16& i);
    OArchive& operator<<(const sint16& i);
    OArchive& operator<<(const uint32& i);
    OArchive& operator<<(const sint32& i);
    OArchive& operator<<(const double& d);
    OArchive& operator<<(const String& s);
	OArchive& operator<<(const changeOwnerReason& reason);
	OArchive& operator<<(const UpdateType& updtyp);
	OArchive& operator<<(const OrderCondition& condition);

    template<typename T>
	OArchive& operator<<(const std::vector<T>&);

    template<typename T>
    OArchive& operator<<(const std::list<T>&);
    
    template<typename T>
    OArchive& insert(const std::vector< std::vector<T> >& toStore);

    template<typename T>
    inline OArchive& operator<<(const Point<T>&);

	//These two lines are required by MSVC, whose templating does seem a little unusual.

	OArchive& operator<<(const Point<sint32>& point);
	OArchive& operator<<(const Point<uint32>& point);
	
	
	template<>
	inline void store<String>(const String& toStore)
	{
	  *this << toStore;
	}

	template<>
	inline void store<WeaponTypeID>(const WeaponTypeID& toStore)
	{
	  *this << toStore->getInternalName();
	}
};

#endif // OARCHIVE_H

