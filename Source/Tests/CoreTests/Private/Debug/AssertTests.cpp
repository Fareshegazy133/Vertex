// Copyright HNDRED GAMES. All Rights Reserved.

#include "Core/Debug/Assert.h"
#include "Debug/AssertRecorder.h"
#include "doctest/doctest.h"
#include <cstdint>
#include <string>

namespace
{
	constinit int NestedFailureCalls = 0;

	bool CheckOnlyOnce(const int Value)
	{
		return VX_CHECK(Value > 0, "Value was {}", Value);
	}

	void FailAgainWhileReporting(const VAssertFailure&)
	{
		++NestedFailureCalls;
		VX_CHECK(false, "Nested failure");
	}
}

TEST_CASE("VX_ASSERT: stays silent when the expression is true")
{
	const int OnlyInExpression = 1;
	const int OnlyInMessage = 2;
	CHECK_NOTHROW(VX_ASSERT(OnlyInExpression == 1, "{}", OnlyInMessage));
}

TEST_CASE("VX_ASSERT: reports the expression, line, and message when it fails")
{
	constexpr int Index = 7;
	constexpr int Num = 5;
#if VERTEX_ENABLE_ASSERTS
	bool bFired = false;
	std::uint_least32_t ExpectedLine = 0;
	try
	{
		ExpectedLine = __LINE__ + 1;
		VX_ASSERT(Index < Num, "Index {} is out of range for {} elements", Index, Num);
	}
	catch (const VRecordedAssert& Fired)
	{
		bFired = true;
		CHECK(Fired.AssertKind == EAssertKind::Assert);
		CHECK(Fired.Expression == "Index < Num");
		CHECK(Fired.Line == ExpectedLine);
		CHECK(Fired.Message == "Index 7 is out of range for 5 elements");
	}
	CHECK(bFired);
#else
	CHECK_NOTHROW(VX_ASSERT(Index < Num, "Index {} is out of range for {} elements", Index, Num));
#endif
}

TEST_CASE("VX_ASSERT: never evaluates its expression in Shipping")
{
	int Evaluations = 0;
	VX_ASSERT(++Evaluations > 0);
	CHECK(Evaluations == (VERTEX_ENABLE_ASSERTS ? 1 : 0));
}

TEST_CASE("VX_VERIFY: evaluates its expression in every configuration")
{
	int Evaluations = 0;
	VX_VERIFY(++Evaluations > 0);
	CHECK(Evaluations == 1);
}

TEST_CASE("VX_VERIFY: fails like VX_ASSERT outside Shipping")
{
	constexpr int Value = 0;
#if VERTEX_ENABLE_ASSERTS
	bool bFired = false;
	try
	{
		VX_VERIFY(Value > 0);
	}
	catch (const VRecordedAssert& Fired)
	{
		bFired = true;
		CHECK(Fired.AssertKind == EAssertKind::Verify);
		CHECK(Fired.Expression == "Value > 0");
		CHECK(Fired.Message.empty());
	}
	CHECK(bFired);
#else
	CHECK_NOTHROW(VX_VERIFY(Value > 0));
#endif
}

TEST_CASE("VX_VERIFY: reports its message")
{
	constexpr int Value = 0;
#if VERTEX_ENABLE_ASSERTS
	bool bFired = false;
	try
	{
		VX_VERIFY(Value > 0, "Value was {}", Value);
	}
	catch (const VRecordedAssert& Fired)
	{
		bFired = true;
		CHECK(Fired.AssertKind == EAssertKind::Verify);
		CHECK(Fired.Message == "Value was 0");
	}
	CHECK(bFired);
#else
	CHECK_NOTHROW(VX_VERIFY(Value > 0, "Value was {}", Value));
#endif
}

TEST_CASE("VX_CHECK: returns the expression's result and carries on")
{
	Vertex::ClearCheckReports();
	CHECK(VX_CHECK(true));
	CHECK_FALSE(VX_CHECK(false));
#if VERTEX_ENABLE_ASSERTS
	REQUIRE(Vertex::GetCheckReports().size() == 1);
	CHECK(Vertex::GetCheckReports()[0].AssertKind == EAssertKind::Check);
	CHECK(Vertex::GetCheckReports()[0].Expression == "false");
#else
	CHECK(Vertex::GetCheckReports().empty());
#endif
}

TEST_CASE("VX_CHECK: reports only the first failure at each line")
{
	Vertex::ClearCheckReports();
	CHECK_FALSE(CheckOnlyOnce(-1));
	CHECK_FALSE(CheckOnlyOnce(-2));
	CHECK(CheckOnlyOnce(3));
#if VERTEX_ENABLE_ASSERTS
	REQUIRE(Vertex::GetCheckReports().size() == 1);
	CHECK(Vertex::GetCheckReports()[0].Message == "Value was -1");
#else
	CHECK(Vertex::GetCheckReports().empty());
#endif
}

TEST_CASE("VX_ASSERT: cuts a message longer than the buffer")
{
	const std::string LongText(4000, 'x');
#if VERTEX_ENABLE_ASSERTS
	bool bFired = false;
	try
	{
		VX_ASSERT(LongText.empty(), "{}", LongText);
	}
	catch (const VRecordedAssert& Fired)
	{
		bFired = true;
		CHECK(Fired.Message == std::string(1024, 'x'));
	}
	CHECK(bFired);
#else
	CHECK_NOTHROW(VX_ASSERT(LongText.empty(), "{}", LongText));
#endif
}

TEST_CASE("VX_ASSERT: a handler that throws leaves later failures reportable")
{
#if VERTEX_ENABLE_ASSERTS
	CHECK_THROWS_AS(VX_ASSERT(false, "First failure"), VRecordedAssert);
	CHECK_THROWS_AS(VX_ASSERT(false, "Second failure"), VRecordedAssert);
#else
	CHECK_NOTHROW(VX_ASSERT(false));
#endif
}

TEST_CASE("VX_CHECK: a failure inside the handler is not reported again")
{
	NestedFailureCalls = 0;
	const auto PreviousHandler = Vertex::SetAssertHandler(&FailAgainWhileReporting);
	CHECK_FALSE(VX_CHECK(false));
	Vertex::SetAssertHandler(PreviousHandler);
	CHECK(NestedFailureCalls == (VERTEX_ENABLE_ASSERTS ? 1 : 0));
}

TEST_CASE("SetAssertHandler: nullptr puts the default handler back")
{
	Vertex::ClearCheckReports();
	const auto PreviousHandler = Vertex::SetAssertHandler(nullptr);
	CHECK_FALSE(VX_CHECK(false, "Expected: this proves the default handler is back"));
	Vertex::SetAssertHandler(PreviousHandler);
	CHECK(Vertex::GetCheckReports().empty());
}

TEST_CASE("SetAssertHandler: returns the handler it replaced")
{
	Vertex::ClearCheckReports();
	const auto ReplacedHandler = Vertex::SetAssertHandler(nullptr);
	Vertex::SetAssertHandler(ReplacedHandler);
	CHECK_FALSE(VX_CHECK(false));
#if VERTEX_ENABLE_ASSERTS
	CHECK(Vertex::GetCheckReports().size() == 1);
#else
	CHECK(Vertex::GetCheckReports().empty());
#endif
}
