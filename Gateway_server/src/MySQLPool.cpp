#include "MySQLPool.h"

Connection::Connection(std::unique_ptr<sql::Connection> con, size_t last_used_time) : con_(std::move(con)), last_used_time_(last_used_time){}

MySQLPool::MySQLPool(const std::string& url, const std::string& user, const std::string& password, const std::string& schema, size_t pool_size):
	url_(url), user_(user), password_(password), schema_(schema), pool_size_(pool_size), is_shutdown_(false) {
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
		for (size_t i = 0; i < pool_size_; ++i) {
			std::unique_ptr<sql::Connection> con(driver->connect(url_, user_, password_));
			con->setSchema(schema);
			auto curr_time = std::chrono::system_clock::now().time_since_epoch();
			size_t curr_time_ms = std::chrono::duration_cast<std::chrono::seconds>(curr_time).count();
			auto conn = std::make_unique<Connection>(std::move(con), curr_time_ms);
			connections_.push(std::move(conn));
		}
		monitor_thread_ = std::thread([this]() {
			if(!is_shutdown_) {
				checkConnectionPro();
				std::this_thread::sleep_for(std::chrono::hours(5));
			}
		});
		monitor_thread_.detach();
	}
	catch (const sql::SQLException& e) {
		std::cerr << "MySQLPool Error: " << e.what() << std::endl;
		throw;
	}
}
MySQLPool::~MySQLPool()
{
	std::lock_guard<std::mutex> lock(mutex_);
	while (!connections_.empty()) {
		connections_.pop();
	}
}

void MySQLPool::Stop() {
	is_shutdown_ = true;
	cond_.notify_all();
}

std::unique_ptr<Connection> MySQLPool::GetConnection() {
	std::unique_lock<std::mutex> lock(mutex_);
	cond_.wait(lock, [this]() { return is_shutdown_ || !connections_.empty(); });
	if (is_shutdown_) {
		return nullptr;
	}
	auto conn = std::move(connections_.front());
	connections_.pop();
	return conn;
}

void MySQLPool::returnConnection(std::unique_ptr<Connection> conn) {
	if (conn == nullptr) {
		return;
	}
	std::lock_guard<std::mutex> lock(mutex_);
	if (is_shutdown_) {
		return;
	}
	connections_.push(std::move(conn));
	cond_.notify_one();
}

void MySQLPool::checkConnectionPro()
{
	size_t pool_size = 0;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		pool_size = connections_.size();
	}
	auto curr_time = std::chrono::system_clock::now().time_since_epoch();
	size_t curr_time_ms = std::chrono::duration_cast<std::chrono::seconds>(curr_time).count();
	size_t processed = 0;

	size_t lost_connections_count = 0;
	while (processed< pool_size) {
		
		std::unique_ptr<Connection> con;
		{
			std::lock_guard<std::mutex> lock(mutex_);
			con = std::move(connections_.front());
			connections_.pop();
		}

		if (curr_time_ms - con->last_used_time_ > 18000) {
			try {
				std::unique_ptr<sql::Statement> stmt(con->con_->createStatement());
				stmt->executeQuery("Select 1");
				con->last_used_time_ = curr_time_ms;
			}
			catch (sql::SQLException& e) {
				std::cout << "MySQL连接已丢失，错误原因 :  " << e.what() << std::endl;
				lost_connections_count++;
				continue; // Skip returning this connection to the pool
			}
		}
		{
			std::lock_guard<std::mutex> lock(mutex_);
			connections_.push(std::move(con));
		}
		processed++;
	}
	if (lost_connections_count > 0) {
		size_t success = reconnect(lost_connections_count);
		std::cout << "成功重连 " << success << " 个MySQL连接" << std::endl;
	}
}

size_t MySQLPool::reconnect(size_t lost_connections_count)
{
	std::cerr << "丢失MySQL连接个数 " << lost_connections_count << " 创建新MySQL连接投递连接池中" << std::endl;
	sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
	for (size_t i = 0; i < lost_connections_count; ++i) {
		try {
			std::unique_ptr<sql::Connection> con(driver->connect(url_, user_, password_));
			con->setSchema(schema_);
			auto curr_time = std::chrono::system_clock::now().time_since_epoch();
			size_t curr_time_ms = std::chrono::duration_cast<std::chrono::seconds>(curr_time).count();
			auto conn = std::make_unique<Connection>(std::move(con), curr_time_ms);
			std::lock_guard<std::mutex> lock(mutex_);
			connections_.push(std::move(conn));
		}
		catch (const sql::SQLException& e) {
			std::cerr << "创建新MySQL连接失败 :  " << e.what() << std::endl;
			return i;
		}
	}
	return lost_connections_count;
}
