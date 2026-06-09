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

set(CMAKE_COMPILE_WARNING_AS_ERROR ON) # 调试阶段可以先关闭

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 23) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 收集所有子目录中的源文件（排除各个目录下的 test.cc）
file(GLOB_RECURSE CORE_SOURCES 
    "src/thread/thread.cc"
    "src/fiber/fiber.cc"
    "src/scheduler/scheduler.cc"
    "src/ioscheduler/ioscheduler.cc"
    "src/timer/timer.cc"
    "src/hook/fd_manager.cc"
    "src/hook/hook.cc"
)

# 2. 设置头文件包含路径，这样内部互相引用时比较方便
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 3. 核心：选择打包成动态库还是静态库（二选一，或者两个都建）

# # 选项 A：打包成动态链接库 .so
# add_library(moczkrin_coroutine SHARED ${LIB_SOURCES})

# 选项 B：打包成静态链接库  .a
add_library(moczkrin_coroutine STATIC ${CORE_SOURCES})


# 测试程序
add_executable(test_fiber src/fiber/test.cc)
target_link_libraries(test_fiber moczkrin_coroutine) # 让测试程序链接你刚生成的库

# foreach(MODULE ${MODULES})
#     # 检查对应目录下是否存在 test.cc
#     set(TEST_FILE "${PROJECT_SOURCE_DIR}/src/${MODULE}/test.cc")
#     if(EXISTS ${TEST_FILE})
#         # 定义可执行文件的名字，例如：test_fiber, test_scheduler
#         set(TARGET_NAME "test_${MODULE}")
        
#         # 添加可执行文件
#         add_executable(${TARGET_NAME} ${TEST_FILE})
        
#         # 链接核心库和线程库
#         target_link_libraries(${TARGET_NAME} PRIVATE my_coroutine Threads::Threads)
        
#         message(STATUS "Created test target: ${TARGET_NAME}")
#     endif()
# endforeach()