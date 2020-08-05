#include <vector>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <bitsery/traits/string.h>
#include <iostream>
#include <fstream>


template<typename A>
struct ArrayTraits {
	template <typename S>
	static void Body(S& s, A& o, int size)
	{
		// s.container1b(o, size);
		throw std::exception("undefine");
	}
};

// 序列化一个动态数组，譬如ArraySerialize(s, std::string);
template <typename S, typename A>
void ArraySerialize(S& s, A& o)
{
	int size = o.size();
	s.value4b(size);

	ArrayTraits<A>::Body(s, o, size);
}

template<>
struct ArrayTraits<std::string> {
	template <typename S>
	static void Body(S& s, std::string& o, int size)
	{
		s.container1b(o, size);
	}
};

template<>
struct ArrayTraits<std::vector<float>> {
	template <typename S>
	static void Body(S& s, std::vector<float>& o, int size)
	{
		s.container4b(o, size);
	}
};

enum class MyEnum :uint16_t { V1, V2, V3 };
struct MyStruct {
	uint32_t i;
	MyEnum e;
	std::vector<float> fs;
	std::string str;
};
template <typename S>
void serialize(S& s, MyStruct& o) {
	s.value4b(o.i);
	s.value2b(o.e);

	//int size = o.fs.size();
	//s.value4b(size);
	//s.container4b(o.fs, size);
	ArraySerialize(s, o.fs);

	ArraySerialize(s, o.str);
}
struct MyStructList {
	std::vector<MyStruct> list;
};
template <typename S>
void serialize(S& s, MyStructList& o) {
	s.container(o.list, 10000);
}
using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

int main2()
{
	// 必须是用64位调试

	MyStructList list;
	MyStruct data{ 8941, MyEnum::V2, {15.0f, -8.5f, 0.045f} };
	data.str = "def";


	for (int j = 0; j < 50000 * 3; j++)
	{
		data.fs.push_back(rand() % 100);
	}

	list.list.reserve(3000);
	list.list.resize(1000);
	for (int i = 0; i < list.list.size(); i++)
	{
		list.list[i] = data;
		list.list[i].i = i;
		list.list[i].e = (MyEnum)(i % 3);
		//list.list[i].fs.reserve(data.fs.size());
		//list.list[i].fs.resize(data.fs.size());
		//list.list.push_back(data);
	}

	MyStructList res{};
	Buffer buffer;
	buffer.reserve(1000 * 1000 * 1000);
	auto writtenSize = bitsery::quickSerialization<OutputAdapter>(buffer, list);

	// 写文件
	using namespace std;
	const char* filename = "abc.bin";
	std::ofstream output;
	output.open(filename, ios::out | ios::ate | ios::binary);
	output.write((char*)buffer.data(), writtenSize);
	output.close();

	std::ifstream input;
	input.open(filename, ios::in | ios::binary);
	{
		auto& t = input;
		t.seekg(0, std::ios::end);
		size_t count = t.tellg();
		char* c = new char[count];
		t.seekg(0, std::ios::beg);
		t.read(c, count); 
		assert(writtenSize == count);

		buffer.resize(count, 0);
		memcpy(buffer.data(), c, count);
		delete[] c;
	}
	input.close();

	auto state = bitsery::quickDeserialization<InputAdapter>({ buffer.begin(), writtenSize }, res);

	assert(state.first == bitsery::ReaderError::NoError && state.second);
	int idx = list.list.size() - 1;
	assert(list.list.size() == res.list.size() && list.list[idx].i == res.list[idx].i && list.list[idx].e == res.list[idx].e);

	return 0;
}

int main1() {
	main2();

	MyStruct data{ 8941, MyEnum::V2, {15.0f, -8.5f, 0.045f} };
	MyStruct res{};

	Buffer buffer;

	auto writtenSize = bitsery::quickSerialization<OutputAdapter>(buffer, data);
	auto state = bitsery::quickDeserialization<InputAdapter>({ buffer.begin(), writtenSize }, res);

	assert(state.first == bitsery::ReaderError::NoError && state.second);
	assert(data.fs == res.fs && data.i == res.i && data.e == res.e);

	return 0;
}