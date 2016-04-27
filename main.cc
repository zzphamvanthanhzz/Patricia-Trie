#include "data.h"
#include "unistd.h"

int main(int argc, char *argv[]) {
	const std::string default_data = "UNKNOWN";
	std::string addr = "171.253.25.45";
//	std::string addr = "125.212.128.1";
//		std::string addr = "127.0.0.1";
	IP2Net<std::string> ipmap;
	ipmap.loadData("VNIX2.txt");
	//	ipmap.dump();
	ipmap.LookUp(addr.c_str());
	return 0;
}

