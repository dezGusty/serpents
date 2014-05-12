#include "SuperClass.h"

#include <guslib/trace/trace.h>

namespace gustest
{
  SuperClass::SuperClass (const std::string & name)
    : name_ (name)
  {
    GTRACE (4, "Superclass created for " << name);
  }
}