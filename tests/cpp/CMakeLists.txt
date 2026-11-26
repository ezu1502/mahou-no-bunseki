find_package(GTest CONFIG REQUIRED)

add_executable(bunseki_tests
    test_analysis.cpp
)

target_link_libraries(bunseki_tests PRIVATE
    bunseki
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(bunseki_tests)