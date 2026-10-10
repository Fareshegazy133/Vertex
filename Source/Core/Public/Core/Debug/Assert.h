// Copyright HNDRED GAMES. All Rights Reserved.

#pragma once

#include <atomic>
#include <cstdint>
#include <format>
#include <source_location>
#include <string_view>

/** Which assert macro failed. It names the macro in the Error line, and decides whether the program ends. */
enum class EAssertKind : std::uint8_t
{
	/** VX_ASSERT: a broken invariant. Ends the program; compiled out of Shipping. */
	Assert,

	/** VX_VERIFY: like VX_ASSERT, but its expression still runs in Shipping. */
	Verify,

	/** VX_CHECK: a bug the program can survive. Reported once per call site, then the program carries on. */
	Check
};

/**
 * Everything known about one failed assert, as the failure handler receives it.
 *
 * @ownership Built on the stack by the failure path and passed by reference. Borrows every string it holds.
 * @lifetime Valid only during the handler call: Message points into a stack buffer that dies when the handler returns. Copy whatever you keep.
 * @threading Built and read on the thread whose assert failed.
 * @networking Local only.
 */
struct VAssertFailure
{
	/** Which macro failed. A Check carries on after the handler; the others end the program. */
	EAssertKind AssertKind = EAssertKind::Assert;

	/** The formatted message, or empty when the macro had none. Cut at 1024 characters. */
	std::string_view Message;

	/** The asserted code as written, such as "Index < Num". */
	std::string_view Expression;

	/** The file, line, and function where the macro was written. */
	std::source_location Location;
};

namespace Vertex
{
	namespace Private
	{
		/**
		 * Reports a failed VX_ASSERT or VX_VERIFY to the handler, then ends the program. Every fatal failure ends up here.
		 *
		 * @param AssertKind Assert or Verify. It names the macro in the report.
		 * @param Expression The asserted code as written.
		 * @param Location Where the macro was written.
		 * @param Format The message's format string, already checked by FailAssert's std::format_string. Empty when there's no message.
		 * @param Arguments The message's values, packed by std::make_format_args.
		 * @warning Never mark anything on this path noexcept. A handler may throw through it, as CoreTests' does, and noexcept would turn that throw into std::terminate.
		 * @note Calls std::abort() after the handler, even when a custom handler returns.
		 * @note A failure while this thread is already reporting one isn't reported: it ends the program at once. Otherwise an assert inside Log or a handler would recurse until the stack ran out.
		 */
		[[noreturn]] CORE_API void HandleAssertFailure(const EAssertKind AssertKind, const std::string_view Expression, const std::source_location& Location, const std::string_view Format, const std::format_args Arguments);

		/**
		 * Reports a failed VX_CHECK to the handler, then returns so the program carries on.
		 *
		 * @param Expression The checked code as written.
		 * @param Location Where the macro was written.
		 * @param Format The message's format string, already checked by FailCheck's std::format_string. Empty when there's no message.
		 * @param Arguments The message's values, packed by std::make_format_args.
		 * @warning Reach it through VX_CHECK, which reports each call site only once. A direct call reports every time.
		 * @note A check that fails while this thread is already reporting a failure is skipped, so a failing handler can't recurse.
		 */
		CORE_API void HandleCheckFailure(const std::string_view Expression, const std::source_location& Location, const std::string_view Format, const std::format_args Arguments);

		/**
		 * Fails an assert that has no message. VX_ASSERT and VX_VERIFY call it.
		 *
		 * @param AssertKind Assert or Verify.
		 * @param Expression The asserted code as written.
		 * @param Location Where the macro was written.
		 */
		[[noreturn]] inline void FailAssert(const EAssertKind AssertKind, const std::string_view Expression, const std::source_location& Location)
		{
			HandleAssertFailure(AssertKind, Expression, Location, {}, std::make_format_args());
		}

		/**
		 * Fails an assert that has a formatted message. VX_ASSERT and VX_VERIFY call it when a message follows the expression.
		 *
		 * @tparam TArguments The message's value types. Deduced from Arguments.
		 * @param AssertKind Assert or Verify.
		 * @param Expression The asserted code as written.
		 * @param Location Where the macro was written.
		 * @param Format The message's format string. The compiler checks it against Arguments, in every configuration.
		 * @param Arguments The message's values. Only read once the assert has failed.
		 */
		template<typename... TArguments>
		[[noreturn]] void FailAssert(const EAssertKind AssertKind, const std::string_view Expression, const std::source_location& Location, const std::format_string<TArguments...> Format, TArguments&&... Arguments)
		{
			HandleAssertFailure(AssertKind, Expression, Location, Format.get(), std::make_format_args(Arguments...));
		}

		/**
		 * Fails a check that has no message. VX_CHECK calls it the first time its call site fails.
		 *
		 * @param Expression The checked code as written.
		 * @param Location Where the macro was written.
		 */
		inline void FailCheck(const std::string_view Expression, const std::source_location& Location)
		{
			HandleCheckFailure(Expression, Location, {}, std::make_format_args());
		}

		/**
		 * Fails a check that has a formatted message. VX_CHECK calls it the first time its call site fails.
		 *
		 * @tparam TArguments The message's value types. Deduced from Arguments.
		 * @param Expression The checked code as written.
		 * @param Location Where the macro was written.
		 * @param Format The message's format string. The compiler checks it against Arguments, in every configuration.
		 * @param Arguments The message's values. Only read once the check has failed.
		 */
		template<typename... TArguments>
		void FailCheck(const std::string_view Expression, const std::source_location& Location, const std::format_string<TArguments...> Format, TArguments&&... Arguments)
		{
			HandleCheckFailure(Expression, Location, Format.get(), std::make_format_args(Arguments...));
		}
	}

	/**
	 * Replaces the function that receives every failed assert.
	 *
	 * @param Handler The new handler, or nullptr to restore the default, which writes one Error line through Log.
	 * @return The handler it replaced, so a caller can put it back. Never nullptr: the default comes back as a real pointer.
	 * @warning For VX_ASSERT and VX_VERIFY, the program ends as soon as the handler returns. A handler that must survive a failure has to throw, as CoreTests' does.
	 * @note Safe to call from any thread: the swap is atomic. Usually called once, at startup.
	 */
	CORE_API auto SetAssertHandler(void (*const Handler)(const VAssertFailure&)) -> void (*)(const VAssertFailure&);
}

#define VX_ASSERT(Expression, ...) ((VERTEX_ENABLE_ASSERTS && !(Expression)) ? ::Vertex::Private::FailAssert(::EAssertKind::Assert, #Expression, ::std::source_location::current() __VA_OPT__(,) __VA_ARGS__) : static_cast<void>(0))
#define VX_VERIFY(Expression, ...) ((!(Expression) && VERTEX_ENABLE_ASSERTS) ? ::Vertex::Private::FailAssert(::EAssertKind::Verify, #Expression, ::std::source_location::current() __VA_OPT__(,) __VA_ARGS__) : static_cast<void>(0))
#define VX_CHECK(Expression, ...) (static_cast<bool>(Expression) || [&, Location = ::std::source_location::current()]() -> bool \
	{ \
		if constexpr (VERTEX_ENABLE_ASSERTS) \
		{ \
			static ::std::atomic<bool> bReported = false; \
			\
			if (!bReported.exchange(true)) \
			{ \
				::Vertex::Private::FailCheck(#Expression, Location __VA_OPT__(,) __VA_ARGS__); \
			} \
		} \
		\
		return false; \
	}())
