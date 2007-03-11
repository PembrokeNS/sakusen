#include "bounded.h"

namespace sakusen {

double Bounded::fast_intersect(const Ray& r) const {
  Box<sint32> bbox;
  double t_near_x, t_far_x, t_near_y, t_far_y, t_near_z, t_far_z, t_near;
  double inf = std::numeric_limits<double>::infinity();
  Point<double> d = Point<double>(r.d);

  bbox = getBoundingBox();
  /* First off, take each pair of parallel planes, and find the interval of t
   * for which the ray is between the planes.
   */
  if (r.d.x != 0) {
    t_near_x = double(bbox.getMin().x - r.origin.x) / d.x;
    t_far_x = double(bbox.getMax().x - r.origin.x) / d.x;
    /* unless the ray is parallel to those planes, in which case check whether
     * it is between them
     */
  } else if (r.origin.x <= bbox.getMax().x && r.origin.x >= bbox.getMin().x) {
    /* the ray is between the planes for all t */
    t_near_x = -inf; t_far_x = inf;
  } else
    return inf;
  /* ^ the ray misses the box entirely */

  if (r.d.y != 0) {
    t_near_y = double(bbox.getMin().y - r.origin.y) / d.y;
    t_far_y = double(bbox.getMax().y - r.origin.y) / d.y;
  } else if (r.origin.y <= bbox.getMax().y && r.origin.y >= bbox.getMax().y) {
    t_near_y = -inf; t_far_y = inf;
  } else
    return inf;

  if (r.d.z != 0) {
    t_near_z = double(bbox.getMin().z - r.origin.z) / d.z;
    t_far_z = double(bbox.getMax().z - r.origin.z) / d.z;
  } else if (r.origin.z <= bbox.getMax().z && r.origin.z >= bbox.getMax().z) {
    t_near_z = -inf; t_far_z = inf;
  } else
    return inf;

  /* now we know the interval for each, check that all three overlap */
  if (t_near_x > t_far_y || t_near_y > t_far_z || t_near_z > t_far_x
      || t_near_x > t_far_z || t_near_y > t_far_x || t_near_z > t_far_y)
    return inf;

  /* The entry intersection is the farthest one, i.e. the point where we enter
   * the intersection of all three ray segments.
   */
  t_near = std::max(t_near_x, std::max(t_near_y, t_near_z));

  /* If the entry point is behind the origin, we must be inside the box, so use
   * the exit point.
   */
  if (t_near > 0.0)
    return t_near;
  else
    return std::min(t_far_x, std::min(t_far_y, t_far_z));

}

bool Bounded::fast_intersection(const Bounded& b) const {
  return !(getBoundingBox().intersection(b.getBoundingBox()).isEmpty());
}

}
