#pragma once

#ifdef GUSLIBTESTDLL_EXPORTS
#define GUSLIB_TEST_EXPORT_SYMBOL __declspec (dllexport)
#else
#define GUSLIB_TEST_EXPORT_SYMBOL __declspec (dllimport)
#endif
