# CMake generated Testfile for 
# Source directory: /Users/noelalben/github/2022-MUSI6106
# Build directory: /Users/noelalben/github/2022-MUSI6106
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(AudioIo "/Users/noelalben/github/2022-MUSI6106/bin/TestExec" "--gtest_filter=AudioIo*" "/Users/noelalben/github/2022-MUSI6106/src/Tests/TestData/")
set_tests_properties(AudioIo PROPERTIES  _BACKTRACE_TRIPLES "/Users/noelalben/github/2022-MUSI6106/CMakeLists.txt;65;add_test;/Users/noelalben/github/2022-MUSI6106/CMakeLists.txt;0;")
add_test(CombFilter "/Users/noelalben/github/2022-MUSI6106/bin/TestExec" "--gtest_filter=CombFilter*")
set_tests_properties(CombFilter PROPERTIES  _BACKTRACE_TRIPLES "/Users/noelalben/github/2022-MUSI6106/CMakeLists.txt;66;add_test;/Users/noelalben/github/2022-MUSI6106/CMakeLists.txt;0;")
add_test(Fft "/Users/noelalben/github/2022-MUSI6106/bin/TestExec" "--gtest_filter=Fft*")
set_tests_properties(Fft PROPERTIES  _BACKTRACE_TRIPLES "/Users/noelalben/github/2022-MUSI6106/CMakeLists.txt;67;add_test;/Users/noelalben/github/2022-MUSI6106/CMakeLists.txt;0;")
subdirs("MUSI6106Exec")
subdirs("AudioFileIO")
subdirs("CombFilter")
subdirs("Fft")
subdirs("googletest")
subdirs("TestExec")
