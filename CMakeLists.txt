cmake_minimum_required(VERSION 3.22)
project(autoplay)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MOD_ID "autoplay")
set(COMPILE_ID "autoplay")

file(GLOB_RECURSE cpp_file_list ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(${COMPILE_ID} SHARED ${cpp_file_list})

file(GLOB_RECURSE all_includes LIST_DIRECTORIES true
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/includes/*)

set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/shared
  ${CMAKE_CURRENT_SOURCE_DIR}/extern/includes
)

foreach(dir ${all_includes})
  if(IS_DIRECTORY ${dir})
    list(APPEND include_dirs ${dir})
  endif()
endforeach()

target_include_directories(${COMPILE_ID} PRIVATE ${include_dirs})

target_compile_options(${COMPILE_ID} PRIVATE -fdeclspec)
target_link_options(${COMPILE_ID} PRIVATE -Wl,--no-undefined)
