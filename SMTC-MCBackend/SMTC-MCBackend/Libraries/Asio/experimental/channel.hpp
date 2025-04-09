//
// experimental/channel.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_EXPERIMENTAL_CHANNEL_HPP
#define ASIO_EXPERIMENTAL_CHANNEL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>
#include <Asio/any_io_executor.hpp>
#include <Asio/detail/type_traits.hpp>
#include <Asio/execution/executor.hpp>
#include <Asio/is_executor.hpp>
#include <Asio/experimental/basic_channel.hpp>
#include <Asio/experimental/channel_traits.hpp>

#include <Asio/detail/push_options.hpp>

namespace asio {
namespace experimental {
namespace detail {

template <typename ExecutorOrSignature, typename = void>
struct channel_type
{
  template <typename... Signatures>
  struct inner
  {
    typedef basic_channel<any_io_executor, channel_traits<>,
        ExecutorOrSignature, Signatures...> type;
  };
};

template <typename ExecutorOrSignature>
struct channel_type<ExecutorOrSignature,
    enable_if_t<
      is_executor<ExecutorOrSignature>::value
        || execution::is_executor<ExecutorOrSignature>::value
    >>
{
  template <typename... Signatures>
  struct inner
  {
    typedef basic_channel<ExecutorOrSignature,
        channel_traits<>, Signatures...> type;
  };
};

} // namespace detail

/// Template type alias for common use of channel.
template <typename ExecutorOrSignature, typename... Signatures>
using channel = typename detail::channel_type<
    ExecutorOrSignature>::template inner<Signatures...>::type;

} // namespace experimental
} // namespace asio

#include <Asio/detail/pop_options.hpp>

#endif // ASIO_EXPERIMENTAL_CHANNEL_HPP
