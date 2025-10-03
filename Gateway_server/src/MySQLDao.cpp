#include "MySQLDao.h"
#include "SectionInfo.h"
MySQLDao::MySQLDao()
{
	auto config = ConfigMgr::GetInstance();
	//const std::string& url, const std::string& user, const std::string& password, const std::string& schema
	pool_ = std::make_unique<MySQLPool>(config["MysqlServer"]["Host"]+":"+ config["MysqlServer"]["Port"], config["MysqlServer"]["User"], config["MysqlServer"]["Passwd"], config["MysqlServer"]["Schema"], 10);
}

MySQLDao::~MySQLDao()
{
	pool_->Stop();
}

int MySQLDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    auto con = pool_->GetConnection();
    try {
        if (con == nullptr) {
            pool_->returnConnection(std::move(con));
            return false;
        }
        // 准备调用存储过程
        std::unique_ptr < sql::PreparedStatement > stmt(con->con_->prepareStatement("CALL reg_user(?,?,?,@result)"));
        // 设置输入参数
        stmt->setString(1, name);
        stmt->setString(2, email);
        stmt->setString(3, pwd);
        // 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
          // 执行存储过程
        stmt->execute();
        // 如果存储过程设置了会话变量或有其他方式获取输出参数的值，你可以在这里执行SELECT查询来获取它们
       // 例如，如果存储过程设置了一个会话变量@result来存储输出结果，可以这样获取：
        std::unique_ptr<sql::Statement> stmtResult(con->con_->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));
        if (res->next()) {
            int result = res->getInt("result");
            //std::cout << "Result: " << result << std::endl;
            pool_->returnConnection(std::move(con));
            return result;
        }
        pool_->returnConnection(std::move(con));
        return -1;
    }
    catch (sql::SQLException& e) {
        pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1;
    }
}
