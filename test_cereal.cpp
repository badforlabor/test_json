#include <iostream>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/polymorphic.hpp>
#include "my_types.h"
#include <fstream>
using namespace cereal;

// 自定义的
#if 0
template<class Archive, class T>
void save(Archive& archive, const MyVector<T>& o)
{
	typename std::vector<T>::size_type size = o.data.size();
	archive(size);

	for (typename std::vector<T>::size_type i = 0; i < size; i++)
	{
		archive(o.data[i]);
	}
}

template<class Archive, class T>
void load(Archive & archive, MyVector<T>& o)
{
	typename std::vector<T>::size_type size = o.data.size();
	archive(size);
	o.data.resize(size);

	for (typename std::vector<T>::size_type i = 0; i < size; i++)
	{
		archive(o.data[i]);
	}
}

template<class Archive>
void save(Archive& archive, const MyVector<float>& o)
{
	typename std::vector<float>::size_type size = o.data.size();
	archive(size);

	for (typename std::vector<float>::size_type i = 0; i < size; i++)
	{
		archive(o.data[i]);
	}
}

template<class Archive>
void load(Archive & archive, MyVector<float>& o)
{
	typename std::vector<float>::size_type size = o.data.size();
	archive(size);
	o.data.resize(size);

	for (typename std::vector<float>::size_type i = 0; i < size; i++)
	{
		archive(o.data[i]);
	}
}
#else

// 如果是二进制序列化，并且是基础数据类型，那么，就用binary进行序列化

  //! Serialization for std::vectors of arithmetic (but not bool) using binary serialization, if supported
template <class Archive, class T>
typename std::enable_if<traits::is_output_serializable<BinaryData<T>, Archive>::value
	&& std::is_arithmetic<T>::value, void>::type
	save(Archive & ar, const MyVector<T>& vector)
{
	ar(make_size_tag(static_cast<size_type>(vector.data.size()))); // number of elements
	ar(binary_data(vector.data.data(), vector.data.size() * sizeof(T)));
}

//! Serialization for std::vectors of arithmetic (but not bool) using binary serialization, if supported
template <class Archive, class T>
typename std::enable_if<traits::is_input_serializable<BinaryData<T>, Archive>::value
	&& std::is_arithmetic<T>::value, void>::type
	load(Archive & ar, MyVector<T> & vector)
{
	size_type vectorSize;
	ar(make_size_tag(vectorSize));

	vector.data.resize(static_cast<std::size_t>(vectorSize));
	ar(binary_data(vector.data.data(), static_cast<std::size_t>(vectorSize) * sizeof(T)));
}

//! Serialization for non-arithmetic vector types
template <class Archive, class T>
typename std::enable_if<!(traits::is_output_serializable<BinaryData<T>, Archive>::value
	&& std::is_arithmetic<T>::value), void>::type
	save(Archive& ar, const MyVector<T>& vector)
{
	// json类型的序列化，处理数组时， SizeTag会自动转化成"[]"
	ar(make_size_tag(static_cast<size_type>(vector.data.size()))); // number of elements

	for (auto && v : vector.data)
		ar(v);
}

//! Serialization for non-arithmetic vector types
template <class Archive, class T>
typename std::enable_if <!(traits::is_input_serializable<BinaryData<T>, Archive>::value
	&& std::is_arithmetic<T>::value), void>::type
	load(Archive & ar, MyVector<T> & vector)
{
	size_type size;
	ar(make_size_tag(size));

	vector.data.resize(static_cast<std::size_t>(size));
	for (auto && v : vector.data)
		ar(v);
}
#endif

template<class Archive>
void serialize(Archive & archive,
	MyVectorData & m)
{
	archive(cereal::make_nvp("list", m.list));
	archive(cereal::make_nvp("list2", m.list2));
}

template<class Archive>
void serialize(Archive & archive,
	MyStruct & m)
{
	archive(cereal::make_nvp("i", m.i), 
		cereal::make_nvp("e", m.e),
		cereal::make_nvp("fs", m.fs),
		cereal::make_nvp("str", m.str));
}


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

	MyVectorData c;
	c.list.data.push_back(a);
	c.list.data.push_back(a);
	c.list.data.push_back(a);
	c.list2.data.push_back(1.23);
	c.list2.data.push_back(4.56);

	//if(false)
	{
		{
			std::ofstream file("abc.bin", std::ios::out | std::ios::binary | std::ios::ate);
			cereal::BinaryOutputArchive archive(file);
			//archive << b;
			serialize(archive, c);
		}
		{
			std::ifstream file("abc.bin", std::ios::in | std::ios::binary);
			cereal::BinaryInputArchive archive(file);
			//archive << b;
			MyVectorData c2;
			serialize(archive, c2);
			assert(c2.list.data.size() == c.list.data.size());
			assert(c2.list2.data.size() == c.list2.data.size());
		}
	}
	{
		std::stringstream str;
		{
			cereal::JSONOutputArchive archive(str);
			//archive << b;
			serialize(archive, c);
		}
		std::cout << str.str() << std::endl;

		{
			cereal::JSONInputArchive archive(str);
			//archive << b;
			MyVectorData c2;
			serialize(archive, c2);
			assert(c2.list.data.size() == c.list.data.size());
			assert(c2.list2.data.size() == c.list2.data.size());
		}
	}

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