
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <regex>
#include <iostream>
#include <tuple>
#include <mutex>
#include "winsock_initializer.h"

#define BOOST_SPIRIT_THREADSAFE // enable thread safety to json read on property tree!
#define BOOST_USE_WINAPI_VERSION 0x0600

#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/asio.hpp>

#include <boost/algorithm/string.hpp>

#include <cpprest/asyncrt_utils.h>

#include <cpprest/details/basic_types.h>
#include <cpprest/json.h>

#include "MT4ManagerAPI.h"
#include "logger.hpp"