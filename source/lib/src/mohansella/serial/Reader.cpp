#include <mohansella/serial/Reader.hpp>


#include <exception>
#include <iostream>

namespace mohansella::serial
{

	using namespace mohansella::lang;

	Reader::Reader()
	{

	}

	Reader::~Reader()
	{

	}

	bool Reader::readNullOpt()
	{
		auto bytes = this->peekRaw(1);
		auto toReturn = bytes[0];
		auto serialType = (ByteSerialType) toReturn;
		if(serialType == ByteSerialType::BSTYPE_NULL)
		{
			this->consumeRaw(1);
			return true;
		}
		return false;
	}

	void Reader::readNull()
	{
		this->readSyntax(ByteSerialType::BSTYPE_NULL);
	}

	std::uint8_t Reader::readByte()
	{
		std::uint8_t toReturn;
		this->readSyntaxAndLen(ByteSerialType::BSTYPE_BYTE, (std::uint8_t *) &toReturn, sizeof(toReturn));
		return toReturn;
	}

	std::int16_t Reader::readShort()
	{
		std::int16_t toReturn;
		this->readSyntaxAndLen(ByteSerialType::BSTYPE_SHORT, (std::uint8_t *) &toReturn, sizeof(toReturn));
		return toReturn;
	}

	std::int32_t Reader::readInteger()
	{
		std::int32_t toReturn;
		this->readSyntaxAndLen(ByteSerialType::BSTYPE_INTEGER, (std::uint8_t *) &toReturn, sizeof(toReturn));
		return toReturn;
	}

	float Reader::readFloat()
	{
		float toReturn;
		this->readSyntaxAndLen(ByteSerialType::BSTYPE_FLOAT, (std::uint8_t *) &toReturn, sizeof(toReturn));
		return toReturn;
	}

	std::int64_t Reader::readLong()
	{
		std::int64_t toReturn;
		this->readSyntaxAndLen(ByteSerialType::BSTYPE_LONG, (std::uint8_t *) &toReturn, sizeof(toReturn));
		return toReturn;
	}

	double Reader::readDouble()
	{
		double toReturn;
		this->readSyntaxAndLen(ByteSerialType::BSTYPE_DOUBLE, (std::uint8_t *) &toReturn, sizeof(toReturn));
		return toReturn;
	}

	void Reader::readString(std::string & target)
	{
		std::int32_t stringLen;
		this->readSyntaxAndLen(ByteSerialType::BSTYPE_STRING, (std::uint8_t *) &stringLen, sizeof(stringLen));

		auto bytes = this->peekRaw(stringLen);
		//target.reset().append((const std::char8_t *) bytes, stringLen);
		this->consumeRaw(stringLen);
	}

	void Reader::readBytes(Buffer<std::uint8_t> & buffer)
	{
		std::int32_t bytesLen;
		this->readSyntaxAndLen(ByteSerialType::BSTYPE_BLOB, (std::uint8_t *) &bytesLen, sizeof(bytesLen));

		auto bytes = this->peekRaw(bytesLen);
		if(buffer.data.unitsCount < bytesLen)
		{
			buffer.reset(bytesLen);
		}
		std::memcpy(buffer.data.unitsPtr, bytes, bytesLen);
		buffer.validLength = bytesLen;
		this->consumeRaw(bytesLen);
	}

	void Reader::startArray()
	{
		this->readSyntax(ByteSerialType::BSTYPE_ARRAY_START);
	}

	void Reader::endArray()
	{
		this->readSyntax(ByteSerialType::BSTYPE_ARRAY_END);
	}

	void Reader::startObject()
	{
		this->readSyntax(ByteSerialType::BSTYPE_OBJECT_START);
	}

	void Reader::endObject()
	{
		this->readSyntax(ByteSerialType::BSTYPE_OBJECT_END);
	}
	
	ByteSerialType Reader::peekSyntax()
	{
		auto bytes = this->peekRaw(1);
		return (ByteSerialType) bytes[0];
	}

	void Reader::readSyntaxAndLen(ByteSerialType type, std::uint8_t * ptr, std::int32_t len)
	{
		this->readSyntax(type);
		auto bytes = this->peekRaw(len);
		for(auto i = 0, j = len - 1; j >= 0; i++, j--)
		{
			ptr[i] = bytes[j];
		}
		this->consumeRaw(len);
	}

	void Reader::readSyntax(ByteSerialType type)
	{
		auto serialType = this->readType();
		if(serialType != type)
		{
            std::cout << "Reader: unexpected serial type:" << serialType << " excepected:" << type << std::endl;
			throw std::exception("Reader: unexpected serial type. check logs");
		}
	}

	ByteSerialType Reader::readType()
	{
		auto byte = this->peekRaw(1);
		auto toReturn = byte[0];
		this->consumeRaw(1);
		return (ByteSerialType) toReturn;
	}

}
