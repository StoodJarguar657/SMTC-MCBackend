//
// impl/io_context.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IMPL_IO_CONTEXT_IPP
#define ASIO_IMPL_IO_CONTEXT_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>
#include <Asio/io_context.hpp>
#include <Asio/detail/concurrency_hint.hpp>
#include <Asio/detail/limits.hpp>
#include <Asio/detail/scoped_ptr.hpp>
#include <Asio/detail/service_registry.hpp>
#include <Asio/detail/throw_error.hpp>

#if defined(ASIO_HAS_IOCP)
#include <Asio/detail/win_iocp_io_context.hpp>
#else
#include <Asio/detail/scheduler.hpp>
#endif

#include <Asio/detail/push_options.hpp>

namespace asio {

io_context::io_context()
  : impl_(add_impl(new impl_type(*this,
          ASIO_CONCURRENCY_HINT_DEFAULT, false)))
{
}

io_context::io_context(int concurrency_hint)
  : impl_(add_impl(new impl_type(*this, concurrency_hint == 1
          ? ASIO_CONCURRENCY_HINT_1 : concurrency_hint, false)))
{
}

io_context::impl_type& io_context::add_impl(io_context::impl_type* impl)
{
  asio::detail::scoped_ptr<impl_type> scoped_impl(impl);
  asio::add_service<impl_type>(*this, scoped_impl.get());
  return *scoped_impl.release();
}

io_context::~io_context()
{
  shutdown();
}

io_context::count_type io_context::run()
{
  asio::error_code ec;
  count_type s = impl_.run(ec);
  asio::detail::throw_error(ec);
  return s;
}

#if !defined(ASIO_NO_DEPRECATED)
io_context::count_type io_context::run(asio::error_code& ec)
{
  return impl_.run(ec);
}
#endif // !defined(ASIO_NO_DEPRECATED)

io_context::count_type io_context::run_one()
{
  asio::error_code ec;
  count_type s = impl_.run_one(ec);
  asio::detail::throw_error(ec);
  return s;
}

#if !defined(ASIO_NO_DEPRECATED)
io_context::count_type io_context::run_one(asio::error_code& ec)
{
  return impl_.run_one(ec);
}
#endif // !defined(ASIO_NO_DEPRECATED)

io_context::count_type io_context::poll()
{
  asio::error_code ec;
  count_type s = impl_.poll(ec);
  asio::detail::throw_error(ec);
  return s;
}

#if !defined(ASIO_NO_DEPRECATED)
io_context::count_type io_context::poll(asio::error_code& ec)
{
  return impl_.poll(ec);
}
#endif // !defined(ASIO_NO_DEPRECATED)

io_context::count_type io_context::poll_one()
{
  asio::error_code ec;
  count_type s = impl_.poll_one(ec);
  asio::detail::throw_error(ec);
  return s;
}

#if !defined(ASIO_NO_DEPRECATED)
io_context::count_type io_context::poll_one(asio::error_code& ec)
{
  return impl_.poll_one(ec);
}
#endif // !defined(ASIO_NO_DEPRECATED)

void io_context::stop()
{
  impl_.stop();
}

bool io_context::stopped() const
{
  return impl_.stopped();
}

void io_context::restart()
{
  impl_.restart();
}

io_context::service::service(asio::io_context& owner)
  : execution_context::service(owner)
{
}

io_context::service::~service()
{
}

void io_context::service::shutdown()
{
#if !defined(ASIO_NO_DEPRECATED)
  shutdown_service();
#endif // !defined(ASIO_NO_DEPRECATED)
}

#if !defined(ASIO_NO_DEPRECATED)
void io_context::service::shutdown_service()
{
}
#endif // !defined(ASIO_NO_DEPRECATED)

void io_context::service::notify_fork(io_context::fork_event ev)
{
#if !defined(ASIO_NO_DEPRECATED)
  fork_service(ev);
#else // !defined(ASIO_NO_DEPRECATED)
  (void)ev;
#endif // !defined(ASIO_NO_DEPRECATED)
}

#if !defined(ASIO_NO_DEPRECATED)
void io_context::service::fork_service(io_context::fork_event)
{
}
#endif // !defined(ASIO_NO_DEPRECATED)

} // namespace asio

#include <Asio/detail/pop_options.hpp>

#endif // ASIO_IMPL_IO_CONTEXT_IPP
