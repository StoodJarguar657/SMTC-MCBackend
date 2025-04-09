//
// detail/reactor.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_REACTOR_HPP
#define ASIO_DETAIL_REACTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>

#if defined(ASIO_HAS_IOCP) || defined(ASIO_WINDOWS_RUNTIME)
#include <Asio/detail/null_reactor.hpp>
#elif defined(ASIO_HAS_IO_URING_AS_DEFAULT)
#include <Asio/detail/null_reactor.hpp>
#elif defined(ASIO_HAS_EPOLL)
#include <Asio/detail/epoll_reactor.hpp>
#elif defined(ASIO_HAS_KQUEUE)
#include <Asio/detail/kqueue_reactor.hpp>
#elif defined(ASIO_HAS_DEV_POLL)
#include <Asio/detail/dev_poll_reactor.hpp>
#else
#include <Asio/detail/select_reactor.hpp>
#endif

namespace asio {
namespace detail {

#if defined(ASIO_HAS_IOCP) || defined(ASIO_WINDOWS_RUNTIME)
typedef null_reactor reactor;
#elif defined(ASIO_HAS_IO_URING_AS_DEFAULT)
typedef null_reactor reactor;
#elif defined(ASIO_HAS_EPOLL)
typedef epoll_reactor reactor;
#elif defined(ASIO_HAS_KQUEUE)
typedef kqueue_reactor reactor;
#elif defined(ASIO_HAS_DEV_POLL)
typedef dev_poll_reactor reactor;
#else
typedef select_reactor reactor;
#endif

} // namespace detail
} // namespace asio

#endif // ASIO_DETAIL_REACTOR_HPP
