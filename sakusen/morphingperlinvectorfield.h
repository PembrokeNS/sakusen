#ifndef LIBSAKUSEN__MORPHINGPERLINVECTORFIELD_H
#define LIBSAKUSEN__MORPHINGPERLINVECTORFIELD_H

#include <sakusen/morphingperlinnoisefield.h>

namespace sakusen {

template<typename UniformRandomNumberGenerator>
class MorphingPerlinVectorField {
  public:
    MorphingPerlinVectorField(
        UniformRandomNumberGenerator& rng,
        uint32 const logMinSpatialScale,
        uint32 const logMaxSpatialScale,
        uint32 const logMinTemporalScale,
        Box<sint32> const& bounds
      ) :
      xField(
          rng, logMinSpatialScale, logMaxSpatialScale, logMinTemporalScale,
          bounds.getSize().x, bounds.getSize().y, bounds.getSize().z
        ),
      yField(
          rng, logMinSpatialScale, logMaxSpatialScale, logMinTemporalScale,
          bounds.getSize().x, bounds.getSize().y, bounds.getSize().z
        ),
      zField(
          rng, logMinSpatialScale, logMaxSpatialScale, logMinTemporalScale,
          bounds.getSize().x, bounds.getSize().y, bounds.getSize().z
        ),
      lowerBound(bounds.getMin())
    {}

    Point<sint32> operator()(Time t, Position const& p) {
      Position d = p - lowerBound;
      /** \bug Noise is not continuous across map boundaries on non-planar
       * topologies. */
      return Point<sint32>(
          xField(t, d.x, d.y, d.z),
          yField(t, d.x, d.y, d.z),
          zField(t, d.x, d.y, d.z)
        );
    }
  private:
    typedef MorphingPerlinNoiseField<3, UniformRandomNumberGenerator> Subfield;
    Subfield xField;
    Subfield yField;
    Subfield zField;
    Position lowerBound;
};

}

#endif // LIBSAKUSEN__MORPHINGPERLINVECTORFIELD_H

