#pragma once
#include "Singleton.h"

class HttpConncation;
using HttpHandler=std::function<void(std::shared_ptr<HttpConncation>)>;

class LogicSystem :public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	bool HandleGet(std::string_view, std::shared_ptr<HttpConncation>);
private:
	LogicSystem();

	std::map<std::string_view, HttpHandler> post_handlers_;
	std::map<std::string_view, HttpHandler> get_handlers_;
};
