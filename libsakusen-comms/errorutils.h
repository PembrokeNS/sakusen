#ifndef ERRORUTILS_H
#define ERRORUTILS_H

#include "libsakusen-global.h"

/** \brief Turn a numeric errno into its string name (e.g. "EACCES") */
String errorUtils_parseErrno(int num);

/** \brief Get an appropriate error message for an errno */
String errorUtils_errorMessage(int num);

#endif // ERRORUTILS_H

