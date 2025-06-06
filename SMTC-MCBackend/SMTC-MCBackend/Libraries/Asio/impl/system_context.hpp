//
// impl/system_context.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IMPL_SYSTEM_CONTEXT_HPP
#define ASIO_IMPL_SYSTEM_CONTEXT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/system_executor.hpp>

#include <Asio/detail/push_options.hpp>

namespace asio {

inline system_context::executor_type
system_context::get_executor() noexcept
{
  return system_executor();
}

} // namespace asio

#include <Asio/detail/pop_options.hpp>

#endif // ASIO_IMPL_SYSTEM_CONTEXT_HPP
