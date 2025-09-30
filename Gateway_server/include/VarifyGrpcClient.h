#pragma once
#include "Singleton.h"
#include "RPConPool.h"

class VarifyGrpcClient : public Singleton<VarifyGrpcClient>
{
	friend class Singleton<VarifyGrpcClient>;
public:
	~VarifyGrpcClient();
	GetVarifyRsp GetVarifyCode(std::string email);
private:
	VarifyGrpcClient();
	std::unique_ptr<RPConPool> pool_;
};

