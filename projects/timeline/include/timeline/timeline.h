#pragma once
#include <vector>
#include <algorithm>

namespace timetable {

  using index_type  = ptrdiff_t;

  template <typename T> struct TimeSubLine
  {
    T time_step;
    index_type nr_steps;
  };

  template <typename T> struct TimeLine
  {
    T start;
    std::vector<TimeSubLine<T>> sublines;
  };

  template <typename T> class TimeLine<T>::Iterator : public std::iterator<std::random_access_iterator_tag, T>
  {
  public:
    using SublineIterator = std::vector<TimeSubLine<T>>::iterator;

    Iterator(const TimeLine<T> &a_timetable, SublineIterator a_timeline_iter, index_type a_position)
      : timetable(a_timetable), timeline_iter(a_timeline_iter), position(a_position)
    {}

    Iterator(const TimeLine<T> &a_timetable, T time) : Iterator(a_timetable, begin(a_timetable.sublines), 0)
    {
      auto current_start_time = a_timetable.start;
      timeline_iter = std::find_if(begin(a_timetable.sublines), end(a_timetable.sublines), [time](const TimeSubLine<T> &subline) {
        auto current_end_time = current_start_time + subline.time_step * subline.nr_steps;
        auto is_in_subline = (current_start_time <= time) && (time <= current_end_time);
        current_start_time = current_end_time;
        return is_in_subline;
      });

      if (timeline_iter == end(a_timetable.sublines)) return;
      position = (time - current_start_time) / *timeline_iter.time_step;
    }

    T& operator*() const 
    {
      return computeSublineStartTime() + position * (*timeline_iter).time_step;
    }

    Iterator& operator++() 
    {
      ++position; 
      recalculateSublinePosition();
      return *this; 
    }

    Iterator& operator--() 
    { 
      --position; 
      recalculateSublinePosition();
      return *this; 
    }

  private:
    std::reference_wrapper<const TimeLine<T>> timeline;
    SublineIterator subline_iter;
    index_type position;

    const TimeLine<T>& timeline() {return time}

    T computeSublineStartTime(SubLineIterator iter) const
    {
      return timetable.start_time + std::accumulate(begin(timetable.get().sublines), end(timetable.get().sublines), T(0), [](const T value, const TimeSubLine<T> &subline) {
        return value + subline.nr_steps * subline.time_step;
      });
    }

    void recalculateSublinePosition()
    {
      if (position > *subline_iter.nr_steps) ++subline_iter;
      if (position < 0) --subline_iter;
    }
  };


}

