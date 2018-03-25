PROJECT(timetable_tester)
cmake_minimum_required(VERSION 2.8.12)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.10/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(BASIC_SETUP CMAKE_TARGETS CONAN_CMAKE_MULTI
  REQUIRES "boost/1.66.0@conan/stable")

set(SOURCES "./source/TimetableTester.cpp")
set(TIMETABLE_LIB "../timetable/include/timetable/timetable.h")

add_executable(timetable_tester ${SOURCES} ${TIMETABLE_LIB})
target_include_directories(timetable_tester PRIVATE "../timetable/include/")
target_link_libraries(timetable_tester CONAN_PKG::boost)  

source_group("timetable_library" FILES ${TIMETABLE_LIB})

