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
  const auto NR_STEPS_1 = 5;
  const auto NR_STEPS_2 = 5;


  tl::TimeLine<double> timeline {START_TIME, { { 1.0, NR_STEPS_1 }, { 2.0, NR_STEPS_2 } }};

  auto b = cbegin(timeline);
  REQUIRE(b != cend(timeline));
  REQUIRE(*b == START_TIME);

  auto counter = 0;
  for (auto iter = cbegin(timeline); iter != cend(timeline); ++iter) 
  { 
    counter++; 
  }
  REQUIRE(counter == NR_STEPS_1 + NR_STEPS_2 + 1);


  counter = 0;
  for (auto iter = crbegin(timeline); iter != crend(timeline); ++iter)
  {
    counter++;
  }
  REQUIRE(counter == NR_STEPS_1 + NR_STEPS_2 + 1);
}