#ifndef LIBSAKUSEN_CLIENT__PARTIALHEIGHTFIELD_H
#define LIBSAKUSEN_CLIENT__PARTIALHEIGHTFIELD_H

#include <sakusen/heightfield.h>

namespace sakusen {
namespace client {

/** \brief A heightfield with less than total information about the heights
 *
 * \bug This class fails dramatically to fulfil its intended purpose at
 * present.
 */
class PartialHeightfield : public Heightfield {
  public:
    PartialHeightfield(
        uint32 horizontalResolution,
        uint32 verticalResolution,
        uint32 width,
        uint32 height
      ) :
      Heightfield(
          horizontalResolution, verticalResolution, width, height,
          hf_type(boost::extents[width][height])
        )
    {}
    PartialHeightfield(const Heightfield& copy) :
      Heightfield(copy)
    {}
};

}}

#endif // LIBSAKUSEN_CLIENT__PARTIALHEIGHTFIELD_H

