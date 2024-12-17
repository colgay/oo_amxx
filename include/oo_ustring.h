#ifndef OO_USTRING_H
#define OO_USTRING_H

#include <cstring>
#include <memory>

namespace oo
{
	class UString 
	{
	private:
		std::unique_ptr<char[]> data;
		size_t length;

	public:
		UString() : data(nullptr), length(0) {}

		UString(const char* str, size_t length = -1)
		{
			if (length == -1)
				length = strlen(str);

			data = std::make_unique<char[]>(length + 1);
			strcpy(data.get(), str);
		}

		UString(const UString& other)
		{
			length = other.length;
			data = std::make_unique<char[]>(length + 1);
			strcpy(data.get(), other.data.get());
		}

		UString(UString&& other) noexcept
			: data(std::move(other.data)), length(other.length)
		{
			other.length = 0;
		}

		UString& operator=(const UString& other)
		{
			if (this == &other) return *this;

			length = other.length;
			data = std::make_unique<char[]>(length + 1);
			strcpy(data.get(), other.data.get());
			return *this;
		}

		UString& operator=(UString&& other) noexcept
		{
			if (this == &other) return *this;

			data = std::move(other.data);
			length = other.length;
			other.length = 0;
			return *this;
		}

		size_t size() const
		{
			return length;
		}

		char* get()
		{
			return data.get();
		}
	};
}

#endif // OO_USTRING_H