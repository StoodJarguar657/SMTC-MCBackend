//
// detail/null_static_mutex.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_NULL_STATIC_MUTEX_HPP
#define ASIO_DETAIL_NULL_STATIC_MUTEX_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>

#if !defined(ASIO_HAS_THREADS)

#include <Asio/detail/scoped_lock.hpp>

#include <Asio/detail/push_options.hpp>

namespace asio {
namespace detail {

struct null_static_mutex
{
  typedef asio::detail::scoped_lock<null_static_mutex> scoped_lock;

  // Initialise the mutex.
  void init()
  {
  }

  // Lock the mutex.
  void lock()
  {
  }

  // Unlock the mutex.
  void unlock()
  {
  }

  int unused_;
};

#define ASIO_NULL_STATIC_MUTEX_INIT { 0 }

} // namespace detail
} // namespace asio

#include <Asio/detail/pop_options.hpp>

#endif // !defined(ASIO_HAS_THREADS)

#endif // ASIO_DETAIL_NULL_STATIC_MUTEX_HPP
