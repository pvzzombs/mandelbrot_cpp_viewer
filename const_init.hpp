/*
    const_init.hpp - Addtional utilities
    Part of mandelbrot viewer made in C++ using SFML library
    Copyright (C) 2021 Miles MJ Jamon

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
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