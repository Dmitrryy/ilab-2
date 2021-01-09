cmake_minimum_required(VERSION 3.10)

# set the project name
project(Current)

# specify the C++ standard
set(MAKE_CXX_STANDARD 17)

# add the executable
add_executable(${PROJECT_NAME} main.cpp ../Libs/matrix/Matrix_other.cpp lex.yy.cc parser.tab.cc)

target_include_directories(${PROJECT_NAME} PRIVATE ../Libs)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
