cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico-servo)

# Initialize the SDK
pico_sdk_init()

add_library(${PROJECT_NAME}
	src/pico_servo.c
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_pwm)

if (${BUILD_EXAMPLES})
        add_subdirectory(examples)
endif()
