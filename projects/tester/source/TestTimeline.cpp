#define CATCH_CONFIG_MAIN
#include <algorithm>
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
  const auto DT_1 = 1.0;
  const auto DT_2 = 2.0;

  tl::TimeLine<double> timeline {START_TIME, { { DT_1, NR_STEPS_1 }, { DT_2, NR_STEPS_2 } }};

  auto calculate_time = [&timeline, START_TIME, NR_STEPS_1, DT_1, DT_2](int counter) {
    return
      START_TIME + 
      std::clamp(counter, 0, NR_STEPS_1) * DT_1 +
      std::clamp(counter - NR_STEPS_1, 0, counter) * DT_2;
  };

  const auto total_steps = NR_STEPS_1 + NR_STEPS_2 + 1;

  SECTION("cbegin and cend creation")
  {
    auto b = cbegin(timeline);
    REQUIRE(*b == START_TIME);
    REQUIRE(b != cend(timeline));
  }

  SECTION("forward loop")
  {
    auto counter = 0;
    for (auto iter = cbegin(timeline); iter != cend(timeline); ++iter)
    {
      REQUIRE(*iter == Approx(calculate_time(counter)));
      counter++;
    }
    REQUIRE(counter == total_steps);
  }

  SECTION("backwards loop")
  {
    auto counter = 0;
    for (auto iter = crbegin(timeline); iter != crend(timeline); ++iter)
    {
      REQUIRE(*iter == Approx(calculate_time(total_steps - counter - 1)));
      counter++;
    }
    REQUIRE(counter == total_steps);
  }

  SECTION("other stuff")
  {
    auto iter = at(timeline, 0.5);
    iter = iter + 3;
    iter = iter - 3;
    REQUIRE(iter == at(timeline, 0.5));
  }
}