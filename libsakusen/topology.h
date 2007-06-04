#ifndef TOPOLOGY_H
#define TOPOLOGY_H

namespace sakusen {

/** \brief Enumeration of possible topologies for maps
 *
 * \note Current serialization implementation limits us to 256 topologies.
 * See [IO]Archive
 * \todo Possibly also the less symmetric versions of the mobius strip,
 * sphere and crosscut */
enum Topology {
  /** Normal planar topology with no edge identification */
  topology_plane,
  /** Cylinder with edges of constant x value identified.
   *
   * \todo Possibly also cylinderY */
  topology_cylinder,
  /** Torus with both opposite pairs of edges identified */
  topology_torus,
  /** M&ouml;bius strip with edges of constant x identified reversed
   *
   * \todo Possibly also mobiusY */
  topology_mobius,
  /** Klein bottle with edges of constant x identified normally, and
   * edges of constant y identified twisted
   *
   * \todo Possibly also the other Klein bottle */
  topology_klein,
  /** Sphere with adjacent pairs of edges identified (singularities at top left
   * and bottom right)
   *
   * \todo Possibly also the other sphere 
   *
   * \note Using a spherical topology on a non-square map will cause
   * everything to go completely haywire */
  topology_sphere,
  /** Real projective plane with opposite pairs of edges identified reversed */
  topology_crosscut
};

}

#endif // TOPOLOGY_H

