#include "update.h"

#include <ostream>

Update::Update() :
  data(NULL)
{
}

Update::Update(IArchive& in) :
  data(NULL)
{
  /* If the archive is empty then it's broken.  Leave data NULL and return */
  if (in.isFinished()) {
    return;
  }

  /* Get the type */
  uint8 typeInt;
  in >> typeInt;
  UpdateType type = static_cast<UpdateType>(typeInt);

  /* Construct the appropriate data */
  //switch (type) {
  //  default:
      Debug("Unknown UpdateType " << type);
  //    break;
  //}
}

Update::Update(const Update& copy) :
  data( copy.data ? copy.data->newCopy() : NULL )
{
}

Update::Update(const UpdateData& d) :
  data( d.newCopy() )
{
}

Update::~Update()
{
  if (data != NULL) {
    delete data;
    data = NULL;
  }
}

Update& Update::operator=(const Update& copy)
{
  data = ( copy.data ? copy.data->newCopy() : NULL );
  return *this;
}

std::ostream& operator<<(std::ostream& output, const Update& update)
{
  output << "Update { type=" << update.getType() << " }";
  return output;
}

