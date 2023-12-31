cmake_minimum_required(VERSION 3.22)
project(c_racket C)

set(CMAKE_C_STANDARD 11)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX /wd4996)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_library(c_racket_lib
        src/tokens/token_type.h
        src/scanner/scanner.c
        src/scanner/scanner.h
        src/tokens/token_list.c
        src/tokens/token_list.h
        src/tokens/token.c
        src/tokens/token.h
        src/number.c
        src/number.h
        src/ast/ast.c
        src/ast/ast.h
        src/ast/ast_builder.c
        src/ast/ast_builder.h
        src/keyword.c
        src/keyword.h
        src/tokens/token_type.c src/ast/ast_list.c src/ast/ast_list.h src/ast/ast_tag.h src/interpreter/interpreter.c src/interpreter/interpreter.h src/interpreter/function_book.c src/interpreter/function_book.h src/interpreter/function.c src/interpreter/function.h
        src/interpreter/definition_book.c
        src/interpreter/definition_book.h
        src/interpreter/definition.c
        src/interpreter/definition.h)
target_include_directories(c_racket_lib PUBLIC src)

add_executable(c_racket src/main.c)

target_link_libraries(c_racket PRIVATE c_racket_lib)
target_include_directories(c_racket PRIVATE src)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if (BUILD_TESTING)
        add_subdirectory(test)
    endif ()
endif ()