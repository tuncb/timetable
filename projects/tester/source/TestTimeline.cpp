#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "timeline/timeline.h"

TEST_CASE("can be constructed", "timeline")
{
  tl::TimeLine<double> t1;
  tl::TimeLine<double> {0.0, { {0.5, 10}, {0.75, 5} }};
}