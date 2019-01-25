cmake_minimum_required (VERSION 2.8)

project(monster)

if ($ENV{DEMO_HOST} STREQUAL mstar)
  set (CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
  set (CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
endif ($ENV{DEMO_HOST} STREQUAL mstar)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/wolfssl")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../b64")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")
include_directories(${THIRD_INC_PATH})
link_directories(${THIRD_LIB_PATH})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../b64 B64_SOURCE_LIST)
set(SOURCE_LIST device.c common.c connect.c router.c)		
add_executable(demo-mqtt 	${SOURCE_LIST} ${B64_SOURCE_LIST})	
target_link_libraries(demo-mqtt mqtt wolfmqtt wolfssl m pthread)


