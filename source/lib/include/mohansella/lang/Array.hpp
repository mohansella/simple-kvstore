#pragma once

#include <string>
#include <cstdint>

namespace mohansella::lang
{

	template<typename T>
	struct Array
	{

	public:
		Array() = default;
		Array(std::int32_t initSize);
		Array(Array<T> && other); //move constructor
		~Array();

		Array<T>& operator=(Array<T> && other); //move assignment

		void reset();
		void reset(std::int32_t unitsCount);
		void reset(T * data, std::int32_t unitsCount);
		T * release();

		T & operator[](std::int32_t index);
		const T & operator[](std::int32_t index) const;

		T * unitsPtr = nullptr;
		std::int32_t unitsCount = 0;

	private:
		//its very unsafe to have a copy constructor here which is auto generated. deleting it.
		//also, forcing pass by reference will make the code more readable/meaningful from javadevs mindset.
		Array(const Array &) = delete;
		Array& operator=(const Array & string) = delete;

	};

}

//----- Inline methods ------
namespace mohansella::lang
{
	template<typename T>
	inline Array<T>::Array(std::int32_t initSize)
	{
		this->reset(initSize);
	}

	template<typename T>
	inline Array<T>::Array(Array<T> && other)
	{
		this->operator=(std::move(other));
	}

	template<typename T>
	inline Array<T> & Array<T>::operator=(Array<T> && other)
	{
		this->reset();
		this->unitsPtr = other.unitsPtr;
		this->unitsCount = other.unitsCount;
		other.release();
		return *this;
	}

	template<typename T>
	inline Array<T>::~Array()
	{
		this->reset();
	}

	template<typename T>
	inline void Array<T>::reset()
	{
		if (this->unitsPtr)
		{
			delete[] this->unitsPtr;
			this->unitsPtr = nullptr;
			this->unitsCount = 0;
		}
	}

	template<typename T>
	inline void Array<T>::reset(std::int32_t unitsCount)
	{
		this->reset(new T[unitsCount], unitsCount);
	}

	template<typename T>
	inline void Array<T>::reset(T * unitsPtr, std::int32_t unitsCount)
	{
		if (this->unitsPtr)
		{
			delete[] this->unitsPtr;
		}
		this->unitsPtr = unitsPtr;
		this->unitsCount = unitsCount;
	}

	template<typename T>
	inline T * Array<T>::release()
	{
		auto toReturn = this->unitsPtr;
		this->unitsPtr = nullptr;
		this->unitsCount = 0;
		return toReturn;
	}

	template<typename T>
	inline T & Array<T>::operator[](std::int32_t index)
	{
		return this->unitsPtr[index];
	}

	template<typename T>
	inline const T & Array<T>::operator[](std::int32_t index) const
	{
		return this->unitsPtr[index];
	}

}
