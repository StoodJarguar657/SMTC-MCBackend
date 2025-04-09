//
// ts/executor.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef ASIO_TS_EXECUTOR_HPP
#define ASIO_TS_EXECUTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <Asio/async_result.hpp>
#include <Asio/associated_allocator.hpp>
#include <Asio/execution_context.hpp>
#include <Asio/is_executor.hpp>
#include <Asio/associated_executor.hpp>
#include <Asio/bind_executor.hpp>
#include <Asio/executor_work_guard.hpp>
#include <Asio/system_executor.hpp>
#include <Asio/executor.hpp>
#include <Asio/any_io_executor.hpp>
#include <Asio/dispatch.hpp>
#include <Asio/post.hpp>
#include <Asio/defer.hpp>
#include <Asio/strand.hpp>
#include <Asio/packaged_task.hpp>
#include <Asio/use_future.hpp>

#endif // ASIO_TS_EXECUTOR_HPP
