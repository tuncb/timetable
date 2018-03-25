#define BOOST_TEST_MODULE TimeTableTestModule
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <timetable/TimeTable.h>

BOOST_AUTO_TEST_SUITE(TimeTable)

BOOST_AUTO_TEST_CASE(SingleTimeLineTest)
{
#if (_MSC_VER >= 1800)
  ozp::TimeTable<double> table(1.0, { { 10, 0.1 } });
#else
  ozp::TimeTable<double> table(1.0);
  table.insert(10, 0.1);
#endif
  BOOST_CHECK_CLOSE(table.end_time(), 2.0, 0.01);

  int c = 0;
  while (!table.finished()) {
    BOOST_CHECK_CLOSE(table.time(), 1.0 + c*0.1, 0.01);
    BOOST_CHECK_CLOSE(table.delta(), 0.1, 0.01);
    ++table;
    ++c;
  }

  --table;
  --c;

  BOOST_CHECK_EQUAL(c, 10);
  for (int i = 0; i < 10; ++i) {
    BOOST_CHECK_CLOSE(table.time(), 1.0 +  c*0.1, 0.01);
    BOOST_CHECK_CLOSE(table.delta(), 0.1, 0.01);
    --table;
    --c;
  }
}

BOOST_AUTO_TEST_CASE(MultiTimeLinesTest)
{
#if (_MSC_VER >= 1800)
  ozp::TimeTable<double> table(1.0, { { 10, 0.1 }, { 4, 0.3 }, {5, 0.2 } });
#else
  ozp::TimeTable<double> table(1.0);
  table.insert(10, 0.1);
  table.insert(4, 0.3);
  table.insert(5, 0.2);
#endif

  BOOST_CHECK_CLOSE(table.end_time(), 4.2, 0.01);

  int c = 0;
  while (!table.finished()) {
    if (c <= 10) {
      BOOST_CHECK_CLOSE(table.time(), 1.0 + c*0.1, 0.01);
      BOOST_CHECK_CLOSE(table.delta(), 0.1, 0.01);
    }
    else if (c <= 14) {
      BOOST_CHECK_CLOSE(table.time(), 2.0 + (c-10)*0.3, 0.01);
      BOOST_CHECK_CLOSE(table.delta(), 0.3, 0.01);
    }
    else  {
      BOOST_CHECK_CLOSE(table.time(), 3.2 + (c - 14)*0.2, 0.01);
      BOOST_CHECK_CLOSE(table.delta(), 0.2, 0.01);
    }
    ++table;
    ++c;
  }

  --table;
  --c;

  BOOST_CHECK_EQUAL(c, 19);
  while (c <= 0) {
    if (c <= 10) {
      BOOST_CHECK_CLOSE(table.time(), 1.0 + c*0.1, 0.01);
      BOOST_CHECK_CLOSE(table.delta(), 0.1, 0.01);
    }
    else if (c <= 14) {
      BOOST_CHECK_CLOSE(table.time(), 2.0 + (c - 10)*0.3, 0.01);
      BOOST_CHECK_CLOSE(table.delta(), 0.3, 0.01);
    }
    else  {
      BOOST_CHECK_CLOSE(table.time(), 3.2 + (c - 14)*0.2, 0.01);
      BOOST_CHECK_CLOSE(table.delta(), 0.2, 0.01);
    }
    --table;
    --c;
  }
}

BOOST_AUTO_TEST_CASE(PostFixTest)
{
#if (_MSC_VER >= 1800)
  ozp::TimeTable<double> table(1.0, { { 10, 0.1 } });
#else
  ozp::TimeTable<double> table(1.0);
  table.insert(10, 0.1);
#endif
  BOOST_CHECK_CLOSE((table++).time(), 1.0, 0.01);
  BOOST_CHECK_CLOSE(table.time(), 1.1, 0.01);
  BOOST_CHECK_CLOSE((table--).time(), 1.1, 0.01);
  BOOST_CHECK_CLOSE(table.time(), 1.0, 0.01);
}

BOOST_AUTO_TEST_CASE(StartTimeChange)
{
#if (_MSC_VER >= 1800)
  ozp::TimeTable<double> table(1.0, { { 10, 0.1 } });
#else
  ozp::TimeTable<double> table(1.0);
  table.insert(10, 0.1);
#endif
  BOOST_CHECK_CLOSE(table.time(), 1.0, 0.01);
  BOOST_CHECK_EQUAL(table.at_start(), true);
  table.set_start_time(2.5);
  BOOST_CHECK_CLOSE(table.time(), 2.5, 0.01);
  BOOST_CHECK_EQUAL(table.at_start(), true);

  auto c = 0;
  while (!table.finished()) {
    BOOST_CHECK_CLOSE(table.time(), 2.5 + c*0.1, 0.01);
    BOOST_CHECK_CLOSE(table.delta(), 0.1, 0.01);
    ++table;
    ++c;
  }
  BOOST_CHECK_EQUAL(table.started(), true);
  BOOST_CHECK_EQUAL(table.finished(), true);

  table.reset();
  BOOST_CHECK_CLOSE(table.time(), 2.5, 0.01);
  BOOST_CHECK_EQUAL(table.at_start(), true);
  BOOST_CHECK_EQUAL(table.started(), false);
  BOOST_CHECK_EQUAL(table.finished(), false);

}

BOOST_AUTO_TEST_CASE(Clear)
{
#if (_MSC_VER >= 1800)
  ozp::TimeTable<double> table(1.0, { { 10, 0.1 } });
#else
  ozp::TimeTable<double> table(1.0);
  table.insert(10, 0.1);
#endif;
  table.clear();
  BOOST_CHECK_CLOSE(table.time(), 0, 0.01);
  table.set_start_time(3.0);
#if (_MSC_VER >= 1800)
  table.insert({ {10, 0.1} });
#else
  table.insert(10, 0.1);
#endif;

  int c = 0;
  while (!table.finished()) {
    BOOST_CHECK_CLOSE(table.time(), 3.0 + c*0.1, 0.01);
    BOOST_CHECK_CLOSE(table.delta(), 0.1, 0.01);
    ++table;
    ++c;
  }
}

BOOST_AUTO_TEST_CASE(InBetweenDouble)
{
#if (_MSC_VER >= 1800)
  ozp::TimeTable<double> table(1.0, { { 10, 0.1 } });
#else
  ozp::TimeTable<double> table(1.0);
  table.insert(10, 0.1);
#endif
  BOOST_CHECK_EQUAL(table.between_start_end(0.99999), false);
  BOOST_CHECK_EQUAL(table.between_start_end(1.0), true);
  BOOST_CHECK_EQUAL(table.between_start_end(2.0), true);
  BOOST_CHECK_EQUAL(table.between_start_end(2.000001), false);

  ++table; ++table;
  BOOST_CHECK_EQUAL(table.between_start_now(1.1999999), true);
  BOOST_CHECK_EQUAL(table.between_start_now(1.2), true);
  BOOST_CHECK_EQUAL(table.between_start_now(1.200001), false);
  BOOST_CHECK_EQUAL(table.between_start_now(2.0), false);

  BOOST_CHECK_EQUAL(table.between_now_end(1.1999999), false);
  BOOST_CHECK_EQUAL(table.between_now_end(1.1999999), false);
  BOOST_CHECK_EQUAL(table.between_now_end(1.2), true);
  BOOST_CHECK_EQUAL(table.between_now_end(2.0), true);
  BOOST_CHECK_EQUAL(table.between_now_end(2.0000001), false);
}

BOOST_AUTO_TEST_CASE(InBetweenFloat)
{
#if (_MSC_VER >= 1800)
  ozp::TimeTable<float> table(1.0f, { { 10, 0.1f } });
#else
  ozp::TimeTable<float> table(1.0f);
  table.insert(10, 0.1f);
#endif
  BOOST_CHECK_EQUAL(table.between_start_end(0.9999f), false);
  BOOST_CHECK_EQUAL(table.between_start_end(1.0f), true);
  BOOST_CHECK_EQUAL(table.between_start_end(2.0f), true);
  BOOST_CHECK_EQUAL(table.between_start_end(2.0001f), false);

  ++table; ++table;
  BOOST_CHECK_EQUAL(table.between_start_now(1.1999f), true);
  BOOST_CHECK_EQUAL(table.between_start_now(1.2f), true);
  BOOST_CHECK_EQUAL(table.between_start_now(1.2001f), false);
  BOOST_CHECK_EQUAL(table.between_start_now(2.0f), false);

  BOOST_CHECK_EQUAL(table.between_now_end(1.1999f), false);
  BOOST_CHECK_EQUAL(table.between_now_end(1.1999f), false);
  BOOST_CHECK_EQUAL(table.between_now_end(1.2f), true);
  BOOST_CHECK_EQUAL(table.between_now_end(2.0f), true);
  BOOST_CHECK_EQUAL(table.between_now_end(2.0001f), false);
}
BOOST_AUTO_TEST_CASE(InBetweenInt)
{
#if (_MSC_VER >= 1800)
  ozp::TimeTable<int> table(1, { { 10, 1 } });
#else
  ozp::TimeTable<int> table(1);
  table.insert(10, 1);
#endif
  BOOST_CHECK_EQUAL(table.between_start_end(0), false);
  BOOST_CHECK_EQUAL(table.between_start_end(1), true);
  BOOST_CHECK_EQUAL(table.between_start_end(11), true);
  BOOST_CHECK_EQUAL(table.between_start_end(12), false);

  ++table; ++table;
  BOOST_CHECK_EQUAL(table.between_start_now(2), true);
  BOOST_CHECK_EQUAL(table.between_start_now(3), true);
  BOOST_CHECK_EQUAL(table.between_start_now(4), false);
  BOOST_CHECK_EQUAL(table.between_start_now(12), false);

  BOOST_CHECK_EQUAL(table.between_now_end(2), false);
  BOOST_CHECK_EQUAL(table.between_now_end(3), true);
  BOOST_CHECK_EQUAL(table.between_now_end(11), true);
  BOOST_CHECK_EQUAL(table.between_now_end(12), false);
}

BOOST_AUTO_TEST_SUITE_END()

