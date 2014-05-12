#pragma once

// General utilities are defined here


// Using the std::string
#include <string>
// Using the std::stringstream
#include <sstream>

#define GusString std::string

#define GusStringStream std::stringstream

#define GusVector std::vector

#ifdef GTHREADSYNC
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#endif //GTHREADSYNC

#ifndef NULL
#define NULL 0
#endif
