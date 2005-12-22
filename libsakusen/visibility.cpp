#include "visibility.h"

Visibility::Visibility() :
  optical(0),
  infraRed(0),
  radar(0),
  passiveRadar(0),
  sonar(0),
  passiveSonar(0),
  seismar(0)
{
}

Visibility::Visibility(IArchive& archive)
{
  archive >> optical >> infraRed >> radar >> passiveRadar >> sonar >>
    passiveSonar >> seismar;
}

