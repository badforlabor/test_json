#pragma once

#include <vector>
#include <string>

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
