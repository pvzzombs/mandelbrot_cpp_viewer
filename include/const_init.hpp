/*
    const_init.hpp - Addtional utilities
    Part of mandelbrot viewer made in C++ using SFML library
    LICENSE: GNU Lesser General Public License v3.0
*/

#ifndef CONST_INIT_LOCK
#define CONST_INIT_LOCK

template < typename T_const_init >
class const_init {
  bool is_self_empty;
  T_const_init self;

  public:
  const_init(): is_self_empty(true) {
  }
  const_init(T_const_init some_data): is_self_empty(true) {
    if (is_self_empty && std::is_copy_assignable < T_const_init > ::value && std::is_copy_assignable < T_const_init > ::value) {
      is_self_empty = false;
      self = some_data;
    } else {
      std::cerr << "const_init Error: Invalid data. Proceeding is dangerous...\n";
    }
  }

  const_init(const const_init & some_data): is_self_empty(true) {
    if (is_self_empty) {
      is_self_empty = false;
      self = some_data.self;
    }
  }

  const_init & operator = (const const_init & some_data) {
    if (is_self_empty) {
      is_self_empty = false;
      self = some_data.self;
    }
    return *this;
  }

  T_const_init return_value() {
    return self;
  }

  T_const_init * return_value_pointer() {
    return &self;
  }
};

#endif