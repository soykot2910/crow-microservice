#pragma once

#include <boost/log/core.hpp>
#include <boost/log/utility/setup/from_stream.hpp>

#define loginfo BOOST_LOG_TRIVIAL(info)
#define logerror BOOST_LOG_TRIVIAL(error)
#define logdebug BOOST_LOG_TRIVIAL(debug)

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

using namespace boost::log::trivial;