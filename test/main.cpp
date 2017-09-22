///
/// \file 				main.cpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2017-09-22
/// \brief 				Contains the main entry point for the unit tests.
/// \details
///		See README.md in root dir for more info.

// 3rd party includes
#include "gtest/gtest.h"


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
  	return RUN_ALL_TESTS();
}