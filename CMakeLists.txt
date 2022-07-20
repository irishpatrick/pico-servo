cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico-servo)

# Initialize the SDK
pico_sdk_init()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

add_library(${PROJECT_NAME}
	src/pico_servo.c
)

target_include_directories(${PROJECT_NAME} PRIVATE include ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/pico-libfixmath/include)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_pwm fixmath)

if (${BUILD_EXAMPLES})
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif()
