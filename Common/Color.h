#ifndef COLOR_H
#define COLOR_H

#include <cstdint>
#include <cstddef>
#include <string>


struct Color
{
	typedef uint32_t Argb;
	enum Channels : size_t { Alfa, Red, Green, Blue, _ChannelCount };

	static constexpr size_t channelSize = sizeof(Argb) / _ChannelCount;
	static constexpr size_t channelCharCount = 2 /*1 byte - 2 hex digits*/ * channelSize;
	static constexpr Argb maxChannelValue = (Argb{ 1 } << (channelSize * 8/*bits in one byte*/)) - 1;

	Color(Argb arbg_ = {});
	Color(const std::string& arbg_);
	bool operator ==(const Color& other) const;
	std::string toStringHex() const;
	void setChannel(Channels channel, Argb value);

	uint32_t argb;
};

#endif // COLOR_H
