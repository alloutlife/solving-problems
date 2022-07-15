cmake_minimum_required( VERSION 3.10 )

set(CMAKE_CXX_STANDARD 20)

project( problems-sandbox )

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(
        -Wall -Wextra -pedantic
        # -Werror  <-- a must for commercial projects. However we need some warnings remain
    )
endif()

add_subdirectory( longest-substr-without-rep-chars )
add_subdirectory( building-h2o )
add_subdirectory( longest-palindrome-substr )
add_subdirectory( best-sum )
