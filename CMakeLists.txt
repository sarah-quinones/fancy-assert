cmake_minimum_required(VERSION 3.16)
project(fancy-assert CXX)

get_directory_property(has_parent PARENT_DIRECTORY)
set(top_level $<NOT:$<BOOL:${has_parent}>>)

if(top_level)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(cmake/compiler_warnings.cmake)
include(cmake/sanitizers.cmake)
include(cmake/conan.cmake)
include(cmake/static_analyzers.cmake)

add_library(fancy-assert ${CMAKE_CURRENT_SOURCE_DIR}/src/fancy_assert.cpp)
target_include_directories(
  fancy-assert PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(top_level)
  add_library(project_options INTERFACE)
  add_library(project_warnings INTERFACE)

  include(cmake/extra_local_settings.cmake)
  set_project_warnings(project_warnings)
  enable_sanitizers(project_options)

  target_link_libraries(fancy-assert INTERFACE project_options project_warnings)

  option(ENABLE_TESTING OFF)
  if(ENABLE_TESTING)
    set(CONAN_REQUIRES # MIT License
        backward-cpp/1.5
    )
    set(CONAN_OPTIONS)

    run_conan()

    enable_testing()
    add_subdirectory(test)
  endif()
endif()
