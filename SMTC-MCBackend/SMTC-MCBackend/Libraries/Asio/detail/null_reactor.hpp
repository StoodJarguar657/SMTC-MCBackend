//
// detail/null_reactor.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_NULL_REACTOR_HPP
#define ASIO_DETAIL_NULL_REACTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>

#if defined(ASIO_HAS_IOCP) \
  || defined(ASIO_WINDOWS_RUNTIME) \
  || defined(ASIO_HAS_IO_URING_AS_DEFAULT)

#include <Asio/detail/scheduler_operation.hpp>
#include <Asio/detail/scheduler_task.hpp>
#include <Asio/execution_context.hpp>

#include <Asio/detail/push_options.hpp>

namespace asio {
namespace detail {

class null_reactor
  : public execution_context_service_base<null_reactor>,
    public scheduler_task
{
public:
  struct per_descriptor_data
  {
  };

  // Constructor.
  null_reactor(asio::execution_context& ctx)
    : execution_context_service_base<null_reactor>(ctx)
  {
  }

  // Destructor.
  ~null_reactor()
  {
  }

  // Initialise the task.
  void init_task()
  {
  }

  // Destroy all user-defined handler objects owned by the service.
  void shutdown()
  {
  }

  // No-op because should never be called.
  void run(long /*usec*/, op_queue<scheduler_operation>& /*ops*/)
  {
  }

  // No-op.
  void interrupt()
  {
  }
};

} // namespace detail
} // namespace asio

#include <Asio/detail/pop_options.hpp>

#endif // defined(ASIO_HAS_IOCP)
       //   || defined(ASIO_WINDOWS_RUNTIME)
       //   || defined(ASIO_HAS_IO_URING_AS_DEFAULT)

#endif // ASIO_DETAIL_NULL_REACTOR_HPP
