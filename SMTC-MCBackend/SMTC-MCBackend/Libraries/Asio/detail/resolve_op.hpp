//
// detail/resolve_op.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_RESOLVE_OP_HPP
#define ASIO_DETAIL_RESOLVE_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>
#include <Asio/error.hpp>
#include <Asio/detail/operation.hpp>

#include <Asio/detail/push_options.hpp>

namespace asio {
namespace detail {

class resolve_op : public operation
{
public:
  // The error code to be passed to the completion handler.
  asio::error_code ec_;

protected:
  resolve_op(func_type complete_func)
    : operation(complete_func)
  {
  }
};

} // namespace detail
} // namespace asio

#include <Asio/detail/pop_options.hpp>

#endif // ASIO_DETAIL_RESOLVE_OP_HPP
