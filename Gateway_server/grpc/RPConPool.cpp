#include "RPConPool.h"
#include "MySQLPool.h"

RPConPool::RPConPool(const size_t size, const std::string& host, const std::string& port) :
	size_(size), host_(host), port_(port),stop_(false)
{
	for (size_t i = 0; i < size_; ++i) {
		con_queue_.push(VarifyService::NewStub(grpc::CreateChannel(host_+":"+port_, grpc::InsecureChannelCredentials())));
	}
}

RPConPool::~RPConPool()
{
	std::lock_guard<std::mutex> lock(mtx_);
	Stop();
	while (!con_queue_.empty()) {
		con_queue_.pop();
	}
}

void RPConPool::Stop() {
	stop_ = true;
	cv_.notify_all();
	
}

std::unique_ptr<VarifyService::Stub> RPConPool::GetConnection()
{
	std::unique_lock<std::mutex> lock(mtx_);
	cv_.wait(lock, [this]() {
		if (stop_ && con_queue_.empty())
			return false;
		return true;
	});
	if (stop_) {
		return  nullptr;
	}
	auto stub=std::move(con_queue_.front());
	con_queue_.pop();
	return stub;
}

void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> con)
{
	std::lock_guard<std::mutex> lock(mtx_);
	if(stop_)
		return;
	con_queue_.push(std::move(con));
	cv_.notify_one();
}
