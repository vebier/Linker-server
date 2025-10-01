#include "LogicSystem.h"
#include "HttpConnection.h"
#include "ThreadPool.h"
#include "VarifyGrpcClient.h"
#include "RedisMgr.h"

LogicSystem::~LogicSystem()
{

}

bool LogicSystem::HandleGet(const std::string url, std::shared_ptr<HttpConnection> connection)
{
    auto it = get_handlers_.find(url);
    if (it == get_handlers_.end()) {
        return false;
    }

    auto& handler = it->second;
    handler(connection);
    
    /*auto res = ThreadPool::instance().commit([&handler, conn = connection]() {
        handler(conn);
        conn->res_.result(http::status::ok);
        conn->res_.insert(http::field::server, "Gateway_server");
        conn->WriteResponse();
    });*/

	return true;
}

bool LogicSystem::HandlePost(const std::string url, std::shared_ptr<HttpConnection> connection)
{
    auto it = post_handlers_.find(url);
    if (it == post_handlers_.end()) {
        return false;
    }

    auto& handler = it->second;
    handler(connection);
    
    /*ThreadPool::instance().commit([&handler, conn = connection]() mutable {
        handler(conn);
        conn->res_.result(http::status::ok);
        conn->res_.insert(http::field::server, "Gateway_server");
        conn->WriteResponse();
    });*/

    return true;
}

LogicSystem::LogicSystem()
{
	get_handlers_["/get_test"] = [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->res_.body()) << "receive get_test req " << std::endl;
        int i = 0;
        for (auto& elem : connection->_get_params) {
            i++;
            beast::ostream(connection->res_.body()) << "param" << i << " key is " << elem.first;
            beast::ostream(connection->res_.body()) << ", " << " value is " << elem.second << std::endl;
        }
	};

    post_handlers_["/get_varifycode"] = [](std::shared_ptr<HttpConnection> connection) {
        auto body = beast::buffers_to_string(connection->req_.body().data());
        std::cout << "验证请求body : " << body << std::endl;
        connection->res_.set(http::field::content_type, "text/json");
        
        Json::Value root;
        Json::Value src_root;
		Json::Reader reader;
		
		auto parse = reader.parse(body, src_root);
        if(!parse) {
			std::cerr << "验证请求 json 解析失败" << std::endl;
			root["code"] = static_cast<int>(ErrorCode::Error_Json);
			auto json_src = root.toStyledString();
            beast::ostream(connection->res_.body()) << json_src;
            return;
		}

        if (!src_root.isMember("email")) {
            std::cerr << "验证请求 json 参数错误" << std::endl;
            root["code"] = static_cast<int>(ErrorCode::Error_Json);
            auto json_src = root.toStyledString();
            beast::ostream(connection->res_.body()) << json_src;
            return;
        }

		std::string email = src_root["email"].asString();
        // 业务逻辑处理
		auto reply =VarifyGrpcClient::GetInstance()->GetVarifyCode(email);
		root["email"] = reply.email();
		root["error"] = reply.error();

        auto json_src = root.toStyledString();
        beast::ostream(connection->res_.body()) << json_src;
        return;
    };

    post_handlers_["/user_register"] = [](std::shared_ptr<HttpConnection> connection) {
        auto body = beast::buffers_to_string(connection->req_.body().data());
        std::cout << "注册请求body : " << body << std::endl;
        connection->res_.set(http::field::content_type, "text/json");

        Json::Value root;
        Json::Value src_root;
        Json::Reader reader;

        auto parse = reader.parse(body, src_root);
        if (!parse) {
            std::cerr << "注册请求 json 解析失败" << std::endl;
            root["code"] = static_cast<int>(ErrorCode::Error_Json);
            auto json_src = root.toStyledString();
            beast::ostream(connection->res_.body()) << json_src;
            return;
        }

		auto user = src_root["user"].asString();
		auto email = src_root["email"].asString();
		auto pass = src_root["pass"].asString();
		auto confirm = src_root["confirm"].asString();
		auto verification = src_root["verification"].asString();

        std::string code;

        if (!RedisMgr::GetInstance()->Get(code_prefix + email, code)) {
            std::cerr << "验证码过期" << std::endl;
            root["code"] = static_cast<int>(ErrorCode::VarifyExpired);
            auto json_src = root.toStyledString();
            beast::ostream(connection->res_.body()) << json_src;
            return;
        }

        if (code != verification) {
            std::cerr << "验证码错误" << std::endl;
            root["code"] = static_cast<int>(ErrorCode::VarifyCodeErr);
            auto json_src = root.toStyledString();
            beast::ostream(connection->res_.body()) << json_src;
            return;
		}

        if (pass != confirm) {
            std::cerr << "密码与确认密码不匹配" << std::endl;
            root["code"] = static_cast<int>(ErrorCode::PasswdErr);
            auto json_src = root.toStyledString();
            beast::ostream(connection->res_.body()) << json_src;
            return;
		}

        root["code"] = static_cast<int>(ErrorCode::Success);
		root["user"] = user;
		root["email"] = email;
		root["pass"] = pass;
		root["confirm"] = confirm; 
        root["verification"] = verification;
        auto json_src = root.toStyledString();
        beast::ostream(connection->res_.body()) << json_src;
        return;
    };
}
