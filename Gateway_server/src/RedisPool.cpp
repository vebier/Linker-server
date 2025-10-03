#include "RedisPool.h"

RedisPool::RedisPool(const std::string& host, int port, const std::string& password, size_t poolSize): poolSize_(poolSize), stop_(true)
{
	for (size_t i = 0; i < poolSize_; ++i) {
		redisContext* context = redisConnect(host.c_str(), port);
		if (context == nullptr || context->err) {
			std::cerr << "Failed to connect to Redis: " << (context ? context->errstr : "Unknown error") << std::endl;
			if (context) {
				redisFree(context);
			}
			continue;
		}
		if (!password.empty()) {
			redisReply* reply = (redisReply*)redisCommand(context, "AUTH %s", password.c_str());
			if (reply == nullptr || reply->type == REDIS_REPLY_ERROR) {
				std::cerr << "Failed to authenticate to Redis: " << (reply ? reply->str : "Unknown error") << std::endl;
				if (reply) {
					freeReplyObject(reply);
				}
				redisFree(context);
				continue;
			}
			freeReplyObject(reply);
		}
		que_.emplace(context);
	}
}

RedisPool::~RedisPool()
{
	Close();
	while(!que_.empty()) {
		auto context = que_.front();
		que_.pop();
		//redisFree(context);
	}
}

redisContext* RedisPool::Acquire()
{
	std::unique_lock<std::mutex> lock(mutex_);
	cv_.wait(lock, [this]() { return !stop_ || !que_.empty(); });
	if (!stop_) {
		return nullptr;
	}
	auto context = que_.front();
	que_.pop();
	return context;
}

void RedisPool::Release(redisContext* context)
{
	if (context == nullptr) {
		return;
	}
	std::lock_guard<std::mutex> lock(mutex_);
	if (!stop_) {
		redisFree(context);
		return;
	}
	que_.emplace(context);
	cv_.notify_one();
}

void RedisPool::Close() {
	stop_ = false;
	cv_.notify_all();
}

size_t RedisPool::Size()
{
	return poolSize_;
}

