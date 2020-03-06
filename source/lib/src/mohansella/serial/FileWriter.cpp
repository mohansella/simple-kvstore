#include <mohansella/serial/FileWriter.hpp>

#include <fstream>

#include <mohansella/lang/Buffer.hpp>

namespace mohansella::serial
{

	using namespace mohansella::lang;

	struct FileWriter::Data
	{
		Array<std::uint8_t> fileBuf;
		std::ofstream outStream;
	};

	FileWriter::FileWriter(const std::string & filePath)
	{
		data = std::make_unique<Data>();
		data->outStream.exceptions(std::fstream::failbit | std::fstream::badbit);
		data->outStream.open(filePath, std::fstream::out | std::fstream::binary | std::fstream::trunc);

		data->fileBuf.reset(16 * 1024);
		data->outStream.rdbuf()->pubsetbuf((char *) data->fileBuf.unitsPtr, data->fileBuf.unitsCount);
	}

	FileWriter::~FileWriter()
	{
		
	}
	
	Writer & FileWriter::putRaw(const std::uint8_t * data, std::int32_t len)
	{
		auto & outStream = this->data->outStream;
		outStream.write((const char *) data, len);
		return *this;
	}

	Writer & FileWriter::putRawInReverse(const std::uint8_t * sourcePtr, std::int32_t len)
	{
		auto & outStream = this->data->outStream;
		for(auto sourceEndPtr = sourcePtr + len - 1; sourcePtr <= sourceEndPtr; sourceEndPtr--)
		{
			outStream.write((const char *) sourceEndPtr, 1);
		}
		return *this;
	}

	void FileWriter::flush()
	{
		this->data->outStream.flush();
	}
	
	void FileWriter::close()
	{
		if(this->data->outStream)
		{
			this->data->outStream.close();
		}
	}
	
}
