//
// impl/thread_pool.ipp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IMPL_THREAD_POOL_IPP
#define ASIO_IMPL_THREAD_POOL_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>
#include <stdexcept>
#include <Asio/thread_pool.hpp>
#include <Asio/detail/throw_exception.hpp>

#include <Asio/detail/push_options.hpp>

namespace asio {

struct thread_pool::thread_function
{
  detail::scheduler* scheduler_;

  void operator()()
  {
#if !defined(ASIO_NO_EXCEPTIONS)
    try
    {
#endif// !defined(ASIO_NO_EXCEPTIONS)
      asio::error_code ec;
      scheduler_->run(ec);
#if !defined(ASIO_NO_EXCEPTIONS)
    }
    catch (...)
    {
      std::terminate();
    }
#endif// !defined(ASIO_NO_EXCEPTIONS)
  }
};

#if !defined(ASIO_NO_TS_EXECUTORS)
namespace detail {

inline long default_thread_pool_size()
{
  std::size_t num_threads = thread::hardware_concurrency() * 2;
  num_threads = num_threads == 0 ? 2 : num_threads;
  return static_cast<long>(num_threads);
}

} // namespace detail

thread_pool::thread_pool()
  : scheduler_(add_scheduler(new detail::scheduler(*this, 0, false))),
    num_threads_(detail::default_thread_pool_size())
{
  scheduler_.work_started();

  thread_function f = { &scheduler_ };
  threads_.create_threads(f, static_cast<std::size_t>(num_threads_));
}
#endif // !defined(ASIO_NO_TS_EXECUTORS)

namespace detail {

inline long clamp_thread_pool_size(std::size_t n)
{
  if (n > 0x7FFFFFFF)
  {
    std::out_of_range ex("thread pool size");
    asio::detail::throw_exception(ex);
  }
  return static_cast<long>(n & 0x7FFFFFFF);
}

} // namespace detail

thread_pool::thread_pool(std::size_t num_threads)
  : scheduler_(add_scheduler(new detail::scheduler(
          *this, num_threads == 1 ? 1 : 0, false))),
    num_threads_(detail::clamp_thread_pool_size(num_threads))
{
  scheduler_.work_started();

  thread_function f = { &scheduler_ };
  threads_.create_threads(f, static_cast<std::size_t>(num_threads_));
}

thread_pool::~thread_pool()
{
  stop();
  join();
  shutdown();
}

void thread_pool::stop()
{
  scheduler_.stop();
}

void thread_pool::attach()
{
  ++num_threads_;
  thread_function f = { &scheduler_ };
  f();
}

void thread_pool::join()
{
  if (num_threads_)
    scheduler_.work_finished();

  if (!threads_.empty())
    threads_.join();
}

detail::scheduler& thread_pool::add_scheduler(detail::scheduler* s)
{
  detail::scoped_ptr<detail::scheduler> scoped_impl(s);
  asio::add_service<detail::scheduler>(*this, scoped_impl.get());
  return *scoped_impl.release();
}

void thread_pool::wait()
{
  scheduler_.work_finished();
  threads_.join();
}

} // namespace asio

#include <Asio/detail/pop_options.hpp>

#endif // ASIO_IMPL_THREAD_POOL_IPP
