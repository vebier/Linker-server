#include "CServer.h"
#include "SectionInfo.h"
#include "RedisMgr.h"

void TestRedisMgr() {
    assert(RedisMgr::GetInstance()->Set("blogwebsite", "llfc.club"));
    std::string value = "";
    assert(RedisMgr::GetInstance()->Get("blogwebsite", value));
    assert(RedisMgr::GetInstance()->Get("nonekey", value) == false);
    assert(RedisMgr::GetInstance()->HSet("bloginfo", "blogwebsite", "llfc.club"));
    assert(RedisMgr::GetInstance()->HGet("bloginfo", "blogwebsite") != "");
    assert(RedisMgr::GetInstance()->ExistsKey("bloginfo"));
    assert(RedisMgr::GetInstance()->Del("bloginfo"));
    assert(RedisMgr::GetInstance()->Del("bloginfo"));
    assert(RedisMgr::GetInstance()->ExistsKey("bloginfo") == false);
    assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue1"));
    assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue2"));
    assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue3"));
    assert(RedisMgr::GetInstance()->RPop("lpushkey1", value));
    assert(RedisMgr::GetInstance()->RPop("lpushkey1", value));
    assert(RedisMgr::GetInstance()->LPop("lpushkey1", value));
    assert(RedisMgr::GetInstance()->LPop("lpushkey2", value) == false);
    RedisMgr::GetInstance()->Close();
}

int main()
{
    //TestRedisMgr();
	try {
		
		std::string gate_port_str = ConfigMgr::GetInstance()["GateServer"]["Port"];
		unsigned short gate_port = atoi(gate_port_str.c_str());
		net::io_context ioc;
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
			if (error) {
				return 0;
			}
			ioc.stop();
		});
		std::make_shared<CServer>(ioc, gate_port)->run();
		ioc.run();
	}
	catch (const std::exception& ec) {
		std::cerr << "CServer Error : " << ec.what() << std::endl;
		return -1;
	}
	return 0;
}

