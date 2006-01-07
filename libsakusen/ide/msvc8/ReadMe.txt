========================================================================
       DYNAMIC LINK LIBRARY : libsakusen
========================================================================

MSVC specific readme:

gnu_extensions.h contains implementations of hash_map to get around the fact that MSVC6 does not have one. 
Reportedly, MSVC7 does. 

libsakusen-global.h contains workarounds for various things MSVC doesn't have, and defines the export macros.

If you want a function to be visible to clients of the dll, you need to use the various EXPORT macros.

There is a small workaround in vectors.h

