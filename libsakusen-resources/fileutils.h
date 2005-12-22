#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <sys/types.h>

#include "libsakusen-global.h"

int fileUtils_mkdirRecursive(String path, mode_t mode);

#endif // FILEUTILS_H

