include_directories(${CMAKE_HOME_DIRECTORY})

include(CTest)

enable_testing()

add_custom_target(
    check
    COMMAND ${CMAKE_CTEST_COMMAND} -j4 -C ${CMAKE_CFG_INTDIR}
)

set(warnings_flag)
if (NOT MSVC)
    set(warnings_flag -Wall)
endif ()

macro(add_test_executable name)
    add_executable(${name} ${name}.cpp)
    target_compile_options(${name} PRIVATE ${warnings_flag})
    target_link_libraries(${name} iterator_facade gtest gtest_main)
    if (NOT MSVC AND NOT LIMIT_TESTING_FOR_CI) # Boost auto-linking handles this with MSVC.
        target_link_libraries(${name} boost_system boost_filesystem)
    endif ()
    if (MSVC)
        target_compile_options(${name} PRIVATE /source-charset:utf-8)
    endif ()
    set_property(TARGET ${name} PROPERTY CXX_STANDARD ${CXX_STD})
    add_test(${name} ${CMAKE_CURRENT_BINARY_DIR}/${name} --gtest_catch_exceptions=1)
    if (clang_on_linux)
        target_link_libraries(${name} c++)
    endif ()
    if (NOT "${ARGN}" STREQUAL "")
        foreach(arg ${ARGN})
            add_custom_command(
                TARGET ${name}
                POST_BUILD
                COMMAND
                    ${CMAKE_COMMAND} -E copy
                        ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
                        ${CMAKE_CURRENT_BINARY_DIR}
            )
        endforeach()
    endif ()
endmacro()

add_test_executable(input)
add_test_executable(forward)
add_test_executable(bidirectional)
