// Copyright HNDRED GAMES. All Rights Reserved.

#include "Debug/AssertRecorder.h"
#include "doctest/doctest.h"
#include <format>
#include <string>
#include <vector>

namespace
{
	std::vector<VRecordedAssert> CheckReports;

	VRecordedAssert Record(const VAssertFailure& AssertFailure)
	{
		return VRecordedAssert{ .AssertKind = AssertFailure.AssertKind, .Message = std::string(AssertFailure.Message), .Expression = std::string(AssertFailure.Expression), .Line = AssertFailure.Location.line() };
	}

	void RecordAssert(const VAssertFailure& AssertFailure)
	{
		if (AssertFailure.AssertKind == EAssertKind::Check)
		{
			CheckReports.push_back(Record(AssertFailure));
		}
		else
		{
			throw Record(AssertFailure);
		}
	}
}

REGISTER_EXCEPTION_TRANSLATOR(const VRecordedAssert& Fired)
{
	const std::string Text = std::format("assert failed: {}, at line {}. {}", Fired.Expression, Fired.Line, Fired.Message);
	return doctest::String(Text.c_str());
}

void Vertex::InstallAssertRecorder()
{
	SetAssertHandler(&RecordAssert);
}

void Vertex::ClearCheckReports()
{
	CheckReports.clear();
}

const std::vector<VRecordedAssert>& Vertex::GetCheckReports()
{
	return CheckReports;
}
