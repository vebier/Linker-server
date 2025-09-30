#include "LogicSystem.h"
#include "HttpConnection.h"
#include "ThreadPool.h"
#include "VarifyGrpcClient.h"

LogicSystem::~LogicSystem()
{

}

bool LogicSystem::HandleGet(const std::string url, std::shared_ptr<HttpConnection> connection)
{
    auto it = get_handlers_.find(std::string(url));
    if (it == get_handlers_.end()) {
        return false;
    }

    /*auto func = it->second;
	(*func)(connection);*/
    auto res = ThreadPool::instance().commit([func = it->second, conn = connection]() {

        (*func)(conn);
        // ȷ������
        conn->res_.result(http::status::ok);
        conn->res_.insert(http::field::server, "Gateway_server");
        conn->WriteResponse();
    });

	return true;
}

bool LogicSystem::HandlePost(const std::string url, std::shared_ptr<HttpConnection> connection)
{
    auto it = post_handlers_.find(std::string(url));
    if (it == post_handlers_.end()) {
        return false;
    }
    /*auto func = it->second;
    (*func)(connection);*/
    auto res = ThreadPool::instance().commit([func = it->second, conn = connection]() {

        (*func)(conn);
        // ȷ������
        conn->res_.result(http::status::ok);
        conn->res_.insert(http::field::server, "Gateway_server");
        conn->WriteResponse();
        });

    return true;
}

LogicSystem::LogicSystem()
{
	get_handlers_["/get_test"] = std::make_shared<std::function<void(std::shared_ptr<HttpConnection>)>>([](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->res_.body()) << "receive get_test req " << std::endl;
        int i = 0;
        for (auto& elem : connection->_get_params) {
            i++;
            beast::ostream(connection->res_.body()) << "param" << i << " key is " << elem.first;
            beast::ostream(connection->res_.body()) << ", " << " value is " << elem.second << std::endl;
        }
	});

    post_handlers_["/get_varifycode"] = std::make_shared<std::function<void(std::shared_ptr<HttpConnection>)>>([](std::shared_ptr<HttpConnection> connection) {
        auto body = beast::buffers_to_string(connection->req_.body().data());
        std::cout << "��֤����body : " << body << std::endl;
        connection->res_.set(http::field::content_type, "text/json");
        
        Json::Value root;
        Json::Value src_root;
		Json::Reader reader;
		
		auto parse = reader.parse(body, src_root);
        if(!parse) {
			std::cerr << "��֤���� json ����ʧ��" << std::endl;
			root["code"] = static_cast<int>(ErrorCode::JSONERROR);
			auto json_src = root.toStyledString();
            beast::ostream(connection->res_.body()) << json_src;
            return;
		}

        if (!src_root.isMember("email")) {
            std::cerr << "��֤���� json ��������" << std::endl;
            root["code"] = static_cast<int>(ErrorCode::JSONERROR);
            auto json_src = root.toStyledString();
            beast::ostream(connection->res_.body()) << json_src;
            return;
        }

		std::string email = src_root["email"].asString();
        // ҵ���߼�����
		auto reply =VarifyGrpcClient::GetInstance()->GetVarifyCode(email);
		root["email"] = reply.email();
		root["error"] = reply.error();

        auto json_src = root.toStyledString();
        beast::ostream(connection->res_.body()) << json_src;
        return;
    });
}
