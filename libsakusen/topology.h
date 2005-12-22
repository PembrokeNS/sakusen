#ifndef TOPOLOGY_H
#define TOPOLOGY_H

/* Note that current serialization implementation limits us to 256 topologies.
 * See map.cpp */
enum Topology {
  topology_plane,
  topology_cylinder, /* possibly also cylinderY */
  topology_torus,
  topology_mobius, /* possibly also mobiusY */
  topology_klein, /* this one has vertical edges joined normally, and
                     horizontal edges joined twisted - possibly also the other
                     Klein bottle */
  topology_sphere, /* this one has singularities at top left and bottom right
                      - possibly also the other sphere */
    /* note that using a spherical topology on a non-square map will cause
     * everything to go completely haywire */
  topology_crosscut /* aka real projective plane */
  /* possibly also the less symmetric versions of the mobius strip, sphere and
   * crosscut */
};

#endif // TOPOLOGY_H

