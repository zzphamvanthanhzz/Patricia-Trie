#include "data.h"
#include "unistd.h"

int main(int argc, char *argv[]) {
	//Test bits2addr and addr2bit
//	std::string ip="203.119.72.0";
//	std::bitset<32> bs = addr2bits(ip);
//	printf("%s\n", bits2addr(bs & (std::bitset<32>(-1) >> (32 - 22))).c_str());
//	printf("%s\n", bits2addr(bs).c_str());
	/*Load map*/
	
	std::shared_ptr<std::string> default_(new std::string("UNKNOWN"));

	IP2Net<std::string> *ipmap = new IP2Net<std::string>(default_);
	ipmap->loadData("/home/thanhpv/Workspace/Code/gitrepo/Patricia/data/VNIX2.txt");
//		ipmap->loadData("/home/thanhpv/Workspace/Code/gitrepo/Patricia/data/VNIXTest.txt");
	ipmap->dump();

	/*search single address*/
	//		std::string addr = "171.253.25.45";
	//		std::string addr = "125.212.128.1";
	//			std::string addr = "127.0.0.1";
	//	std::string addr = "103.17.88.5";

	//Failed
		std::string addr = "101.99.0.5"; 
//		std::string addr = "119.15.160.2";
//	std::string addr = argv[1];
	auto data = ipmap->LookUp(addr.c_str());
	printf("%s %s\n", addr.c_str(), data->c_str());


	/*search from file*/
	//		std::string file = argv[1];
	//		ipmap->searchFromFile(file);
	return 0;
}

