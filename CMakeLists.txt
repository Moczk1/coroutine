cmake_minimum_required(VERSION 3.21)

project(coroutine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(coroutine_core
    thread/thread.cc
    fiber/fiber.cc
    scheduler/scheduler.cc
    timer/timer.cc
    iomanager/ioscheduler.cc
)

target_include_directories(coroutine_core PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/thread
    ${PROJECT_SOURCE_DIR}/fiber
    ${PROJECT_SOURCE_DIR}/scheduler
    ${PROJECT_SOURCE_DIR}/timer
    ${PROJECT_SOURCE_DIR}/iomanager
)

target_compile_options(coroutine_core PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(coroutine_core PUBLIC
    Threads::Threads
)

# add_executable(thread_test
#     thread/main.cc
# )

# target_link_libraries(thread_test PRIVATE
#     coroutine_core
# )

# add_executable(fiber_test
#     fiber/test.cc
# )

# target_link_libraries(fiber_test PRIVATE
#     coroutine_core
# )

# add_executable(scheduler_test
#     scheduler/test.cc
# )

# target_link_libraries(scheduler_test PRIVATE
#     coroutine_core
# )

# add_executable(timer_test
#     timer/test.cc
# )

# target_link_libraries(timer_test PRIVATE
#     coroutine_core
# )

add_executable(iomanager_test
    iomanager/test.cc
)

target_link_libraries(iomanager_test PRIVATE
    coroutine_core
)