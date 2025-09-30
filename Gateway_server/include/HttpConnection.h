#pragma once
#include "const.h"

class HttpConnection :public std::enable_shared_from_this<HttpConnection>
{
	friend class LogicSystem;
public:
	HttpConnection(net::io_context& ioc);
	tcp::socket& GetSocket();
	void run();
private:
	tcp::socket socket_;
	void CheckDeadline();
	void WriteResponse();
	void HandleReq();
	void PreParseGetParam();
	beast::flat_buffer buffer_;//beast�ṩ�Ķ�д������
	http::request<http::dynamic_body> req_;//�������
	http::response<http::dynamic_body> res_;//��Ӧ����
	net::steady_timer deadline_;//��ʱ��ʱ��
	std::string _get_url;//�洢get�����url
	std::unordered_map<std::string, std::string> _get_params;//�洢get����Ĳ���
};