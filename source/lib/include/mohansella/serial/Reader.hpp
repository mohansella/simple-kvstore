#pragma once

#include <string>

#include <mohansella/lang/Buffer.hpp>
#include <mohansella/serial/ByteSerialType.hpp>

namespace mohansella::serial
{

	class Reader
	{

	public:
		Reader();
		virtual ~Reader();

		bool readNullOpt();
		void readNull();
		std::uint8_t readByte();
		std::int16_t readShort();
		std::int32_t readInteger();
		float readFloat();
		std::int64_t readLong();
		double readDouble();
		void readString(std::string & target);
		void readBytes(mohansella::lang::Buffer<std::uint8_t> & buffer);

		void startArray();
		void endArray();
		void startObject();
		void endObject();

		ByteSerialType peekSyntax();

	protected:
		void readSyntax(ByteSerialType type);
		void readSyntaxAndLen(ByteSerialType type, std::uint8_t * ptr, std::int32_t len);

		virtual void readRaw(std::uint8_t * buffer, std::int32_t len) = 0;
		virtual const std::uint8_t * peekRaw(std::int32_t readLen) = 0;
		virtual void consumeRaw(std::int32_t commitLen) = 0;

	private:
		ByteSerialType readType();

	};

}
