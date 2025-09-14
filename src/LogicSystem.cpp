#include "LogicSystem.h"
#include "HttpConnection.h"

LogicSystem::~LogicSystem()
{

}

bool LogicSystem::HandleGet(std::string_view url, std::shared_ptr<HttpConncation> connection)
{
	if (get_handlers_.find(url) == get_handlers_.end()) {
		return false;
	}
	get_handlers_[url](connection);
	return true;
}

LogicSystem::LogicSystem()
{
	get_handlers_["/get_test"] = [](std::shared_ptr<HttpConncation> connection) {
		beast::ostream(connection->res_.body()) << "receive get_test req";
	};
}
