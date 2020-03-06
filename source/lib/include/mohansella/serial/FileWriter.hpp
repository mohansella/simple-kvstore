#pragma once

#include <memory>

#include <mohansella/serial/Writer.hpp>

namespace mohansella::serial
{

	class FileWriter : virtual public Writer
	{

	public:
		FileWriter(const std::string & filePath);
		virtual ~FileWriter();

		virtual Writer & putRaw(const std::uint8_t * data, std::int32_t len) override;
		virtual Writer & putRawInReverse(const std::uint8_t * data, std::int32_t len) override;

		virtual void close();
		virtual void flush();

	private:
		struct Data;
		std::unique_ptr<Data> data;

	};

}
