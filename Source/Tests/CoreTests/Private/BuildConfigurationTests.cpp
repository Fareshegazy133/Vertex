// Copyright HNDRED GAMES. All Rights Reserved.

#include "doctest/doctest.h"

TEST_CASE("Build configuration: exactly one configuration define is set")
{
	int ConfigurationDefines = 0;
#if defined(VERTEX_DEBUG)
	++ConfigurationDefines;
#endif
#if defined(VERTEX_DEVELOPMENT)
	++ConfigurationDefines;
#endif
#if defined(VERTEX_SHIPPING)
	++ConfigurationDefines;
#endif
	CHECK(ConfigurationDefines == 1);
}

TEST_CASE("Build configuration: asserts are compiled out of Shipping only")
{
#if defined(VERTEX_SHIPPING)
	CHECK(VERTEX_ENABLE_ASSERTS == 0);
#else
	CHECK(VERTEX_ENABLE_ASSERTS == 1);
#endif
}
