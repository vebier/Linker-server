#include "VarifyGrpcClient.h"
#include "SectionInfo.h"

VarifyGrpcClient::~VarifyGrpcClient()
{
}

GetVarifyRsp VarifyGrpcClient::GetVarifyCode(std::string email)
{
	ClientContext context;
	GetVarifyReq request;
	GetVarifyRsp reply;
	request.set_email(email);
	auto stub_ = pool_->GetConnection();
	Status status = stub_->GetVarifyCode(&context, request, &reply);
	pool_->returnConnection(std::move(stub_));
	if (status.ok()) {
		return reply;
	}
	else {
		reply.set_error(static_cast<int>(ErrorCode::RPCFAILED));
	}
}

VarifyGrpcClient::VarifyGrpcClient()
{
	auto& gCfgMgr = ConfigMgr::GetInstance();
	std::string host = gCfgMgr["VarifyServer"]["Host"];
	std::string port = gCfgMgr["VarifyServer"]["Port"];
	pool_ = std::make_unique<RPConPool>(10, host, port);
}
