// Copyright HNDRED GAMES. All Rights Reserved.

#include "Logging/FormatToBuffer.h"
#include <cstddef>
#include <iterator>

namespace
{
	struct VTruncatingWriter
	{
		using value_type = char;
		void push_back(const char Character);

		std::span<char> Buffer;
		std::size_t Size = 0;
	};

	void VTruncatingWriter::push_back(const char Character)
	{
		if (Size < Buffer.size())
		{
			Buffer[Size] = Character;
			Size++;
		}
	}
}

std::string_view Vertex::FormatToBuffer(const std::span<char> Buffer, const std::string_view Format, const std::format_args Arguments)
{
	VTruncatingWriter Writer = { .Buffer = Buffer };
	static_cast<void>(std::vformat_to(std::back_inserter(Writer), Format, Arguments));
	return std::string_view(Buffer.data(), Writer.Size);
}
