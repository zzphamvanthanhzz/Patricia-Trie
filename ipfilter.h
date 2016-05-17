/* 
 * File:   data.h
 * Author: root
 *
 * Created on April 14, 2016, 4:07 PM
 */
#include <vector>
#include <utility>
#include <memory>
#include <bitset>
#include <string>
#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <sstream>
#include <glog/logging.h>


#define LINESIZE 130

inline std::bitset<32> addr2bits(const std::string& addr) {
    unsigned long addr_ul;
    auto err = inet_pton(AF_INET, addr.c_str(), &addr_ul);
    if (err != 1) {
        return 0;
    }
    return std::bitset<32>(ntohl(addr_ul));
}

inline std::string bits2addr(std::bitset<32> ip_) {
    auto result = new char[16];
    auto ip = ip_.to_ulong();
    sprintf(result, "%lu.%lu.%lu.%lu", (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, (ip) & 0xFF);
    return std::string(result);
}

inline std::string addr2hex(std::string& addr) {
    //0xDA640A00 of 218.100.10.0
    std::bitset<32> bits = addr2bits(addr.c_str());
    std::stringstream hexval;
    hexval << std::hex << bits.to_ulong();
    printf("%s\n", hexval.str().c_str());
    return hexval.str();
}

template <class DataType>
struct Node {
    std::shared_ptr<Node> left, right;
    std::bitset<32> addr;
    unsigned int len; //mask
    std::shared_ptr<DataType> data;

    Node(const std::bitset<32> addr_, unsigned int len, std::shared_ptr<DataType> data)
    : addr(addr_ & (std::bitset<32>(-1) << (32 - len))) //and operator with mask
    , len(len)
    , data(data) {
    };

    /**
     * 
     * @param addr_
     * @return false when addr_ is in Node's IP list
     */
    inline bool match(std::bitset<32> addr_) {
        //Returns an unsigned long with the integer value that has the same bits set as the bitset.
        return (addr ^ (addr_ & (std::bitset<32>(-1) << (32 - len)))).to_ulong();
    }

    inline void dump() {
        std::cout << bits2addr(addr) << "/" << len;
        if (data != nullptr) {
            std::cout << " -> " << *data;
        }
        std::cout << std::endl;
        if (left != nullptr) left->dump();
        if (right != nullptr) right->dump();
    }
};

template <class DataType>
class IP2Net {
public:
    IP2Net(): root_(new Node<DataType>(addr2bits("0.0.0.0"), 0, nullptr)){
    }

    IP2Net(const std::shared_ptr<DataType> default_) : root_(new Node<DataType>(addr2bits("0.0.0.0"), 0, nullptr)), default_(default_) {
    };

    IP2Net(const std::vector<std::pair<std::string, std::shared_ptr<DataType>>> data, const std::shared_ptr<DataType> default_)
    : IP2Net(default_) {
        for (auto& item : data) {
            add(item.first, item.second);
        }
    };

    virtual ~IP2Net() {
    };

    std::shared_ptr<DataType> LookUp(const std::string& addr) {
        auto data = longest_common_path(addr2bits(addr))->data;
		LOG(WARNING) << "Data search" << data ;
        if (data == nullptr) {
            return default_;
        }
        return data;
    }

    void add(const std::string& addr, std::shared_ptr<DataType> data) {
        auto idx = std::strchr(addr.c_str(), '/');
        auto addr_ = addr;
        int mask = 0;

        if (idx != nullptr) {
            mask = std::stoi(addr.substr(idx - addr.c_str() + 1)); //mask
            addr_ = addr.substr(0, idx - addr.c_str()); //ip
        }
        //insert only range, not a single ip
        insert(addr2bits(addr_) & (std::bitset<32>(-1) << (32 - mask)), mask, data);
    }

    int loadData(std::string datafile) {
        FILE *fp;
        char line[LINESIZE];
        char addr_mask[18];
        char data[20];
        if ((fp = fopen(datafile.c_str(), "r")) == NULL) {
            LOG(WARNING) << "Data file:" << datafile.c_str() << " not exists";
            exit(1);
        }
        //reset nodecount first
        nodecount = 0;
        while (fgets(line, LINESIZE, fp)) {
            int match = sscanf(line, "%s %s", addr_mask, data);
            if (match == EOF || match < 2) {
               LOG(WARNING) << "Error parse line " << match;
                continue;
            }
            nodecount++;
            this->add(addr_mask, std::make_shared<std::string>(data));
        }
		fclose(fp);
        return nodecount;
    }

    void dump() {
        root_->dump();
    }

private:
    std::shared_ptr<Node<DataType>> root_;
    std::shared_ptr<DataType> default_;
    int nodecount;

    std::shared_ptr<Node<DataType>> longest_common_path(std::bitset<32> addr) {
        auto cur = root_;
        while (true) {
            auto next = cur->addr[cur->len + 1] ? cur->right : cur->left;
            if (next == nullptr || !next->match(addr)) {
                if (next != nullptr) cur = next;
                break;
            }
            cur = next;
        }
        return cur;
    }

    void insert(const std::bitset<32> addr, unsigned int mask, std::shared_ptr<DataType> data) {
        auto cur = longest_common_path(addr);
        //      cur->dump();
        if (cur->len == 32) return;
        //left of a node is where bit is not set.
        //right of a node is where bit is set.
        //auto next = addr[cur->len + 1] ? cur->left : cur->right;
        auto next = addr[cur->len + 1] ? cur->right : cur->left;
        //overwrite exiting node.
        if (next == nullptr) {
            if (data == cur->data && mask < cur->len) {
                cur->len = mask;
                cur->addr = addr;
                return;
            }
            auto n = std::make_shared<Node < DataType >> (addr, mask, data);
            (addr[cur->len + 1]) ? cur->right = n : cur->left = n;
            return;
        }

        // merge here
        if (next->data == data) {
            if (next->len > mask) {
                if (addr[cur->len + 1]) {
                    cur->right->addr = addr;
                    cur->right->len = mask;
                } else {
                    cur->left->addr = addr;
                    cur->left->len = mask;
                }
            }
            return;
        }

        // split here 
        auto n = std::make_shared<Node < DataType >> (addr, mask, data);
        if (addr[cur->len + 1]) {
            n->right = next;
            cur->right = n;
        } else {
            n->left = next;
            cur->left = n;
        }
    }
};


