#include "AsioIOContextPool.h"
#include "RPConPool.h"

IOContext& AsioIOContextPool::GetIOContext()
{
	auto& service = io_contexts_[_nextIOService++];
	
	if(_nextIOService== io_contexts_.size())
		_nextIOService = 0;
	return service;
}

AsioIOContextPool::~AsioIOContextPool()
{
	Stop();
}

void AsioIOContextPool::Stop()
{
	for (auto& io_context : io_contexts_) {
		io_context.stop();
	}

	for (auto& work : works_) {
		work->reset();
	}
	
	for (auto& thread : threads_) {
		if (thread.joinable())
			thread.join();
	}
}

AsioIOContextPool::AsioIOContextPool(const size_t size) :io_contexts_(size), _nextIOService(0)
{
	for (int i = 0; i < size; ++i) {
		works_.emplace_back(std::make_unique<Work>(io_contexts_[i].get_executor()));
	}

	for(int i = 0; i < size; ++i) {
		threads_.emplace_back([this,i]() {
			io_contexts_[i].run();
			});
	}
}
