#pragma once

#include <memory>

#include <mohansella/lang/Array.hpp>

namespace mohansella::lang
{

    template<typename T>
    struct Buffer
    {

    public:
        Buffer() = default;
        Buffer(std::int32_t initSize);
        Buffer(Buffer<T> && other); //move constructor
        ~Buffer() = default;

		Buffer<T>& operator=(Buffer<T> && other); //move assignment

		void reset();
		void reset(std::int32_t unitsCount);

		Array<T> data;
        std::int32_t validLength = 0;

	private:
		Buffer(const Buffer &) = delete;
		Buffer& operator=(const Buffer & string) = delete;

    };

}

//inliners
namespace mohansella::lang
{
	template<typename T>
	inline Buffer<T>::Buffer(std::int32_t initSize)
	{
		this->data.reset(initSize);
	}

	template<typename T>
	inline Buffer<T>::Buffer(Buffer<T> && other)
	{
		this->operator=(std::move(other));
	}

	template<typename T>
	inline Buffer<T> & Buffer<T>::operator=(Buffer<T> && other)
	{
        this->data = std::move(other.data);
        this->validLength = other.validLength;
        other.validLength = 0;
		return *this;
	}

	template<typename T>
	inline void Buffer<T>::reset()
	{
		this->validLength = 0;
		this->data.reset();
	}

	template<typename T>
	void Buffer<T>::reset(std::int32_t unitsCount)
	{
		this->validLength = 0;
		this->data.reset(unitsCount);
	}
	
}
