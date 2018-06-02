PROJECT(timeline_tester)
cmake_minimum_required(VERSION 3.1.2)

include(../../conan.cmake)
conan_cmake_run(BASIC_SETUP CMAKE_TARGETS CONAN_CMAKE_MULTI
  REQUIRES "catch2/2.2.2@bincrafters/stable")

set(SOURCES "./source/TestTimeline.cpp")
set(TIMELINE_LIB "../timeline/include/timeline/timeline.h")

add_executable(timeline_tester ${SOURCES} ${TIMELINE_LIB})
target_include_directories(timeline_tester PRIVATE "../timeline/include/")
target_link_libraries(timeline_tester CONAN_PKG::catch2)  

source_group("timeline_library" FILES ${TIMELINE_LIB})

set_target_properties(timeline_tester PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

