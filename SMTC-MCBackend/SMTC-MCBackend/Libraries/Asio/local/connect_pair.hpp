//
// local/connect_pair.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_LOCAL_CONNECT_PAIR_HPP
#define ASIO_LOCAL_CONNECT_PAIR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>

#if defined(ASIO_HAS_LOCAL_SOCKETS) \
  || defined(GENERATING_DOCUMENTATION)

#include <Asio/basic_socket.hpp>
#include <Asio/detail/socket_ops.hpp>
#include <Asio/detail/throw_error.hpp>
#include <Asio/error.hpp>
#include <Asio/local/basic_endpoint.hpp>

#include <Asio/detail/push_options.hpp>

namespace asio {
namespace local {

/// Create a pair of connected sockets.
template <typename Protocol, typename Executor1, typename Executor2>
void connect_pair(basic_socket<Protocol, Executor1>& socket1,
    basic_socket<Protocol, Executor2>& socket2);

/// Create a pair of connected sockets.
template <typename Protocol, typename Executor1, typename Executor2>
ASIO_SYNC_OP_VOID connect_pair(basic_socket<Protocol, Executor1>& socket1,
    basic_socket<Protocol, Executor2>& socket2, asio::error_code& ec);

template <typename Protocol, typename Executor1, typename Executor2>
inline void connect_pair(basic_socket<Protocol, Executor1>& socket1,
    basic_socket<Protocol, Executor2>& socket2)
{
  asio::error_code ec;
  connect_pair(socket1, socket2, ec);
  asio::detail::throw_error(ec, "connect_pair");
}

template <typename Protocol, typename Executor1, typename Executor2>
inline ASIO_SYNC_OP_VOID connect_pair(
    basic_socket<Protocol, Executor1>& socket1,
    basic_socket<Protocol, Executor2>& socket2, asio::error_code& ec)
{
  // Check that this function is only being used with a UNIX domain socket.
  asio::local::basic_endpoint<Protocol>* tmp
    = static_cast<typename Protocol::endpoint*>(0);
  (void)tmp;

  Protocol protocol;
  asio::detail::socket_type sv[2];
  if (asio::detail::socket_ops::socketpair(protocol.family(),
        protocol.type(), protocol.protocol(), sv, ec)
      == asio::detail::socket_error_retval)
    ASIO_SYNC_OP_VOID_RETURN(ec);

  socket1.assign(protocol, sv[0], ec);
  if (ec)
  {
    asio::error_code temp_ec;
    asio::detail::socket_ops::state_type state[2] = { 0, 0 };
    asio::detail::socket_ops::close(sv[0], state[0], true, temp_ec);
    asio::detail::socket_ops::close(sv[1], state[1], true, temp_ec);
    ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  socket2.assign(protocol, sv[1], ec);
  if (ec)
  {
    asio::error_code temp_ec;
    socket1.close(temp_ec);
    asio::detail::socket_ops::state_type state = 0;
    asio::detail::socket_ops::close(sv[1], state, true, temp_ec);
    ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  ASIO_SYNC_OP_VOID_RETURN(ec);
}

} // namespace local
} // namespace asio

#include <Asio/detail/pop_options.hpp>

#endif // defined(ASIO_HAS_LOCAL_SOCKETS)
       //   || defined(GENERATING_DOCUMENTATION)

#endif // ASIO_LOCAL_CONNECT_PAIR_HPP
