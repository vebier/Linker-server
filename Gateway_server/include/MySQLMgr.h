#pragma once
#include "MySQLDao.h"
#include "Singleton.h"

class MySQLMgr : public Singleton<MySQLMgr>, public std::enable_shared_from_this<MySQLMgr> {
	friend class Singleton<MySQLMgr>;
public:
	~MySQLMgr();
	int reg_user(const std::string& name, const std::string& email, const std::string& pwd);
private:
	MySQLMgr();
	MySQLDao dao_;
};