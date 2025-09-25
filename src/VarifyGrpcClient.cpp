#include "VarifyGrpcClient.h"

VarifyGrpcClient::~VarifyGrpcClient()
{
}

GetVarifyRsp VarifyGrpcClient::GetVarifyCode(std::string email)
{
	ClientContext context;
	GetVarifyReq request;
	GetVarifyRsp reply;
	request.set_email(email);
	Status status = stub_->GetVarifyCode(&context, request, &reply);
	if (status.ok()) {
		return reply;
	}
	else {
		reply.set_error(static_cast<int>(ErrorCode::RPCFAILED));
	}
}

VarifyGrpcClient::VarifyGrpcClient()
{
	stub_ = VarifyService::NewStub(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
}
