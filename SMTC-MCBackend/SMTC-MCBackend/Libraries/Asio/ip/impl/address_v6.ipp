//
// ip/impl/address_v6.ipp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IP_IMPL_ADDRESS_V6_IPP
#define ASIO_IP_IMPL_ADDRESS_V6_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>
#include <cstring>
#include <stdexcept>
#include <typeinfo>
#include <Asio/detail/socket_ops.hpp>
#include <Asio/detail/throw_error.hpp>
#include <Asio/detail/throw_exception.hpp>
#include <Asio/error.hpp>
#include <Asio/ip/address_v6.hpp>
#include <Asio/ip/bad_address_cast.hpp>

#include <Asio/detail/push_options.hpp>

namespace asio {
namespace ip {

address_v6::address_v6() noexcept
  : addr_(),
    scope_id_(0)
{
}

address_v6::address_v6(const address_v6::bytes_type& bytes,
    scope_id_type scope)
  : scope_id_(scope)
{
#if UCHAR_MAX > 0xFF
  for (std::size_t i = 0; i < bytes.size(); ++i)
  {
    if (bytes[i] > 0xFF)
    {
      std::out_of_range ex("address_v6 from bytes_type");
      asio::detail::throw_exception(ex);
    }
  }
#endif // UCHAR_MAX > 0xFF

  using namespace std; // For memcpy.
  memcpy(addr_.s6_addr, bytes.data(), 16);
}

address_v6::address_v6(const address_v6& other) noexcept
  : addr_(other.addr_),
    scope_id_(other.scope_id_)
{
}

address_v6::address_v6(address_v6&& other) noexcept
  : addr_(other.addr_),
    scope_id_(other.scope_id_)
{
}

address_v6& address_v6::operator=(const address_v6& other) noexcept
{
  addr_ = other.addr_;
  scope_id_ = other.scope_id_;
  return *this;
}

address_v6& address_v6::operator=(address_v6&& other) noexcept
{
  addr_ = other.addr_;
  scope_id_ = other.scope_id_;
  return *this;
}

address_v6::bytes_type address_v6::to_bytes() const noexcept
{
  using namespace std; // For memcpy.
  bytes_type bytes;
  memcpy(bytes.data(), addr_.s6_addr, 16);
  return bytes;
}

std::string address_v6::to_string() const
{
  asio::error_code ec;
  char addr_str[asio::detail::max_addr_v6_str_len];
  const char* addr =
    asio::detail::socket_ops::inet_ntop(
        ASIO_OS_DEF(AF_INET6), &addr_, addr_str,
        asio::detail::max_addr_v6_str_len, scope_id_, ec);
  if (addr == 0)
    asio::detail::throw_error(ec);
  return addr;
}

#if !defined(ASIO_NO_DEPRECATED)
std::string address_v6::to_string(asio::error_code& ec) const
{
  char addr_str[asio::detail::max_addr_v6_str_len];
  const char* addr =
    asio::detail::socket_ops::inet_ntop(
        ASIO_OS_DEF(AF_INET6), &addr_, addr_str,
        asio::detail::max_addr_v6_str_len, scope_id_, ec);
  if (addr == 0)
    return std::string();
  return addr;
}

address_v4 address_v6::to_v4() const
{
  if (!is_v4_mapped() && !is_v4_compatible())
  {
    bad_address_cast ex;
    asio::detail::throw_exception(ex);
  }

  address_v4::bytes_type v4_bytes = { { addr_.s6_addr[12],
    addr_.s6_addr[13], addr_.s6_addr[14], addr_.s6_addr[15] } };
  return address_v4(v4_bytes);
}
#endif // !defined(ASIO_NO_DEPRECATED)

bool address_v6::is_loopback() const noexcept
{
  return ((addr_.s6_addr[0] == 0) && (addr_.s6_addr[1] == 0)
      && (addr_.s6_addr[2] == 0) && (addr_.s6_addr[3] == 0)
      && (addr_.s6_addr[4] == 0) && (addr_.s6_addr[5] == 0)
      && (addr_.s6_addr[6] == 0) && (addr_.s6_addr[7] == 0)
      && (addr_.s6_addr[8] == 0) && (addr_.s6_addr[9] == 0)
      && (addr_.s6_addr[10] == 0) && (addr_.s6_addr[11] == 0)
      && (addr_.s6_addr[12] == 0) && (addr_.s6_addr[13] == 0)
      && (addr_.s6_addr[14] == 0) && (addr_.s6_addr[15] == 1));
}

bool address_v6::is_unspecified() const noexcept
{
  return ((addr_.s6_addr[0] == 0) && (addr_.s6_addr[1] == 0)
      && (addr_.s6_addr[2] == 0) && (addr_.s6_addr[3] == 0)
      && (addr_.s6_addr[4] == 0) && (addr_.s6_addr[5] == 0)
      && (addr_.s6_addr[6] == 0) && (addr_.s6_addr[7] == 0)
      && (addr_.s6_addr[8] == 0) && (addr_.s6_addr[9] == 0)
      && (addr_.s6_addr[10] == 0) && (addr_.s6_addr[11] == 0)
      && (addr_.s6_addr[12] == 0) && (addr_.s6_addr[13] == 0)
      && (addr_.s6_addr[14] == 0) && (addr_.s6_addr[15] == 0));
}

bool address_v6::is_link_local() const noexcept
{
  return ((addr_.s6_addr[0] == 0xfe) && ((addr_.s6_addr[1] & 0xc0) == 0x80));
}

bool address_v6::is_site_local() const noexcept
{
  return ((addr_.s6_addr[0] == 0xfe) && ((addr_.s6_addr[1] & 0xc0) == 0xc0));
}

bool address_v6::is_v4_mapped() const noexcept
{
  return ((addr_.s6_addr[0] == 0) && (addr_.s6_addr[1] == 0)
      && (addr_.s6_addr[2] == 0) && (addr_.s6_addr[3] == 0)
      && (addr_.s6_addr[4] == 0) && (addr_.s6_addr[5] == 0)
      && (addr_.s6_addr[6] == 0) && (addr_.s6_addr[7] == 0)
      && (addr_.s6_addr[8] == 0) && (addr_.s6_addr[9] == 0)
      && (addr_.s6_addr[10] == 0xff) && (addr_.s6_addr[11] == 0xff));
}

#if !defined(ASIO_NO_DEPRECATED)
bool address_v6::is_v4_compatible() const
{
  return ((addr_.s6_addr[0] == 0) && (addr_.s6_addr[1] == 0)
      && (addr_.s6_addr[2] == 0) && (addr_.s6_addr[3] == 0)
      && (addr_.s6_addr[4] == 0) && (addr_.s6_addr[5] == 0)
      && (addr_.s6_addr[6] == 0) && (addr_.s6_addr[7] == 0)
      && (addr_.s6_addr[8] == 0) && (addr_.s6_addr[9] == 0)
      && (addr_.s6_addr[10] == 0) && (addr_.s6_addr[11] == 0)
      && !((addr_.s6_addr[12] == 0)
        && (addr_.s6_addr[13] == 0)
        && (addr_.s6_addr[14] == 0)
        && ((addr_.s6_addr[15] == 0) || (addr_.s6_addr[15] == 1))));
}
#endif // !defined(ASIO_NO_DEPRECATED)

bool address_v6::is_multicast() const noexcept
{
  return (addr_.s6_addr[0] == 0xff);
}

bool address_v6::is_multicast_global() const noexcept
{
  return ((addr_.s6_addr[0] == 0xff) && ((addr_.s6_addr[1] & 0x0f) == 0x0e));
}

bool address_v6::is_multicast_link_local() const noexcept
{
  return ((addr_.s6_addr[0] == 0xff) && ((addr_.s6_addr[1] & 0x0f) == 0x02));
}

bool address_v6::is_multicast_node_local() const noexcept
{
  return ((addr_.s6_addr[0] == 0xff) && ((addr_.s6_addr[1] & 0x0f) == 0x01));
}

bool address_v6::is_multicast_org_local() const noexcept
{
  return ((addr_.s6_addr[0] == 0xff) && ((addr_.s6_addr[1] & 0x0f) == 0x08));
}

bool address_v6::is_multicast_site_local() const noexcept
{
  return ((addr_.s6_addr[0] == 0xff) && ((addr_.s6_addr[1] & 0x0f) == 0x05));
}

bool operator==(const address_v6& a1, const address_v6& a2) noexcept
{
  using namespace std; // For memcmp.
  return memcmp(&a1.addr_, &a2.addr_,
      sizeof(asio::detail::in6_addr_type)) == 0
    && a1.scope_id_ == a2.scope_id_;
}

bool operator<(const address_v6& a1, const address_v6& a2) noexcept
{
  using namespace std; // For memcmp.
  int memcmp_result = memcmp(&a1.addr_, &a2.addr_,
      sizeof(asio::detail::in6_addr_type));
  if (memcmp_result < 0)
    return true;
  if (memcmp_result > 0)
    return false;
  return a1.scope_id_ < a2.scope_id_;
}

address_v6 address_v6::loopback() noexcept
{
  address_v6 tmp;
  tmp.addr_.s6_addr[15] = 1;
  return tmp;
}

#if !defined(ASIO_NO_DEPRECATED)
address_v6 address_v6::v4_mapped(const address_v4& addr)
{
  address_v4::bytes_type v4_bytes = addr.to_bytes();
  bytes_type v6_bytes = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF,
    v4_bytes[0], v4_bytes[1], v4_bytes[2], v4_bytes[3] } };
  return address_v6(v6_bytes);
}

address_v6 address_v6::v4_compatible(const address_v4& addr)
{
  address_v4::bytes_type v4_bytes = addr.to_bytes();
  bytes_type v6_bytes = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    v4_bytes[0], v4_bytes[1], v4_bytes[2], v4_bytes[3] } };
  return address_v6(v6_bytes);
}
#endif // !defined(ASIO_NO_DEPRECATED)

address_v6 make_address_v6(const char* str)
{
  asio::error_code ec;
  address_v6 addr = make_address_v6(str, ec);
  asio::detail::throw_error(ec);
  return addr;
}

address_v6 make_address_v6(const char* str,
    asio::error_code& ec) noexcept
{
  address_v6::bytes_type bytes;
  unsigned long scope_id = 0;
  if (asio::detail::socket_ops::inet_pton(
        ASIO_OS_DEF(AF_INET6), str, &bytes[0], &scope_id, ec) <= 0)
    return address_v6();
  return address_v6(bytes, scope_id);
}

address_v6 make_address_v6(const std::string& str)
{
  return make_address_v6(str.c_str());
}

address_v6 make_address_v6(const std::string& str,
    asio::error_code& ec) noexcept
{
  return make_address_v6(str.c_str(), ec);
}

#if defined(ASIO_HAS_STRING_VIEW)

address_v6 make_address_v6(string_view str)
{
  return make_address_v6(static_cast<std::string>(str));
}

address_v6 make_address_v6(string_view str,
    asio::error_code& ec) noexcept
{
  return make_address_v6(static_cast<std::string>(str), ec);
}

#endif // defined(ASIO_HAS_STRING_VIEW)

address_v4 make_address_v4(
    v4_mapped_t, const address_v6& v6_addr)
{
  if (!v6_addr.is_v4_mapped())
  {
    bad_address_cast ex;
    asio::detail::throw_exception(ex);
  }

  address_v6::bytes_type v6_bytes = v6_addr.to_bytes();
  address_v4::bytes_type v4_bytes = { { v6_bytes[12],
    v6_bytes[13], v6_bytes[14], v6_bytes[15] } };
  return address_v4(v4_bytes);
}

address_v6 make_address_v6(
    v4_mapped_t, const address_v4& v4_addr)
{
  address_v4::bytes_type v4_bytes = v4_addr.to_bytes();
  address_v6::bytes_type v6_bytes = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0xFF, 0xFF, v4_bytes[0], v4_bytes[1], v4_bytes[2], v4_bytes[3] } };
  return address_v6(v6_bytes);
}

} // namespace ip
} // namespace asio

#include <Asio/detail/pop_options.hpp>

#endif // ASIO_IP_IMPL_ADDRESS_V6_IPP
