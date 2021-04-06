#include "Color.h"

#include <sstream>
#include <iomanip>

Color::Color(Argb arbg_) : argb(arbg_) {}

Color::Color(const std::string& arbg_) : argb{ 0 }
{
	if ((arbg_.size() == 1 /*#*/ + _ChannelCount * channelCharCount)
		&& (arbg_.front() == '#'))
	{
		size_t start{ 1 /*# - is first*/ };
		char* end{ nullptr };
		for (size_t channelIndex = 0; channelIndex < _ChannelCount; ++channelIndex, start += channelCharCount)
			argb += std::strtol(arbg_.substr(start, channelCharCount).data(), &end, 16/*hex*/)
				<< (_ChannelCount - channelIndex - 1) * channelSize * 8 /*bits in one byte*/;
	}
}

bool Color::operator==(const Color& other) const
{
	return argb == other.argb;
}

std::string Color::toStringHex() const
{
	std::ostringstream stringHex{};

	stringHex
		<< '#'
		<< std::setfill('0')
		<< std::setw(channelCharCount * _ChannelCount)
		<< std::hex
		<< std::uppercase
		<< argb;

	return stringHex.str();
}

void Color::setChannel(Channels channel, Argb value)
{
	auto bitCount = (_ChannelCount - channel - 1) * channelSize * 8/*bits in one byte*/;
	value &= maxChannelValue;
	argb = (argb & ~(maxChannelValue << bitCount)) | (value << bitCount);
}
