#pragma once
#include <vector>
#include <numeric>

namespace tl {

  using index_type  = ptrdiff_t;

  template <typename T> struct TimeSubline
  {
    T time_step;
    index_type nr_steps;
  };

  template <typename T> struct TimeLine
  {
    T start;
    std::vector<TimeSubline<T>> sublines;
  };

  template <typename T> class TimeLineIterator : public std::iterator<std::random_access_iterator_tag, T>
  {
  public:
    using SublineIterator = typename std::vector<TimeSubline<T>>::const_iterator;

    TimeLineIterator(const TimeLine<T> &a_timeline, SublineIterator a_subline_iter, index_type a_position)
      : timeline(a_timeline), subline_iter(a_subline_iter), position(a_position)
    {}

    inline bool operator==(const TimeLineIterator<T> &other) const
    {
      return
        (&timeline.get() == &other.timeline.get()) &&
        (subline_iter == other.subline_iter) &&
        (position == other.position);
    }

    inline T operator*() const
    {
      return computeSublineStartTime() + subline_iter->time_step * position;
    }

    TimeLineIterator<T>& operator++()
    {
      ++position;
      recalculateSublinePosition();
      return *this;
    }

    TimeLineIterator<T>& operator--()
    {
      --position;
      recalculateSublinePosition();
      return *this;
    }

  private:
    std::reference_wrapper<const TimeLine<T>> timeline;
    SublineIterator subline_iter;
    index_type position;

    const TimeLine<T>& get_timeline() const { return timeline.get(); }

    T computeSublineStartTime() const
    {
      return get_timeline().start + std::accumulate(cbegin(get_timeline().sublines), subline_iter, T(0), [](const T value, const TimeSubline<T> &subline) {
        return value + subline.nr_steps * subline.time_step;
      });
    }

    void switchToNextSubline()
    {
      ++subline_iter;
      position = 1;
    }

    void switchToPreviousSubline()
    {
      --subline_iter;
      position = subline_iter->nr_steps;
    }

    void recalculateSublinePosition()
    {
      if (position > subline_iter->nr_steps) switchToNextSubline();
      if (position < 0) switchToPreviousSubline();
    }
  };

  template <typename T> bool operator !=(const TimeLineIterator<T> &lhs, const TimeLineIterator<T> &rhs) {return !(lhs == rhs); }


  template <typename T> TimeLineIterator<T> cbegin(const TimeLine<T> &timeline)
  {
    return { timeline, cbegin(timeline.sublines), 0 };
  }

  template <typename T> TimeLineIterator<T> cend(const TimeLine<T> &timeline)
  {
    return { timeline, cend(timeline.sublines), 1 };
  }

  template <typename T> std::reverse_iterator<TimeLineIterator<T>> crbegin(const TimeLine<T> &timeline)
  {
    auto iter = (rbegin(timeline.sublines) + 1).base();
    if (timeline.sublines.empty()) return std::reverse_iterator(TimeLineIterator<T>{timeline, iter, 0});
    return std::reverse_iterator(TimeLineIterator<T>{timeline, iter, iter->nr_steps});
  }

  template <typename T> std::reverse_iterator<TimeLineIterator<T>>  crend(const TimeLine<T> &timeline)
  {
    return std::reverse_iterator(cbegin(timeline));
  }



/*
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

  */

}

