// Copyright HNDRED GAMES. All Rights Reserved.

#include "Debug/AssertRecorder.h"
#include "doctest/doctest.h"

int main(const int ArgC, char** const ArgV)
{
	Vertex::InstallAssertRecorder();
	doctest::Context Context(ArgC, ArgV);
	return Context.run();
}
