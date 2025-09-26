#pragma once
#include "Singleton.h"

class HttpConnection;
using HttpHandler=std::shared_ptr<std::function<void(std::shared_ptr<HttpConnection>)>>;

class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	bool HandleGet(const std::string, std::shared_ptr<HttpConnection>);
	bool HandlePost(const std::string, std::shared_ptr<HttpConnection>);
private:
	LogicSystem();
	std::map<std::string, HttpHandler> post_handlers_;
	std::map<std::string, HttpHandler> get_handlers_;
};
