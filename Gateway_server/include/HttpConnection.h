#pragma once
#include "const.h"

class HttpConnection :public std::enable_shared_from_this<HttpConnection>
{
	friend class LogicSystem;
public:
	HttpConnection(net::io_context& ioc);
	tcp::socket& GetSocket();
	void run();
private:
	tcp::socket socket_;
	void CheckDeadline();
	void WriteResponse();
	void HandleReq();
	void PreParseGetParam();
	beast::flat_buffer buffer_;//beast提供的读写缓冲区
	http::request<http::dynamic_body> req_;//请求对象
	http::response<http::dynamic_body> res_;//响应对象
	net::steady_timer deadline_;//超时定时器
	std::string _get_url;//存储get请求的url
	std::unordered_map<std::string, std::string> _get_params;//存储get请求的参数
};