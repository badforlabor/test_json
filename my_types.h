#pragma once

#include <vector>
#include <string>
#include "cereal/cereal.hpp"

enum class MyEnum :uint16_t { V1, V2, V3 };

struct MyStruct {
	uint32_t i;
	MyEnum e;
	std::vector<float> fs;
	std::string str;
};

struct MyStructList {
	std::vector<MyStruct> list;
};


// 自定义数组
template<class T>
class MyVector
{
public:
	MyVector() {}

	std::vector<T> data;
	
};

struct MyVectorData
{
	MyVector<MyStruct> list;
	MyVector<float> list2;
};