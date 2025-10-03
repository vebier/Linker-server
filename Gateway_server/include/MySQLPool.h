#pragma once
#include "const.h"
#include <jdbc/mysql_driver.h>           // 获取驱动
#include <jdbc/cppconn/connection.h>     // Connection 接口
#include <jdbc/cppconn/prepared_statement.h> // 预编译语句 (安全)
#include <jdbc/cppconn/resultset.h>      // 处理结果
#include <jdbc/cppconn/exception.h>      // 异常处理

struct Connection {
	Connection(std::unique_ptr<sql::Connection> con, size_t last_used_time);
	std::unique_ptr<sql::Connection> con_;
	size_t last_used_time_;
};

class MySQLPool {
public:
	MySQLPool(const std::string& url, const std::string& user, const std::string& password, const std::string& schema,size_t pool_size = 5);
	~MySQLPool();
	void Stop();
	std::unique_ptr<Connection> GetConnection();
	void returnConnection(std::unique_ptr<Connection> conn);
	void checkConnectionPro();
	size_t reconnect(size_t lost_connections_count);
private:
	std::string url_;
	std::string user_;
	std::string password_;
	std::string schema_;
	size_t pool_size_;
	std::queue<std::unique_ptr<Connection>> connections_;
	std::mutex mutex_;
	std::condition_variable cond_;
	std::atomic<bool> is_shutdown_;
	std::thread monitor_thread_;
};
