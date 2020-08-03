#include <vector>
#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>

enum class MyEnum :uint16_t { V1, V2, V3 };
struct MyStruct {
	uint32_t i;
	MyEnum e;
	std::vector<float> fs;
};

template <typename S>
void serialize(S& s, MyStruct& o) {
	s.value4b(o.i);
	s.value2b(o.e);
	int size = o.fs.size();
	s.value4b(size);
	s.container4b(o.fs, size);
}

using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

int main2()
{

}

int main1() {
	MyStruct data{ 8941, MyEnum::V2, {15.0f, -8.5f, 0.045f} };
	MyStruct res{};

	Buffer buffer;

	auto writtenSize = bitsery::quickSerialization<OutputAdapter>(buffer, data);
	auto state = bitsery::quickDeserialization<InputAdapter>({ buffer.begin(), writtenSize }, res);

	assert(state.first == bitsery::ReaderError::NoError && state.second);
	assert(data.fs == res.fs && data.i == res.i && data.e == res.e);

	return 0;
}