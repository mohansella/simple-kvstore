#pragma once

#include <string>

#include <mohansella/serial/ByteSerialType.hpp>

namespace mohansella::serial
{

	class Writer
	{

	public:
		Writer();
		virtual ~Writer();

		Writer & startObject();
		Writer & endObject();
		Writer & startArray();
		Writer & endArray();

		Writer & putNull();
		Writer & putByte(std::uint8_t val);
		Writer & putShort(std::int16_t val);
		Writer & putInteger(std::int32_t val);
		Writer & putFloat(float val);
		Writer & putLong(std::int64_t val);
		Writer & putDouble(double val);
		Writer & putString(const std::string & val);
		Writer & putCharArray(const char * val, std::int32_t len);
		Writer & putBytes(const std::uint8_t * data, std::int32_t len);

		virtual Writer & putRaw(const std::uint8_t * data, std::int32_t len) = 0;
		virtual Writer & putRawInReverse(const std::uint8_t * data, std::int32_t len) = 0; //to handle endianess

	protected:
		void writeSyntax(ByteSerialType type);
		void writeSyntaxAndLen(ByteSerialType type, const std::uint8_t * ptr, std::int32_t len);

	private:

	};

}
