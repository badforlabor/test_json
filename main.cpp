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
	struct personList {
		std::vector<person> list;
	};
}
namespace ns {
	void to_json(json& j, const person& p) {
		j = json{ {"name", p.name}, {"address", p.address}, {"age", p.age} };
	}

	void from_json(const json& j, person& p) {
		p.name = j.at("name").get<std::string>();
		p.address = j.at("address").get<std::string>();
		p.age = j.at("age").get<int>();
	}

	void to_json(json& j, const personList& p) {

		//json required_json;
		//for (auto it : p.list)
		//{
		//	json one;
		//	to_json(one, it);
		//	required_json.push_back(one);
		//}

		//j = json{ {"list", required_json} };
		j = json{ {"list", p.list} };
	}

	void from_json(const json& j, personList& p) {
		//j.at("list").get_to<std::vector<person>>(p.list);
		p.list = j.at("list").get<std::vector<person>>();
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

		std::string a = j.dump();
		json j2 = json::parse(a);
		p2 = j2;
		std::cout << p2.address << std::endl;

		
#if 0
		// 错误的，应该使用json::parse
		p2 = json(a);
		std::cout << p2.address << std::endl;
#endif
	}

	// 数组+嵌套其他
	{
		ns::person p{ "Ned Flanders", "744 Evergreen Terrace", 60 };

		ns::personList pl;
		pl.list.push_back(p);

		// 隐式转换: person -> json
		json j = pl;

		std::string a = j.dump();
		json j2 = json::parse(a);
		
		ns::personList p2 = j2;

		std::cout << p2.list.size() << std::endl;
	}

	extern int main1();
	main1();

	system("pause");

	return 0;
}