#include <iostream>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>
#include "my_types.h"
#include <fstream>

template<class Archive>
void serialize(Archive & archive,
	MyStruct & m)
{
	archive(cereal::make_nvp("i", m.i), 
		cereal::make_nvp("e", m.e),
		cereal::make_nvp("fs", m.fs),
		cereal::make_nvp("str", m.str));
}
//CEREAL_REGISTER_TYPE(MyStruct);


//CEREAL_REGISTER_TYPE(MyStructList);
template<class Archive>
void serialize(Archive & archive,
	MyStructList & m)
{
	archive(cereal::make_nvp("list", m.list));
}

template<class T>
T& LastOne(std::vector<T>& arr)
{
	assert(arr.size() > 0);

	return arr[arr.size() - 1];
}

template<class T>
T& RandOne(std::vector<T>& arr)
{
	assert(arr.size() > 0);

	return arr[rand() % arr.size()];
}

int main4()
{
	if(0)
	{
		cereal::XMLOutputArchive archive(std::cout);
		bool arr[] = { true, false };
		std::vector<int> vec = { 1, 2, 3, 4, 5 };
		archive(CEREAL_NVP(vec),
			arr);
	}

	MyStruct a;
	a.i = 2020;
	a.e = MyEnum::V1;
	a.fs.push_back(8);
	a.fs.push_back(6);
	a.str = "10:14";

	MyStructList b;
	b.list.push_back(a);
	b.list.push_back(a);
	b.list.push_back(a);

	if (false)
	{
		cereal::XMLOutputArchive archive(std::cout);
		//archive << cereal::make_nvp("MyStructList", b);
		serialize(archive, b);
	}

	if (false)
	{
		cereal::JSONOutputArchive archive(std::cout);
		//archive << b;
		serialize(archive, b);
	}

	{
		{
			std::ofstream file("abc.bin");
			cereal::BinaryOutputArchive archive(file);
			//archive << b;
			serialize(archive, b);
		}
		{
			std::ifstream file("abc.bin");
			cereal::BinaryInputArchive archive(file);
			//archive << b;
			MyStructList b2;
			serialize(archive, b2);	
			assert(b2.list.size() == b.list.size());
		}
	}

	{
		// 测试大量数据
		MyStructList list;
		MyStruct data{ 8941, MyEnum::V2, {15.0f, -8.5f, 0.045f} };
		data.str = "def";

		for (int j = 0; j < 50000 * 3; j++)
		{
			data.fs.push_back(rand() % 100);
		}

		list.list.reserve(3000);
		list.list.resize(2000);
		for (int i = 0; i < list.list.size(); i++)
		{
			list.list[i] = data;
			list.list[i].i = i;
			list.list[i].e = (MyEnum)(i % 3);
			//list.list[i].fs.reserve(data.fs.size());
			//list.list[i].fs.resize(data.fs.size());
			//list.list.push_back(data);
		}


		{
			std::ofstream file("abc.bin", std::ios::out | std::ios::binary | std::ios::ate);
			cereal::BinaryOutputArchive archive(file);
			//archive << b;
			serialize(archive, list);
		}
		{
			std::ifstream file("abc.bin", std::ios::in | std::ios::binary);
			cereal::BinaryInputArchive archive(file);
			//archive << b;
			MyStructList b2;
			serialize(archive, b2);
			assert(b2.list.size() == list.list.size());

			assert(LastOne(LastOne(b2.list).fs) == LastOne(LastOne(list.list).fs));

			// 随便找10个，如果对了，就对了
			for (int i = 0; i < 1000; i++)
			{
				int id1 = rand() % b2.list.size();
				int id2 = rand() % (b2.list[id1].fs.size());
				assert(b2.list[id1].fs[id2] == list.list[id1].fs[id2]);
			}
		}

	}


	return 0;
}