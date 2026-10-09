// Copyright HNDRED GAMES. All Rights Reserved.

#include "doctest/doctest.h"

int main(const int ArgC, char** const ArgV)
{
	doctest::Context Context(ArgC, ArgV);
	return Context.run();
}
