// Copyright HNDRED GAMES. All Rights Reserved.

#include "Core/Debug/Assert.h"
#include "Core/Logging/Log.h"
#include "Logging/FormatToBuffer.h"
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <string_view>

namespace
{
	constexpr std::size_t MaxAssertMessageLength = 1024;

	constinit thread_local bool bReportingFailure = false;

	struct VReportingScope
	{
		VReportingScope()
		{
			bReportingFailure = true;
		}

		VReportingScope(const VReportingScope&) = delete;

		~VReportingScope()
		{
			bReportingFailure = false;
		}

		VReportingScope& operator=(const VReportingScope&) = delete;
	};

	std::string_view AssertKindToMacroName(const EAssertKind AssertKind)
	{
		switch (AssertKind)
		{
		case EAssertKind::Assert:
			return "VX_ASSERT";
		case EAssertKind::Verify:
			return "VX_VERIFY";
		case EAssertKind::Check:
			return "VX_CHECK";
		}

		return "Unknown";
	}

	void LogAssertFailure(const VAssertFailure& AssertFailure)
	{
		if (AssertFailure.Message.empty())
		{
			Vertex::Log(ELogLevel::Error, "{} failed: {}, at {}({})", AssertKindToMacroName(AssertFailure.AssertKind), AssertFailure.Expression, AssertFailure.Location.file_name(), AssertFailure.Location.line());
		}
		else
		{
			Vertex::Log(ELogLevel::Error, "{} failed: {}, at {}({}). {}", AssertKindToMacroName(AssertFailure.AssertKind), AssertFailure.Expression, AssertFailure.Location.file_name(), AssertFailure.Location.line(), AssertFailure.Message);
		}
	}

	constinit std::atomic AssertHandler = &LogAssertFailure;

	void DispatchAssertFailure(const EAssertKind AssertKind, const std::string_view Expression, const std::source_location& Location, const std::string_view Format, const std::format_args Arguments)
	{
		if (bReportingFailure)
		{
			return;
		}

		const VReportingScope ReportingScope;
		std::array<char, MaxAssertMessageLength> Buffer;
		const VAssertFailure Failure = { .AssertKind = AssertKind, .Message = Vertex::FormatToBuffer(Buffer, Format, Arguments), .Expression = Expression, .Location = Location };
		AssertHandler.load()(Failure);
	}
}

void Vertex::Private::HandleAssertFailure(const EAssertKind AssertKind, const std::string_view Expression, const std::source_location& Location, const std::string_view Format, const std::format_args Arguments)
{
	DispatchAssertFailure(AssertKind, Expression, Location, Format, Arguments);
	std::abort();
}

void Vertex::Private::HandleCheckFailure(const std::string_view Expression, const std::source_location& Location, const std::string_view Format, const std::format_args Arguments)
{
	DispatchAssertFailure(EAssertKind::Check, Expression, Location, Format, Arguments);
}

auto Vertex::SetAssertHandler(void (*const Handler)(const VAssertFailure&)) -> void (*)(const VAssertFailure&)
{
	return AssertHandler.exchange(Handler ? Handler : &LogAssertFailure);
}
