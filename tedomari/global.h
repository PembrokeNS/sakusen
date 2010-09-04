#ifndef TEDOMARI_GLOBAL_H
#define TEDOMARI_GLOBAL_H

#include <sakusen/resources/global.h>
#include <sakusen/gnu_extensions.h>

#define TEDOMARI_APPLICATION_NAME "tedomari"
#define TEDOMARI_APPLICATION_VERSION "0.1"

#ifdef _MSC_VER
 #pragma warning(disable:4251 4275)
 #define REVISION "0.1"
#endif

namespace tedomari {

using sakusen::String;
using sakusen::u_map;

}

#endif // TEDOMARI_GLOBAL_H

