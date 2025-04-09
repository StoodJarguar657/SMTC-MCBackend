//
// impl/ssl/src.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_SSL_IMPL_SRC_HPP
#define ASIO_SSL_IMPL_SRC_HPP

#define ASIO_SOURCE

#include <Asio/detail/config.hpp>

#if defined(ASIO_HEADER_ONLY)
# error Do not compile Asio library source with ASIO_HEADER_ONLY defined
#endif

#include <Asio/ssl/impl/context.ipp>
#include <Asio/ssl/impl/error.ipp>
#include <Asio/ssl/detail/impl/engine.ipp>
#include <Asio/ssl/detail/impl/openssl_init.ipp>
#include <Asio/ssl/impl/host_name_verification.ipp>
#include <Asio/ssl/impl/rfc2818_verification.ipp>

#endif // ASIO_SSL_IMPL_SRC_HPP
