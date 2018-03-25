#pragma once
#include <vector>
#include <algorithm>
#include <limits>

namespace ozp {


  namespace detail_timetable {  

    template <typename T> struct EpsilonHelper { T get_epsilon(T) { return 0; } };
    template <> struct EpsilonHelper<float> { float  get_epsilon(float epsilon) { if (epsilon > 0.00001f)  return epsilon; else return 0.00001f; } };
    template <> struct EpsilonHelper<double> { double get_epsilon(double epsilon) { if (epsilon > 0.00000000000001) return epsilon; else return 0.00000000000001; } };
    
    template<class T> typename std::enable_if<std::is_integral<T>::value, bool >::type       in_between(T l, T h,T val, T)  { return (h >= val && val >= l); }
    template<class T> typename std::enable_if<std::is_floating_point<T>::value, bool >::type in_between(T l, T h, T val, T epsilon) { return (h + epsilon >= val && val >= l - epsilon); }

    template<class T> typename std::enable_if<std::is_integral<T>::value, bool >::type       in_between_wo_end(T l, T h, T val, T)  { return (h > val && val >= l); }
    template<class T> typename std::enable_if<std::is_floating_point<T>::value, bool >::type in_between_wo_end(T l, T h, T val, T epsilon) { return (h > val && val >= l - epsilon); }

    template<class T> typename std::enable_if<std::is_integral<T>::value, bool >::type       in_between_wo_start(T l, T h, T val, T)  { return (h >= val && val > l); }
    template<class T> typename std::enable_if<std::is_floating_point<T>::value, bool >::type in_between_wo_start(T l, T h, T val, T epsilon) { return (h + epsilon >= val && val > l); }
  }

  /// <summary>
  /// Represents Discretized time
  /// TimeTable<double> table(0.0, {{10, 0.1},{5, 0.2}});
  /// while(! table.finished()) {cout << table.time() << endl; table.next();}
  /// </summary>
  template <typename T> class TimeTable
  {
  public:
    static const int major_version = 0;
    static const int minor_verion  = 3;
    static const int patch_version = 1;
#if (_MSC_VER >= 1800)
    /// <summary>
    /// Initializes a new instance of the <see cref="TimeTable{T}"/> class.
    /// start time = 0, no time line is defined.
    /// </summary> 
    TimeTable() : TimeTable(0) {}
    /// <summary>
    /// Initializes a new instance of the <see cref="TimeTable{T}"/> class.
    /// </summary>
    /// <param name="start_time">The start_time.</param>
    /// <param name="il">Time lines in #step delta pairs. Ex: {{10, 0.1}, {5, 0.2}}</param>
    TimeTable(T start_time, std::initializer_list<std::pair<size_t, T>> il) : TimeTable(start_time) { this->insert(il); }
#else
    TimeTable() : _pos(0), _start_time(0), _current_time(_start_time), _current_tl_end_n(0), _current_tl_start_n(0), _n(0), _epsilon((T)0.0000001) {}
#endif
    /// <summary>
    /// Initializes a new instance of the <see cref="TimeTable{T}"/> class.
    /// </summary>
    /// <param name="start_time">The start_time.</param>
    explicit TimeTable(T start_time) : _pos(0), _start_time(start_time), _current_time(_start_time), _current_tl_end_n(0), _current_tl_start_n(0), _n(0), _epsilon((T)0.0000001)
    { 
      this->insert(0, 0); 
      auto ep_helper = detail_timetable::EpsilonHelper<T>();
      _epsilon = ep_helper.get_epsilon(0);
    }

    /// <summary>
    /// Returns the start time.
    /// </summary>
    /// <returns>Start time</returns>
    T start_time() const { return _start_time; }
    /// <summary>
    /// Computes the end time of the table
    /// </summary>
    /// <returns>End time</returns>
    T end_time()   const
    {
      T end = _start_time;
      for (auto&& pair : _timelines) end += pair.first * pair.second;
      return end;
    }
    /// <summary>
    /// Returns true if table is finished. 
    /// </summary>
    /// <returns>Is table finished</returns>
    bool finished() const { return (_pos >= _timelines.size()); }
    /// <summary>
    /// Returns true if table is started.
    /// </summary>
    /// <returns>Is table started</returns>
    bool started() const { return _current_time > _start_time; }
    /// <summary>
    /// Returns true if table is at start
    /// </summary>
    /// <returns>Is table at start</returns>
    bool at_start() const { return _current_time == _start_time; }

    /// <summary>
    /// Sets the start time of the table.
    /// </summary>
    /// <param name="time">The start time.</param>
    void set_start_time(T time) 
    {
      _current_time += time - _start_time;
      _start_time = time; 
    } 
    /// <summary>
    /// Inserts the time-line at the end of time table.
    /// </summary>
    /// <param name="n">Number of time steps of time line.</param>
    /// <param name="delta">The delta of time line.</param>
    void insert(size_t n, T delta) { _timelines.push_back(std::make_pair(n, delta)); }

#if (_MSC_VER >= 1800)
    /// <summary>
    /// Inserts time-lines to the end of table
    /// </summary>
    /// <param name="il">The initializer list of pairs for time lines. Ex: {{10, 0.1}, {5, 0.2}}</param>
    void insert(std::initializer_list<std::pair<size_t, T>> il) 
    { 
      _timelines.insert(_timelines.end(), il.begin(), il.end()); 
    }
#endif

    /// <summary>
    /// Resets the time-table.
    /// Current time = Start time
    /// </summary>
    void reset()
    {
      _current_time = _start_time;
      _pos = 0;
      _n = 0;
      _current_tl_end_n = 0;
      _current_tl_start_n = 0;
    }

    /// <summary>
    /// Returns current time. If finished() returns std::numeric_limits<T>::max()
    /// </summary>
    /// <returns>Current time. If finished() returns std::numeric_limits<T>::max()</returns>
    T time()  const { if (!this->finished()) return _current_time; else return std::numeric_limits<T>::max(); }
    /// <summary>
    /// Returns current delta. If finished() returns 0
    /// </summary>
    /// <returns>Delta. If finished returns 0</returns>
    T delta() const 
    { 
      if (this->finished()) return 0;
      else if (_pos == 0) {
        if (_timelines.size() > 1) return _timelines[1].second;
        else return 0;
      }
      else return _timelines[_pos].second;
    }

    /// <summary>
    /// Prefix increment for table. Sets current time to next step.
    /// </summary>
    /// <returns>*(this) after increment</returns>
    TimeTable<T>& operator++()
    {
      if (finished()) return *this;

      if (_pos == 0) {
        this->advance_timeline();
        if (!this->finished()) {
          _current_time += this->delta();
          ++_n;
        }
      }
      else {
        ++_n;
        if (_n > _current_tl_end_n) this->advance_timeline();
        _current_time += this->delta();
      }
      return *this;
    }

    /// <summary>
    /// Prefix decrement for table. Sets current time to previous step.
    /// </summary>
    /// <returns>*(this) after decrement</returns>>
    TimeTable<T>& operator--()
    {
      if (_current_time <= start_time()) return *this; // "<=" to get rid of precision errors
      if (_pos == 0) return *this;
      if (this->finished()) {
        this->retreat_timeline();
        _current_time = this->end_time();
        return *this;
      }
      --_n;
      if (_n < _current_tl_start_n) this->retreat_timeline();
      _current_time -= this->delta();
      return *this;
    }
    /// <summary>
    /// Postfix increment for table. Sets current time to next step.
    /// </summary>
    /// <param name="">Dummy</param>
    /// <returns>Copy of *(this) before increment</returns>
    TimeTable<T> operator++(int)
    {
      auto other = *this;
      ++(*this);
      return other;
    }
    /// <summary>
    /// Postfix decrement for table. Sets current time to previous step.
    /// </summary>
    /// <param name="">Dummy</param>
    /// <returns>Copy of *(this) before decrement</returns>>
    TimeTable<T> operator--(int)
    {
      auto other = *this;
      --(*this);
      return other;
    }

    /// <summary>
    /// Clears the time-table
    /// </summary>
    void clear()
    {
      _start_time = 0;
      _current_time = _start_time;
      _current_tl_end_n = 0;
      _current_tl_start_n = 0;
      if (_timelines.size() > 1) _timelines.erase(_timelines.begin() + 1, _timelines.end());
      _pos = 0;
      _n = 0;
    }

    bool between(T val, T start, T end) const
    {
      return detail_timetable::in_between(start, end, val, _epsilon);
    }

    bool between_start_end(T val) const
    {
      return detail_timetable::in_between(this->start_time(), this->end_time(), val, _epsilon);
    }

    bool between_start_now(T val) const
    {
      return detail_timetable::in_between(this->start_time(), this->time(), val, _epsilon);
    }

    bool between_now_end(T val) const
    {
      return detail_timetable::in_between(this->time(), this->end_time(), val, _epsilon);
    }

    bool in_past(T val) const
    {
      return detail_timetable::in_between_wo_end(this->start_time(), this->time(), val, _epsilon);
    }

    bool in_future(T val) const
    {
      return detail_timetable::in_between_wo_start(this->time(), this->end_time(), val, _epsilon);
    }

    const std::vector < std::pair<size_t, T> >& data() const { return _timelines; }

  private:
    typedef std::vector < std::pair<size_t, T> > TimeLineVectorType;

    T _start_time;
    T _current_time;
    size_t _current_tl_start_n;
    size_t _current_tl_end_n;
    TimeLineVectorType _timelines;
    size_t _pos;
    size_t _n;
    T _epsilon;

    void advance_timeline()
    {
      if (this->finished()) return;
      ++_pos;
      _current_tl_start_n = _current_tl_end_n;
      if (!this->finished()) _current_tl_end_n += _timelines[_pos].first;
    }
    void retreat_timeline()
    {
      if (_pos == 0) return;
      --_pos;
      _current_tl_end_n = _current_tl_start_n;
      _current_tl_start_n -= _timelines[_pos].first;
    }

  };

}