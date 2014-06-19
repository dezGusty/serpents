#pragma once

// Include the main configuration file.
#include <config/serpents_config.h>

// Due to the large amounts of WARNINGS provided by OGRE, there is an option to suppress the warnings.
#if SERPENTS_SUPPRESS_OGRE_C4251 == 1
#pragma warning (push)
#pragma warning (disable:4251)
#include "Ogre.h"
#pragma warning (pop)
#else
#include "Ogre.h"
#endif
