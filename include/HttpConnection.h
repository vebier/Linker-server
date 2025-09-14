#pragma once
#include "const.h"

class HttpConncation :public std::enable_shared_from_this<HttpConncation>
{
	friend class LogicSystem;
public:
	HttpConncation(tcp::socket&& socket);
	void run();
private:
	tcp::socket socket_;
	void CheckDeadline();
	void WriteResponse();
	void HandleReq();
	beast::flat_buffer buffer_;
	http::request<http::dynamic_body> req_;
	http::response<http::dynamic_body> res_;
	net::steady_timer deadline_;

};