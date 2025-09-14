#include "CServer.h"
#include "HttpConnection.h"
CServer::CServer(net::io_context& ioc,unsigned short& port):ioc_(ioc), 
acceptor_(ioc_,tcp::endpoint(tcp::v4(), port)), socket_(ioc)
{

}

void CServer::run()
{
	auto self(shared_from_this());
	acceptor_.async_accept(socket_,[self](boost::system::error_code ec){//接收连接后的回调函数
		try{//这里用try捕获异常是因为防止accept失败抛出异常导致崩溃无法闭环
			if (ec) {
				std::cerr << "accept Failed: " << ec.message() << std::endl;
				self->run();
				return;
			}
			std::make_shared<HttpConncation>(std::move(self->socket_))->run();
			//启动下一个异步接受操作
			self->run();
		}
		catch (const std::exception& e) {
			std::cerr << "Exception in accept: " << e.what() << std::endl;
			self->run();
		}
	});
}