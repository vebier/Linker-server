#pragma once
#include "const.h"

class RedisPool {
public:
	RedisPool(const std::string& host, int port, const std::string& password, size_t poolSize = 5);
	~RedisPool();
	redisContext* Acquire();
	void Release(redisContext* context);
	void Close();
	size_t Size();
private:
	std::queue<redisContext*> que_;
	std::mutex mutex_;
	std::condition_variable cv_;
	size_t poolSize_;
	std::atomic<bool> stop_;
};
