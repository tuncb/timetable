#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "timeline/timeline.h"

TEST_CASE("can be constructed", "timeline")
{
  tl::TimeLine<double> t1;
  tl::TimeLine<double> t2 {0.0, { {0.5, 10}, {0.75, 5} }};
}


TEST_CASE("iterator tests", "iterator")
{
  const auto START_TIME = 10.0;
  tl::TimeLine<double> timeline {START_TIME, { { 1.0, 5 },{ 2.0, 5 } }};

  auto b = cbegin(timeline);
  REQUIRE(b != cend(timeline));
  REQUIRE(*b == START_TIME);

}