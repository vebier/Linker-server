#include "LogicSystem.h"
#include "HttpConnection.h"
#include "ThreadPool.h"

LogicSystem::~LogicSystem()
{

}

bool LogicSystem::HandleGet(std::string_view url, std::shared_ptr<HttpConncation> connection)
{
    auto it = get_handlers_.find(std::string(url));
    if (it == get_handlers_.end()) {
        return false;
    }

    ThreadPool::instance().commit([handler = it->second, conn = connection]() {
        try {
            handler(conn);
            // È·±£·¢ËÍ
            conn->res_.result(http::status::ok);
            conn->res_.insert(http::field::server, "Gateway_server");
            conn->WriteResponse();
        }
        catch (const std::exception& e) {
            std::cerr << "Error in handler: " << e.what() << std::endl;                
        }
        });
	return true;
}

LogicSystem::LogicSystem()
{
	get_handlers_["/get_test"] = [](std::shared_ptr<HttpConncation> connection) {
		beast::ostream(connection->res_.body()) << "receive get_test req";
	};
}
