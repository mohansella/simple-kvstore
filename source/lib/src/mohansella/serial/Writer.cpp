#include <mohansella/serial/Writer.hpp>


#include <mohansella/serial/ByteSerialType.hpp>

namespace mohansella::serial
{

	Writer::Writer()
	{

	}

	Writer::~Writer()
	{

	}

	Writer & Writer::putNull()
	{
		std::uint8_t nullEnVal = ByteSerialType::BSTYPE_NULL;
		return this->putRaw(&nullEnVal, sizeof(nullEnVal));
	}

	Writer & Writer::putByte(std::uint8_t val)
	{
		this->writeSyntaxAndLen(ByteSerialType::BSTYPE_BYTE, &val, sizeof(val));
		return *this;
	}

	Writer & Writer::putShort(std::int16_t val)
	{
		this->writeSyntaxAndLen(ByteSerialType::BSTYPE_SHORT, (std::uint8_t *) &val, sizeof(val));
		return *this;
	}

	Writer & Writer::putInteger(std::int32_t val)
	{
		this->writeSyntaxAndLen(ByteSerialType::BSTYPE_INTEGER, (std::uint8_t *) &val, sizeof(val));
		return *this;
	}

	Writer & Writer::putFloat(float val)
	{
		this->writeSyntaxAndLen(ByteSerialType::BSTYPE_FLOAT, (std::uint8_t *) &val, sizeof(val));
		return *this;
	}

	Writer & Writer::putLong(std::int64_t val)
	{
		this->writeSyntaxAndLen(ByteSerialType::BSTYPE_LONG, (std::uint8_t *) &val, sizeof(val));
		return *this;
	}

	Writer & Writer::putDouble(double val)
	{
		this->writeSyntaxAndLen(ByteSerialType::BSTYPE_DOUBLE, (std::uint8_t *) &val, sizeof(val));
		return *this;
	}

	Writer & Writer::putString(const std::string & val)
	{
		return this->putCharArray(val.c_str(), val.length());
	}
	
	Writer & Writer::putCharArray(const char * dataPtr, std::int32_t len)
	{
		this->writeSyntaxAndLen(ByteSerialType::BSTYPE_STRING, (std::uint8_t *) &len, sizeof(len));
		return this->putRaw((const std::uint8_t *) dataPtr, len);
	}

	Writer & Writer::putBytes(const std::uint8_t * dataPtr, std::int32_t len)
	{
		this->writeSyntaxAndLen(ByteSerialType::BSTYPE_BLOB, (std::uint8_t *) &len, sizeof(len));
		return this->putRaw(dataPtr, len);
	}

	Writer & Writer::startObject()
	{
		this->writeSyntax(ByteSerialType::BSTYPE_OBJECT_START);
		return *this;
	}

	Writer & Writer::endObject()
	{
		this->writeSyntax(ByteSerialType::BSTYPE_OBJECT_END);
		return *this;
	}

	Writer & Writer::startArray()
	{
		this->writeSyntax(ByteSerialType::BSTYPE_ARRAY_START);
		return *this;
	}

	Writer & Writer::endArray()
	{
		this->writeSyntax(ByteSerialType::BSTYPE_ARRAY_END);
		return *this;
	}

	void Writer::writeSyntax(ByteSerialType type)
	{
		std::uint8_t data = type;
		this->putRaw(&data, sizeof(data));
	}

	void Writer::writeSyntaxAndLen(ByteSerialType type, const std::uint8_t * ptr, std::int32_t len)
	{
		this->writeSyntax(type);
		this->putRawInReverse(ptr, len);
	}
	
}
