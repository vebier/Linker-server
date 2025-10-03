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
        // ׼�����ô洢����
        std::unique_ptr < sql::PreparedStatement > stmt(con->con_->prepareStatement("CALL reg_user(?,?,?,@result)"));
        // �����������
        stmt->setString(1, name);
        stmt->setString(2, email);
        stmt->setString(3, pwd);
        // ����PreparedStatement��ֱ��֧��ע�����������������Ҫʹ�ûỰ������������������ȡ���������ֵ
          // ִ�д洢����
        stmt->execute();
        // ����洢���������˻Ự��������������ʽ��ȡ���������ֵ�������������ִ��SELECT��ѯ����ȡ����
       // ���磬����洢����������һ���Ự����@result���洢������������������ȡ��
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
