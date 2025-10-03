#pragma once
#include "MySQLPool.h"

class MySQLDao {
public:
	MySQLDao();
	~MySQLDao();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
private:
	std::unique_ptr<MySQLPool> pool_;
};
