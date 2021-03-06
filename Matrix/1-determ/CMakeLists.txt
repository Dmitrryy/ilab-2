cmake_minimum_required(VERSION 3.10)

# set the project name
project(Matrix)

# specify the C++ standard
set(MAKE_CXX_STANDARD 17)

# add the executable
add_executable(${PROJECT_NAME} main.cpp tests/gen_test.cpp ../Libs/matrix/Matrix_other.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ../Libs)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

####################################################

project(MatrixTest)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)

# add the executable
add_executable(${PROJECT_NAME} tests/gen_test.cpp ../Libs/matrix/Matrix_other.cpp tests/test_main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ../Libs)
target_include_directories(${PROJECT_NAME} PRIVATE tests)

# copy resource file to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/tests $<TARGET_FILE_DIR:${PROJECT_NAME}>/tests)

####################################################

project(MatrixGtest)

# specify the C++ standard
set(${PROJECT_NAME} CMAKE_CXX_STANDARD 17)
set(${PROJECT_NAME} CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} tests/Matrix_test.cpp ../Libs/matrix/Matrix_other.cpp tests/gen_test.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
        ../include
        ${GTEST_INCLUDE_DIRS}
        )

target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES} Threads::Threads)

target_include_directories(${PROJECT_NAME} PRIVATE ../Libs)
target_include_directories(${PROJECT_NAME} PRIVATE tests)

enable_testing()
add_test(NAME    MatrixGTest
        COMMAND ${PROJECT_NAME})