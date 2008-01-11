#ifndef TEDOMARI_GLOBAL_H
#define TEDOMARI_GLOBAL_H

#include "libsakusen-global.h"

#define APPLICATION_NAME "tedomari"
#define APPLICATION_VERSION "0.1"

#ifdef _MSC_VER
 #pragma warning(disable:4251 4275)
 #define REVISION "0.1"
#endif

namespace tedomari {

using sakusen::String;

}

#endif // TEDOMARI_GLOBAL_H

