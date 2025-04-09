//
// impl/src.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_IMPL_SRC_HPP
#define ASIO_IMPL_SRC_HPP

#define ASIO_SOURCE

#include <Asio/detail/config.hpp>

#if defined(ASIO_HEADER_ONLY)
# error Do not compile Asio library source with ASIO_HEADER_ONLY defined
#endif

#include <Asio/impl/any_completion_executor.ipp>
#include <Asio/impl/any_io_executor.ipp>
#include <Asio/impl/cancellation_signal.ipp>
#include <Asio/impl/connect_pipe.ipp>
#include <Asio/impl/error.ipp>
#include <Asio/impl/error_code.ipp>
#include <Asio/impl/execution_context.ipp>
#include <Asio/impl/executor.ipp>
#include <Asio/impl/io_context.ipp>
#include <Asio/impl/multiple_exceptions.ipp>
#include <Asio/impl/serial_port_base.ipp>
#include <Asio/impl/system_context.ipp>
#include <Asio/impl/thread_pool.ipp>
#include <Asio/detail/impl/buffer_sequence_adapter.ipp>
#include <Asio/detail/impl/descriptor_ops.ipp>
#include <Asio/detail/impl/dev_poll_reactor.ipp>
#include <Asio/detail/impl/epoll_reactor.ipp>
#include <Asio/detail/impl/eventfd_select_interrupter.ipp>
#include <Asio/detail/impl/handler_tracking.ipp>
#include <Asio/detail/impl/io_uring_descriptor_service.ipp>
#include <Asio/detail/impl/io_uring_file_service.ipp>
#include <Asio/detail/impl/io_uring_socket_service_base.ipp>
#include <Asio/detail/impl/io_uring_service.ipp>
#include <Asio/detail/impl/kqueue_reactor.ipp>
#include <Asio/detail/impl/null_event.ipp>
#include <Asio/detail/impl/pipe_select_interrupter.ipp>
#include <Asio/detail/impl/posix_event.ipp>
#include <Asio/detail/impl/posix_mutex.ipp>
#include <Asio/detail/impl/posix_serial_port_service.ipp>
#include <Asio/detail/impl/posix_thread.ipp>
#include <Asio/detail/impl/posix_tss_ptr.ipp>
#include <Asio/detail/impl/reactive_descriptor_service.ipp>
#include <Asio/detail/impl/reactive_socket_service_base.ipp>
#include <Asio/detail/impl/resolver_service_base.ipp>
#include <Asio/detail/impl/scheduler.ipp>
#include <Asio/detail/impl/select_reactor.ipp>
#include <Asio/detail/impl/service_registry.ipp>
#include <Asio/detail/impl/signal_set_service.ipp>
#include <Asio/detail/impl/socket_ops.ipp>
#include <Asio/detail/impl/socket_select_interrupter.ipp>
#include <Asio/detail/impl/strand_executor_service.ipp>
#include <Asio/detail/impl/strand_service.ipp>
#include <Asio/detail/impl/thread_context.ipp>
#include <Asio/detail/impl/throw_error.ipp>
#include <Asio/detail/impl/timer_queue_ptime.ipp>
#include <Asio/detail/impl/timer_queue_set.ipp>
#include <Asio/detail/impl/win_iocp_file_service.ipp>
#include <Asio/detail/impl/win_iocp_handle_service.ipp>
#include <Asio/detail/impl/win_iocp_io_context.ipp>
#include <Asio/detail/impl/win_iocp_serial_port_service.ipp>
#include <Asio/detail/impl/win_iocp_socket_service_base.ipp>
#include <Asio/detail/impl/win_event.ipp>
#include <Asio/detail/impl/win_mutex.ipp>
#include <Asio/detail/impl/win_object_handle_service.ipp>
#include <Asio/detail/impl/win_static_mutex.ipp>
#include <Asio/detail/impl/win_thread.ipp>
#include <Asio/detail/impl/win_tss_ptr.ipp>
#include <Asio/detail/impl/winrt_ssocket_service_base.ipp>
#include <Asio/detail/impl/winrt_timer_scheduler.ipp>
#include <Asio/detail/impl/winsock_init.ipp>
#include <Asio/execution/impl/bad_executor.ipp>
#include <Asio/experimental/impl/channel_error.ipp>
#include <Asio/generic/detail/impl/endpoint.ipp>
#include <Asio/ip/impl/address.ipp>
#include <Asio/ip/impl/address_v4.ipp>
#include <Asio/ip/impl/address_v6.ipp>
#include <Asio/ip/impl/host_name.ipp>
#include <Asio/ip/impl/network_v4.ipp>
#include <Asio/ip/impl/network_v6.ipp>
#include <Asio/ip/detail/impl/endpoint.ipp>
#include <Asio/local/detail/impl/endpoint.ipp>

#endif // ASIO_IMPL_SRC_HPP
