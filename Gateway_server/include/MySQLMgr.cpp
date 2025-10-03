#include "MySQLMgr.h"

MySQLMgr::~MySQLMgr()
{
}

int MySQLMgr::reg_user(const std::string& name, const std::string& email, const std::string& pwd)
{
	return dao_.RegUser(name, email, pwd);
}

MySQLMgr::MySQLMgr()
{
}
