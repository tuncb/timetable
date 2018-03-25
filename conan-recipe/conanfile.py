from conans import ConanFile

class TimeTableConan(ConanFile):
    name = "timetable"
    version = "1.0.0"
    license = "https://www.apache.org/licenses/LICENSE-2.0"
    url = "https://github.com/tuncb/timetable"
    description = ("A data structure for iterative discrete time, mainly for FEM analysis")

    def package(self):
        self.copy("*", dst="timetable", src="../projects/timetable/include/timetable")
    def package_info(self):
        self.cpp_info.includedirs = ['.']
