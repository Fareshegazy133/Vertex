// Copyright HNDRED GAMES. All Rights Reserved.

#pragma once

#include "Core/Debug/Assert.h"
#include <cstdint>
#include <string>
#include <vector>

/**
 * A failed assert, copied out of the handler so it outlives the failure. CoreTests throws one for VX_ASSERT and VX_VERIFY, and stores one for VX_CHECK.
 *
 * @ownership Owns a copy of every string, so it stays valid after the failing code's stack is gone. Copyable and movable.
 * @lifetime Valid until destroyed. A thrown one lives until the test, or doctest, that catches it is done with it.
 * @threading Test thread only: doctest runs the tests on one thread.
 * @networking Local only.
 */
struct VRecordedAssert
{
	/** Which macro failed. Tests read it to tell VX_ASSERT and VX_VERIFY apart. */
	EAssertKind AssertKind = EAssertKind::Assert;

	/** The formatted message, or empty when the macro had none. */
	std::string Message;

	/** The asserted code as written, such as "Index < Num". */
	std::string Expression;

	/** The line the macro was written on. Tests compare it with __LINE__. */
	std::uint_least32_t Line = 0;
};

namespace Vertex
{
	/**
	 * Makes every failed VX_ASSERT and VX_VERIFY throw a VRecordedAssert, and records every failed VX_CHECK, for the rest of the run.
	 *
	 * @warning Call it once, at the start of main, before any test runs. An assert that fails before then ends the run.
	 * @note An assert nobody catches fails only its own test. doctest prints it through the exception translator, and the run carries on.
	 */
	void InstallAssertRecorder();

	/** Empties the recorded checks. A test calls it first, so reports from earlier tests don't count. */
	void ClearCheckReports();

	/**
	 * Lists the VX_CHECK failures recorded since the last ClearCheckReports.
	 *
	 * @return The reports, oldest first. The reference stays valid for the whole run, but the next failed check or ClearCheckReports changes what it holds.
	 */
	const std::vector<VRecordedAssert>& GetCheckReports();
}
