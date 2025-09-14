#include "HttpConnection.h"
#include "LogicSystem.h"

HttpConncation::HttpConncation(tcp::socket&& socket) : socket_(std::move(socket)),
deadline_{socket_.get_executor(), std::chrono::seconds(60)}
{

}
void HttpConncation::run() {
	auto self(shared_from_this());
	http::async_read(socket_, buffer_, req_,[self](boost::system::error_code ec, std::size_t bytes_transferred) {
		try {
			if(ec) {
				std::cerr << "http_read Failed: " << ec.message() << std::endl;
				return;
			}
			std::cout << "接收到HTTP请求" << std::endl;
			boost::ignore_unused(bytes_transferred);
			self->HandleReq();
			self->CheckDeadline();
		}
		catch (const std::exception& e) {
			std::cerr << "Exception in http_read: " << e.what() << std::endl;
		}
	});
}

void HttpConncation::CheckDeadline()
{
	auto self(shared_from_this());
	deadline_.async_wait([self](boost::system::error_code ec) {
		try {
			if(!ec) {
				// Close socket to cancel any outstanding operation.
				self->socket_.close(ec);
				return;
			}
		}
		catch (const std::exception& e) {
			std::cerr << "Exception in CheckDeadline: " << e.what() << std::endl;
		}
		});
}

void HttpConncation::WriteResponse()
{
	auto self = shared_from_this();
	res_.content_length(res_.body().size());
	http::async_write(socket_,res_,[self](beast::error_code ec, std::size_t){
		self->socket_.shutdown(tcp::socket::shutdown_send, ec);
		self->deadline_.cancel();
	});
}

void HttpConncation::HandleReq()
{
	auto self(shared_from_this());
	res_.version(req_.version());
	res_.keep_alive(false);
	if(req_.method() == http::verb::get) {
		bool success = LogicSystem::GetInstance()->HandleGet(req_.target(), shared_from_this());
		if(!success) {
			res_.result(http::status::not_found);
			res_.set(http::field::content_type, "text/plain");
			beast::ostream(res_.body()) << "url not found\r\n";
			WriteResponse();
		}
		return;
	}
}
