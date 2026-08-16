cmake_minimum_required(VERSION 3.20)
project(gabs-test)

enable_testing()
include(CTest)

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)
include(Catch)

add_subdirectory(../ gabs)

gabs_select(gabs-testing-catch2)

add_subdirectory(mutex)
add_subdirectory(semaphore)
add_subdirectory(pbuf)
add_subdirectory(timer)
add_subdirectory(log)
