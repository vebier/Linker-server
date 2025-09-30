#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOContextPool.h"
CServer::CServer(net::io_context& ioc,unsigned short& port):ioc_(ioc), 
acceptor_(ioc_,tcp::endpoint(tcp::v4(), port))
{

}

void CServer::run()
{
	auto self(shared_from_this());
	auto& ioc=AsioIOContextPool::GetInstance()->GetIOContext();
	auto connection = std::make_shared<HttpConnection>(ioc);
	acceptor_.async_accept(connection->GetSocket(), [self, connection](boost::system::error_code ec) {//接收连接后的回调函数
		try{//这里用try捕获异常是因为防止accept失败抛出异常导致崩溃无法闭环
			if (ec) {
				std::cerr << "accept Failed: " << ec.message() << std::endl;
				self->run();
				return;
			}
			connection->run();
			//启动下一个异步接受操作
			self->run();
		}
		catch (const std::exception& e) {
			std::cerr << "Exception in accept: " << e.what() << std::endl;
			self->run();
		}
	});
}