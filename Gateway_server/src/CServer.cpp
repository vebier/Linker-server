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
	acceptor_.async_accept(connection->GetSocket(), [self, connection](boost::system::error_code ec) {//�������Ӻ�Ļص�����
		try{//������try�����쳣����Ϊ��ֹacceptʧ���׳��쳣���±����޷��ջ�
			if (ec) {
				std::cerr << "accept Failed: " << ec.message() << std::endl;
				self->run();
				return;
			}
			connection->run();
			//������һ���첽���ܲ���
			self->run();
		}
		catch (const std::exception& e) {
			std::cerr << "Exception in accept: " << e.what() << std::endl;
			self->run();
		}
	});
}