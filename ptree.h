#pragma once

#include <string>

typedef struct node {
    node*           left,right;
    DataType*       data;
    int             lenght;
    unsigned long   mark;

    void distroy() {
        if (left != nullptr) { left.distroy(); }
        if (right != nullptr) { right.distroy(); }
        delete data;
    }
} Node;

inline int equal(unsigned long* prefix, Node* node) {

}
<template DataType>
class Ptree
{
public:
    Ptree () {};
    Ptree (const std::vector<std::string, DataType> map){
    };
    virtual ~Ptree () {
        root_.distroy();
    };


    void Add(const std::string, DateType data ) {
        return;
    }

    DataType* LookUp(const std::string& ip) {
        return nullptr;
    }



private:
    Node* root_;
    Node* find(prefix, int& idx) {
        Node* cur = root_;
        while true {

        }
    };
};
