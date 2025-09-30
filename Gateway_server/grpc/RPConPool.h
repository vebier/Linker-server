#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPool {
public:
	RPConPool(const size_t size,const std::string& host,const std::string& post);
	~RPConPool();
	void Stop();
	std::unique_ptr<VarifyService::Stub> GetConnection();
	void returnConnection(std::unique_ptr<VarifyService::Stub> con);
private:
	size_t size_;
	std::atomic<bool> stop_;
	std::mutex mtx_;
	std::condition_variable cv_;
	std::queue<std::unique_ptr<VarifyService::Stub>> con_queue_;
	std::string host_;
	std::string port_;
};
