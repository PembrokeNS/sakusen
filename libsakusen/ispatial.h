#ifndef LIBSAKUSEN_ISPATIAL_H
#define LIBSAKUSEN_ISPATIAL_H

namespace sakusen {

/** \brief An interface to a spatial index.
 *
 * This class provides a common interface to all types of spatial index. A
 * spatial index is a data structure that allows you to efficiently find
 * objects based on their position in space. A quadtree is an example of a
 * spatial index.
 *
 * In Sakusen, the spatial index will store references to only objects with
 * bboxes; that is, instances of Ref<Bounded>. Note that we use subtyping
 * rather than a parametric (templated) type because we will want to store
 * different types of objects in the same index, as long as they all have bboxen.
 */
class ISpatial {
  protected:
    ISpatial() {}
    ISpatial(const ISpatial& original) {}
  public:
    /** \brief Add a new entry to the index.
     *
     * If b is a valid Ref, adds it to the index; otherwise, does nothing.
     * \param[in] b Ref to the bounded object to add.
     */
    virtual void add(const Ref<Bounded> b) = 0;
    /** \brief Remove an about-to-be-invalidated entry from the index.
     *
     * If b is a valid Ref, finds its entry in the index and removes it;
     * otherwise, does nothing.
     *
     * \param[in] b Ref to the bounded object to add.
     */
    virtual void remove(const Ref<Bounded>) = 0;
    /** \brief Find objects whose bboxes are entirely inside the given bbox. */
    /** \brief Find objects whose bboxes are partially or completely inside the given bbox. */
    /** \brief Find objects whose bbox bounds intersect the given bbox bounds. */
    /** \brief Find objects whose bboxes partially or completely enclose the given bbox. */
    /** \brief Find objects whose bboxes completely enclose the given bbox. */
    /* find ray intersections with bboxes */
};

}
#endif /* LIBSAKUSEN_ISPATIAL_H */
