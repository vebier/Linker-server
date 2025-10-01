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
	Success = 0,
	Error_Json = 1001,  //Json��������
	RPCFailed = 1002,  //RPC�������
	VarifyExpired = 1003, //��֤�����
	VarifyCodeErr = 1004, //��֤�����
	UserExist = 1005,       //�û��Ѿ�����
	PasswdErr = 1006,    //�������
	EmailNotMatch = 1007,  //���䲻ƥ��
	PasswdUpFailed = 1008,  //��������ʧ��
	PasswdInvalid = 1009,   //�������ʧ��
	TokenInvalid = 1010,   //TokenʧЧ
	UidInvalid = 1011,  //uid��Ч
};

#define code_prefix "code_"