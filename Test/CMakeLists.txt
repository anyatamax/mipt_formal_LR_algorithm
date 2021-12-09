add_executable(Test main_test.cpp test_LR_algorithm.cpp test_LR_algorithm.h)
target_link_libraries(Test ${GTEST_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} gcov Threads::Threads Library)

enable_testing()
add_test(Test0 ${PROJECT_SOURCE_DIR}/bin/Test)
setup_target_for_coverage(tests ${PROJECT_SOURCE_DIR}/bin/Test coverage)