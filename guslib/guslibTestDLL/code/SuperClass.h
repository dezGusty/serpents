#pragma once

#include "guslibtestbuildopts.h"

#include <string>
namespace gustest
{
  class GUSLIB_TEST_EXPORT_SYMBOL SuperClass
  {
  public:

    SuperClass (const std::string & name);

  protected:
    std::string name_;
  };
}