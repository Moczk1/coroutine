# cmake_minimum_required(VERSION 3.21)

# project(coroutine)

# set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# find_package(Threads REQUIRED)

# add_library(coroutine_core
#     thread/thread.cc
#     fiber/fiber.cc
#     scheduler/scheduler.cc
#     timer/timer.cc
#     ioscheduler/ioscheduler.cc
#     hook/hook.cc
#     hook/fd_manager.cc
# )

# target_include_directories(coroutine_core PUBLIC
#     ${PROJECT_SOURCE_DIR}
#     ${PROJECT_SOURCE_DIR}/thread
#     ${PROJECT_SOURCE_DIR}/fiber
#     ${PROJECT_SOURCE_DIR}/scheduler
#     ${PROJECT_SOURCE_DIR}/timer
#     ${PROJECT_SOURCE_DIR}/ioscheduler
#     ${PROJECT_SOURCE_DIR}/hook
# )

# target_compile_options(coroutine_core PRIVATE
#     -Wall
#     -Wextra
#     -Wpedantic
# )

# # target_link_libraries(coroutine_core PUBLIC
# #     Threads::Threads
# # )

# # add_executable(thread_test
# #     thread/test.cc
# # )

# # target_link_libraries(thread_test PRIVATE
# #     coroutine_core
# # )

# # add_executable(fiber_test
# #     fiber/test.cc
# # )

# # target_link_libraries(fiber_test PRIVATE
# #     coroutine_core
# # )

# # add_executable(scheduler_test
# #     scheduler/test.cc
# # )

# # target_link_libraries(scheduler_test PRIVATE
# #     coroutine_core
# # )

# # add_executable(timer_test
# #     timer/test.cc
# # )

# # target_link_libraries(timer_test PRIVATE
# #     coroutine_core
# # )

# # add_executable(ioscheduler_test
# #     ioscheduler/test.cc
# # )

# # target_link_libraries(ioscheduler_test PRIVATE
# #     coroutine_core
# # )

# add_executable(hook_test
#     hook/test.cc
# )

# target_link_libraries(hook_test PRIVATE
#     coroutine_core
# )


cmake_minimum_required(VERSION 3.31)
project(my_coroutine VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 23) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 收集所有子目录中的源文件（排除各个目录下的 test.cc）
file(GLOB_RECURSE LIB_SOURCES 
    "thread/thread.cc"
    "fiber/fiber.cc"
    "scheduler/scheduler.cc"
    "ioscheduler/ioscheduler.cc"
    "timer/timer.cc"
    "hook/fd_manager.cc"
    "hook/hook.cc"
)

# 2. 设置头文件包含路径，这样内部互相引用时比较方便
# 让编译器去这几个子目录里寻找头文件
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/thread
    ${CMAKE_CURRENT_SOURCE_DIR}/fiber
    ${CMAKE_CURRENT_SOURCE_DIR}/scheduler
    ${CMAKE_CURRENT_SOURCE_DIR}/ioscheduler
    ${CMAKE_CURRENT_SOURCE_DIR}/timer
    ${CMAKE_CURRENT_SOURCE_DIR}/hook
)

# 3. 核心：选择打包成动态库还是静态库（二选一，或者两个都建）

# 选项 A：打包成动态链接库（libmy_coroutine.so）
add_library(my_coroutine SHARED ${LIB_SOURCES})

# # 选项 B：打包成静态链接库（libmy_coroutine.a）
# add_library(my_coroutine STATIC ${LIB_SOURCES})

# # 4. 如果你的安全/异步处理链接了系统 pthread 库
# find_package(Threads REQUIRED)
# target_link_libraries(my_coroutine PRIVATE Threads::Threads)

# 5. 可选：如果你还想编译你原有的测试程序，可以这样写
add_executable(test_fiber fiber/test.cc)
target_link_libraries(test_fiber my_coroutine) # 让测试程序链接你刚生成的库