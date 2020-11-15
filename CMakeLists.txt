cmake_minimum_required(VERSION 3.16)
project(fancy-assert CXX)

get_directory_property(has_parent PARENT_DIRECTORY)

if(has_parent)
  set(top_level FALSE)
else()
  set(top_level TRUE)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(top_level)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

include(cmake/compiler_warnings.cmake)
include(cmake/sanitizers.cmake)
include(cmake/conan.cmake)
include(cmake/static_analyzers.cmake)

add_library(fancy-assert ${CMAKE_CURRENT_SOURCE_DIR}/src/fancy_assert.cpp)
target_include_directories(
  fancy-assert PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(top_level AND ENABLE_TESTING)
  add_library(project_options INTERFACE)
  add_library(project_warnings INTERFACE)

  include(cmake/extra_local_settings.cmake)
  set_project_warnings(project_warnings)
  enable_sanitizers(project_options)

  target_link_libraries(fancy-assert INTERFACE project_options project_warnings)

  set(CONAN_REQUIRES # MIT License
      backward-cpp/1.5
  )
  set(CONAN_OPTIONS)
  run_conan()

  enable_testing()
  add_subdirectory(test)
endif()
