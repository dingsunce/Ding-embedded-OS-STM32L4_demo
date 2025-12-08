
file(GLOB_RECURSE unit_test_src_c *.c)
file(GLOB_RECURSE unit_test_src_cpp *.cpp)

add_executable(WINODWS_TEST main.c ${unit_test_src_c} ${unit_test_src_cpp})

target_compile_definitions(DING_OS PUBLIC MEM_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC MESSAGE_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC TASK_DEBUG=1)
target_compile_definitions(DING_OS PUBLIC UNIT_TEST=1)
target_compile_definitions(DING_OS PUBLIC MEM_ALIGMENT_4BYTES=0)
target_compile_definitions(DING_OS PUBLIC UPPERCASE_TYPE_DEFINE=1)
target_compile_definitions(DING_OS PUBLIC BOOL_DEFINE=1)

target_compile_definitions(DING_OS PUBLIC MEM_ALIGMENT_4BYTES=0)


target_link_libraries(WINODWS_TEST PUBLIC DING_OS)
target_link_libraries(WINODWS_TEST PUBLIC OS_AL_LIB)
target_link_libraries(WINODWS_TEST PUBLIC winmm)








