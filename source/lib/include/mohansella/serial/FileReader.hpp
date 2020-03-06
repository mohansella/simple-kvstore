#pragma once

#include <memory>

#include <mohansella/serial/Reader.hpp>

namespace mohansella::serial
{

	class FileReader : virtual public Reader
	{

	public:
		FileReader(const std::string & filePath);
		virtual ~FileReader();

		virtual void readRaw(std::uint8_t * buffer, std::int32_t len) override;
		virtual void close();

	protected:
		virtual const std::uint8_t * peekRaw(std::int32_t readLen) override;
		virtual void consumeRaw(std::int32_t commitLen) override;

	private:
		struct Data;
		std::unique_ptr<Data> data;

	};

}
