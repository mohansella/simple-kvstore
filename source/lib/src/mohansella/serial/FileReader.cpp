#include <mohansella/serial/FileReader.hpp>


#include <fstream>

#include <boost/beast/core/flat_buffer.hpp>

namespace mohansella::serial
{

	using namespace mohansella::lang;

	struct FileReader::Data
	{
		std::ifstream inStream;
		Array<std::uint8_t> fileBuf;
		boost::beast::flat_buffer buffer;
	};

	FileReader::FileReader(const std::string & filePath)
	{
		this->data = std::make_unique<Data>();
		this->data->inStream.exceptions(std::fstream::failbit | std::fstream::badbit);
		this->data->inStream.open(filePath, std::fstream::in | std::fstream::binary);
		
		data->fileBuf.reset(16 * 1024);
		data->inStream.rdbuf()->pubsetbuf((char *) data->fileBuf.unitsPtr, data->fileBuf.unitsCount);
	}

	FileReader::~FileReader()
	{

	}

	void FileReader::close()
	{
		if(data->inStream)
		{
			data->inStream.close();
		}
	}

	void FileReader::readRaw(std::uint8_t * buffer, std::int32_t len)
	{
		auto bytes = this->peekRaw(len);
		std::memcpy(buffer, bytes, len);
		this->consumeRaw(len);
	}

	const std::uint8_t * FileReader::peekRaw(std::int32_t rawLen)
	{
		auto & buff = data->buffer;
		auto & inStream = data->inStream;
		if(buff.size() < (std::size_t) rawLen)
		{
			auto expandLen = rawLen - buff.size();
			auto prepBuf = buff.prepare(expandLen);
			inStream.read((char *) prepBuf.data(), prepBuf.size());
			auto readSize = (std::size_t) inStream.gcount();
			buff.commit(readSize);
		}
		return (std::uint8_t *) buff.data().data();
	}

	void FileReader::consumeRaw(std::int32_t commitLen)
	{
		data->buffer.consume(commitLen);
	}
	
}
