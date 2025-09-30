#pragma once
#include "const.h"
#include "Singleton.h"

using IOContext = net::io_context;
using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
using WorkPtr = std::unique_ptr<Work>;


class AsioIOContextPool : public Singleton<AsioIOContextPool>
{
	friend class Singleton<AsioIOContextPool>;
public:
	IOContext& GetIOContext();
	~AsioIOContextPool();
	AsioIOContextPool(const AsioIOContextPool&) = delete;
	AsioIOContextPool& operator=(const AsioIOContextPool&) = delete;
	void Stop();
private:
	AsioIOContextPool(const size_t size= std::thread::hardware_concurrency());
	std::vector<IOContext> io_contexts_;
	std::vector<WorkPtr> works_;
	std::vector<std::thread> threads_;
	size_t _nextIOService;
};