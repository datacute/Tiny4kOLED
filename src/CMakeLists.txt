add_library(Tiny4kOLED INTERFACE)

target_compile_definitions(Tiny4kOLED INTERFACE
    RP2040
)

target_sources(Tiny4kOLED INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/font6x8caps.h
    ${CMAKE_CURRENT_LIST_DIR}/font6x8digits.h
    ${CMAKE_CURRENT_LIST_DIR}/font6x8.h
    ${CMAKE_CURRENT_LIST_DIR}/font6x8p.h
    ${CMAKE_CURRENT_LIST_DIR}/font8x16caps.h
    ${CMAKE_CURRENT_LIST_DIR}/font8x16capsp.h
    ${CMAKE_CURRENT_LIST_DIR}/font8x16digits.h
    ${CMAKE_CURRENT_LIST_DIR}/font8x16.h
    ${CMAKE_CURRENT_LIST_DIR}/font8x16p.h
    ${CMAKE_CURRENT_LIST_DIR}/Tiny4kOLED_bitbang.h
    ${CMAKE_CURRENT_LIST_DIR}/Tiny4kOLED_common.h
    ${CMAKE_CURRENT_LIST_DIR}/Tiny4kOLED.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Tiny4kOLED.h
    ${CMAKE_CURRENT_LIST_DIR}/Tiny4kOLEDprintless.h
    ${CMAKE_CURRENT_LIST_DIR}/Tiny4kOLED_rp2040.h
    ${CMAKE_CURRENT_LIST_DIR}/Tiny4kOLED_tiny-i2c.h
    ${CMAKE_CURRENT_LIST_DIR}/Tiny4kOLED_TinyWireM.h
    ${CMAKE_CURRENT_LIST_DIR}/Tiny4kOLED_Wire.h
)

target_include_directories(Tiny4kOLED INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(Tiny4kOLED INTERFACE
    hardware_i2c
)
