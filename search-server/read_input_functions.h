#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "paginator.h"
#include "document.h"


std::string ReadLine();

int ReadLineWithNumber();

template<typename T>
std::ostream& operator<<(std::ostream& out, std::vector<T> vec){
    for (T elem : vec){
        out<<elem;
    }
    return out;
}


template<typename T>
std::vector<std::vector<T>>Paginate(std::vector<T> list, int page_size){
    Paginator exem(list.begin(), list.end(), page_size);
    return exem.ReturnList();
}


