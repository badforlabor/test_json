#include <errno.h>
#include "nlohmann/json.hpp"
using nlohmann::json;

#include <windows.h>
#include <iostream>

struct A1
{
	int a;
	float b;
	std::string c;
};

namespace ns {
	struct person {
		std::string name;
		std::string address;
		int age;
	};
}
namespace ns {
	void to_json(json& j, const person& p) {
		j = json{ {"name", p.name}, {"address", p.address}, {"age", p.age} };
	}

	void from_json(const json& j, person& p) {
		p.name = j.at("name").get<std::string>();
		p.name = j.at("address").get<std::string>();
		p.name = j.at("age").get<int>();
	}
}

int main()
{
	{
		auto j3 = json::parse("{ \"happy\": true, \"pi\": 3.141 }");

		std::string s = j3.dump();    // {\"happy\":true,\"pi\":3.141}
		std::cout << s << std::endl;

		std::cout << std::endl;
		std::cout << j3.dump(4) << std::endl;
	}


	{
		ns::person p = { "Ned Flanders", "744 Evergreen Terrace", 60 };

		// 转换为JSON：将每个值复制到JSON对象中
		json j;
		j["name"] = p.name;
		j["address"] = p.address;
		j["age"] = p.age;

		// ...

		// 从JSON转换：从JSON对象复制每个值
		ns::person p2 {
			j["name"].get<std::string>(),
			j["address"].get<std::string>(),
			j["age"].get<int>()
		};

		std::cout << j.dump(4) << std::endl;

		ns::person p3{
			j["name"].get<std::string>(),
			j["address"].get<std::string>(),
			j["age"].get<int>()
		};
		std::cout << (j.contains("name") ? j["name"] : "") << std::endl;
		std::cout << (j.contains("name1") ? j["name1"] : "") << std::endl;
	}

	{
		// 创建一个person类并初始化
		ns::person p{ "Ned Flanders", "744 Evergreen Terrace", 60 };

		// 隐式转换: person -> json
		json j = p;

		std::cout << j << std::endl;
		// {"address":"744 Evergreen Terrace","age":60,"name":"Ned Flanders"}

		// 隐式转换: json -> person
		ns::person p2 = j;

		std::cout << p2.address << std::endl;
	}

	system("pause");

	return 0;
}