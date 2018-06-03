#pragma once
#include <vector>
#include <numeric>

namespace tl {

  template <typename T> struct TimeSubline
  {
    T time_step;
    ptrdiff_t nr_steps;
  };

  template <typename T> struct TimeLine
  {
    T start;
    std::vector<TimeSubline<T>> sublines;
  };

  template <typename T> class TimeLineIterator
  {
  public:
    using SublineIterator = typename std::vector<TimeSubline<T>>::const_iterator;

    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    TimeLineIterator(const TimeLine<T> &a_timeline, SublineIterator a_subline_iter, ptrdiff_t a_position)
      : timeline(a_timeline), subline_iter(a_subline_iter), position(a_position)
    {}

    TimeLineIterator(const TimeLine<T> &a_timeline, T time) : TimeLineIterator(a_timeline, begin(a_timeline.sublines), 0)
    {
      if (time < get_timeline().start) return;

      auto current_start_time = a_timeline.start;
      subline_iter = std::find_if(begin(a_timeline.sublines), end(a_timeline.sublines), [time, &current_start_time](const TimeSubline<T> &subline) {
        auto current_end_time = current_start_time + subline.time_step * subline.nr_steps;
        auto is_in_subline = (current_start_time <= time) && (time <= current_end_time);
        current_start_time = current_end_time;
        return is_in_subline;
      });

      if (subline_iter == end(a_timeline.sublines)) return;

      position = static_cast<ptrdiff_t>((time - current_start_time) / subline_iter->time_step);
    }

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

    inline TimeLineIterator<T>& operator++()
    {
      ++position;
      recalculateSublinePosition();
      return *this;
    }

    inline TimeLineIterator<T>& operator--()
    {
      --position;
      recalculateSublinePosition();
      return *this;
    }

    inline TimeLineIterator<T>& operator+=(ptrdiff_t value)
    {
      while (value != 0) {
        value = this->addToCurrentTimeLine(value);
      }
      return *this;
    }

    bool operator<(const TimeLineIterator<T>& rhs)
    {
      if (this->subline_iter < rhs.subline_iter) return true;
      if (this->subline_iter == rhs.subline_iter) return this->position < rhs.position;
      return false;
    }

  private:
    std::reference_wrapper<const TimeLine<T>> timeline;
    SublineIterator subline_iter;
    ptrdiff_t position;

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

    ptrdiff_t addToCurrentTimeLine(ptrdiff_t value)
    {
      auto next_pos = position + value;
      if (next_pos > subline_iter->nr_steps) {
        switchToNextSubline();
        return value - (subline_iter->nr_steps - position);
      }

      if (next_pos < 0) {
        switchToPreviousSubline();
        return value - position;
      }

      position = next_pos;
      return 0;
    }

    void recalculateSublinePosition()
    {
      if (position > subline_iter->nr_steps) switchToNextSubline();
      if (position < 0) switchToPreviousSubline();
    }
  };

  // Operators

  template <typename T> bool operator!=(const TimeLineIterator<T> &lhs, const TimeLineIterator<T> &rhs) {return !(lhs == rhs); }
  template <typename T> TimeLineIterator<T> operator+(TimeLineIterator<T> lhs, ptrdiff_t value)
  {
    lhs += value;
    return lhs;
  }
  template <typename T> TimeLineIterator<T> operator-(TimeLineIterator<T> lhs, const ptrdiff_t value)
  { 
    lhs += (-value);
    return lhs;
  }
  template <typename T> TimeLineIterator<T>& operator-=(TimeLineIterator<T>& lhs, ptrdiff_t value)
  {
    lhs += (-value);
    return lhs;
  }

  template <typename T> bool operator>(const TimeLineIterator<T>& lhs, const TimeLineIterator<T>& rhs) { return rhs < lhs; }
  template <typename T> bool operator<=(const TimeLineIterator<T>& lhs, const TimeLineIterator<T>& rhs) { return !(lhs > rhs); }
  template <typename T> bool operator>=(const TimeLineIterator<T>& lhs, const TimeLineIterator<T>& rhs) { return !(lhs < rhs); }

  template <typename T> TimeLineIterator<T> operator++(TimeLineIterator<T>& lhs, int)
  {
    auto res = lhs;
    ++(lhs)
    return res;
  }

  template <typename T> TimeLineIterator<T> operator--(TimeLineIterator<T>& lhs, int)
  {
    auto res = lhs;
    --(lhs)
    return res;
  }

  // iterator factories

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

  template <typename T> std::reverse_iterator<TimeLineIterator<T>> crend(const TimeLine<T> &timeline)
  {
    return std::reverse_iterator(cbegin(timeline));
  }

  template <typename T> TimeLineIterator<T> at(const TimeLine<T> &timeline, T time)
  {
    return { timeline, time };
  }

  template <typename T> std::reverse_iterator<TimeLineIterator<T>> rat(const TimeLine<T> &timeline, T time)
  {
    return std::reverse_iterator(TimeLineIterator<T>{ timeline, time });
  }
}

