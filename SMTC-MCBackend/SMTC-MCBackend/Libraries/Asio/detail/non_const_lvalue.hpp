//
// detail/non_const_lvalue.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_DETAIL_NON_CONST_LVALUE_HPP
#define ASIO_DETAIL_NON_CONST_LVALUE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>
#include <Asio/detail/type_traits.hpp>

#include <Asio/detail/push_options.hpp>

namespace asio {
namespace detail {

template <typename T>
struct non_const_lvalue
{
  explicit non_const_lvalue(T& t)
    : value(static_cast<conditional_t<
        is_same<T, decay_t<T>>::value, decay_t<T>&, T&&>>(t))
  {
  }

  conditional_t<is_same<T, decay_t<T>>::value, decay_t<T>&, decay_t<T>> value;
};

} // namespace detail
} // namespace asio

#include <Asio/detail/pop_options.hpp>

#endif // ASIO_DETAIL_NON_CONST_LVALUE_HPP
