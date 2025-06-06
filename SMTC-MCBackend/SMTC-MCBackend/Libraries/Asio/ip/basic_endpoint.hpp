//
// ip/basic_endpoint.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IP_BASIC_ENDPOINT_HPP
#define ASIO_IP_BASIC_ENDPOINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/detail/config.hpp>
#include <functional>
#include <Asio/detail/cstdint.hpp>
#include <Asio/ip/address.hpp>
#include <Asio/ip/detail/endpoint.hpp>

#if !defined(ASIO_NO_IOSTREAM)
#include <iosfwd>
#endif // !defined(ASIO_NO_IOSTREAM)

#include <Asio/detail/push_options.hpp>

namespace asio {
namespace ip {

/// Type used for storing port numbers.
typedef uint_least16_t port_type;

/// Describes an endpoint for a version-independent IP socket.
/**
 * The asio::ip::basic_endpoint class template describes an endpoint that
 * may be associated with a particular socket.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 *
 * @par Concepts:
 * Endpoint.
 */
template <typename InternetProtocol>
class basic_endpoint
{
public:
  /// The protocol type associated with the endpoint.
  typedef InternetProtocol protocol_type;

  /// The type of the endpoint structure. This type is dependent on the
  /// underlying implementation of the socket layer.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined data_type;
#else
  typedef asio::detail::socket_addr_type data_type;
#endif

  /// Default constructor.
  basic_endpoint() noexcept
    : impl_()
  {
  }

  /// Construct an endpoint using a port number, specified in the host's byte
  /// order. The IP address will be the any address (i.e. INADDR_ANY or
  /// in6addr_any). This constructor would typically be used for accepting new
  /// connections.
  /**
   * @par Examples
   * To initialise an IPv4 TCP endpoint for port 1234, use:
   * @code
   * asio::ip::tcp::endpoint ep(asio::ip::tcp::v4(), 1234);
   * @endcode
   *
   * To specify an IPv6 UDP endpoint for port 9876, use:
   * @code
   * asio::ip::udp::endpoint ep(asio::ip::udp::v6(), 9876);
   * @endcode
   */
  basic_endpoint(const InternetProtocol& internet_protocol,
      port_type port_num) noexcept
    : impl_(internet_protocol.family(), port_num)
  {
  }

  /// Construct an endpoint using a port number and an IP address. This
  /// constructor may be used for accepting connections on a specific interface
  /// or for making a connection to a remote endpoint.
  basic_endpoint(const asio::ip::address& addr,
      port_type port_num) noexcept
    : impl_(addr, port_num)
  {
  }

  /// Copy constructor.
  basic_endpoint(const basic_endpoint& other) noexcept
    : impl_(other.impl_)
  {
  }

  /// Move constructor.
  basic_endpoint(basic_endpoint&& other) noexcept
    : impl_(other.impl_)
  {
  }

  /// Assign from another endpoint.
  basic_endpoint& operator=(const basic_endpoint& other) noexcept
  {
    impl_ = other.impl_;
    return *this;
  }

  /// Move-assign from another endpoint.
  basic_endpoint& operator=(basic_endpoint&& other) noexcept
  {
    impl_ = other.impl_;
    return *this;
  }

  /// The protocol associated with the endpoint.
  protocol_type protocol() const noexcept
  {
    if (impl_.is_v4())
      return InternetProtocol::v4();
    return InternetProtocol::v6();
  }

  /// Get the underlying endpoint in the native type.
  data_type* data() noexcept
  {
    return impl_.data();
  }

  /// Get the underlying endpoint in the native type.
  const data_type* data() const noexcept
  {
    return impl_.data();
  }

  /// Get the underlying size of the endpoint in the native type.
  std::size_t size() const noexcept
  {
    return impl_.size();
  }

  /// Set the underlying size of the endpoint in the native type.
  void resize(std::size_t new_size)
  {
    impl_.resize(new_size);
  }

  /// Get the capacity of the endpoint in the native type.
  std::size_t capacity() const noexcept
  {
    return impl_.capacity();
  }

  /// Get the port associated with the endpoint. The port number is always in
  /// the host's byte order.
  port_type port() const noexcept
  {
    return impl_.port();
  }

  /// Set the port associated with the endpoint. The port number is always in
  /// the host's byte order.
  void port(port_type port_num) noexcept
  {
    impl_.port(port_num);
  }

  /// Get the IP address associated with the endpoint.
  asio::ip::address address() const noexcept
  {
    return impl_.address();
  }

  /// Set the IP address associated with the endpoint.
  void address(const asio::ip::address& addr) noexcept
  {
    impl_.address(addr);
  }

  /// Compare two endpoints for equality.
  friend bool operator==(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) noexcept
  {
    return e1.impl_ == e2.impl_;
  }

  /// Compare two endpoints for inequality.
  friend bool operator!=(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) noexcept
  {
    return !(e1 == e2);
  }

  /// Compare endpoints for ordering.
  friend bool operator<(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) noexcept
  {
    return e1.impl_ < e2.impl_;
  }

  /// Compare endpoints for ordering.
  friend bool operator>(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) noexcept
  {
    return e2.impl_ < e1.impl_;
  }

  /// Compare endpoints for ordering.
  friend bool operator<=(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) noexcept
  {
    return !(e2 < e1);
  }

  /// Compare endpoints for ordering.
  friend bool operator>=(const basic_endpoint<InternetProtocol>& e1,
      const basic_endpoint<InternetProtocol>& e2) noexcept
  {
    return !(e1 < e2);
  }

private:
  // The underlying IP endpoint.
  asio::ip::detail::endpoint impl_;
};

#if !defined(ASIO_NO_IOSTREAM)

/// Output an endpoint as a string.
/**
 * Used to output a human-readable string for a specified endpoint.
 *
 * @param os The output stream to which the string will be written.
 *
 * @param endpoint The endpoint to be written.
 *
 * @return The output stream.
 *
 * @relates asio::ip::basic_endpoint
 */
template <typename Elem, typename Traits, typename InternetProtocol>
std::basic_ostream<Elem, Traits>& operator<<(
    std::basic_ostream<Elem, Traits>& os,
    const basic_endpoint<InternetProtocol>& endpoint);

#endif // !defined(ASIO_NO_IOSTREAM)

} // namespace ip
} // namespace asio

namespace std {

template <typename InternetProtocol>
struct hash<asio::ip::basic_endpoint<InternetProtocol>>
{
  std::size_t operator()(
      const asio::ip::basic_endpoint<InternetProtocol>& ep)
    const noexcept
  {
    std::size_t hash1 = std::hash<asio::ip::address>()(ep.address());
    std::size_t hash2 = std::hash<unsigned short>()(ep.port());
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
  }
};

} // namespace std

#include <Asio/detail/pop_options.hpp>

#include <Asio/ip/impl/basic_endpoint.hpp>

#endif // ASIO_IP_BASIC_ENDPOINT_HPP
