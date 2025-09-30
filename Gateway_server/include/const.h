#pragma once

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <functional>
#include <map>
#include <unordered_map>
#include <atomic>
#include <condition_variable>
#include <future>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <cassert>
#include "json/json.h"
#include "json/value.h"
#include "json/reader.h"
#include "hiredis.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

enum class ErrorCode
{
	SUCCESS = 0,
	JSONERROR = 1001,
	RPCFAILED = 1002,
};

class ConfigMgr;
extern ConfigMgr gCfgMgr;