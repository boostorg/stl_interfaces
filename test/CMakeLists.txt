# Copyright (C) 2019 T. Zachary Laine
#
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
include_directories(${CMAKE_HOME_DIRECTORY})

include(CTest)

enable_testing()

add_custom_target(stl_interfaces_check COMMAND ${CMAKE_CTEST_COMMAND} -j4 -C ${CMAKE_CFG_INTDIR})
if (NOT TARGET check)
    add_custom_target(check DEPENDS stl_interfaces_check)
else()
    add_dependencies(check stl_interfaces_check)
endif()

set(warnings_flag)
if (NOT MSVC)
    set(warnings_flag -Wall)
endif ()

macro(add_test_executable name)
    add_executable(${name} ${name}.cpp)
    target_compile_options(${name} PRIVATE ${warnings_flag})
    target_link_libraries(${name} stl_interfaces)
    target_compile_definitions(${name} PRIVATE BOOST_NO_AUTO_PTR)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD ${CXX_STD})
    add_test(${name} ${CMAKE_CURRENT_BINARY_DIR}/${name})
    if (clang_on_linux)
        target_link_libraries(${name} c++)
    endif ()
endmacro()

add_test_executable(input)
add_test_executable(output)
add_test_executable(forward)
add_test_executable(bidirectional)
add_test_executable(random_access)
add_test_executable(reverse_iter)
add_test_executable(detail)
add_test_executable(static_vec)
add_test_executable(array)

if (HAVE_CMCSTL2)
    add_test_executable(v2_input)
    add_test_executable(v2_output)
    add_test_executable(v2_forward)
    add_test_executable(v2_bidirectional)
    add_test_executable(v2_random_access)
    add_test_executable(v2_static_vec)
    add_test_executable(v2_array)
endif()
