add_executable(simple simple.c)

pico_enable_stdio_usb(simple 0)
pico_enable_stdio_uart(simple 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(simple)

target_include_directories(simple PRIVATE ../include)
target_link_directories(simple PRIVATE ../build)
target_link_libraries(simple PRIVATE pico_stdlib hardware_pwm pico-servo fixmath)