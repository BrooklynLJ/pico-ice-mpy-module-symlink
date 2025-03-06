add_library(ice-module INTERFACE)

include(${CMAKE_CURRENT_LIST_DIR}/pico-ice-sdk/cmake/preinit_pico_ice_sdk.cmake)

set(PICO_SDK_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/pico-ice-sdk)

target_sources(ice-module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ice_module.c
    ${PICO_SDK_DIRECTORY}/src/ice_fpga.c
    ${PICO_SDK_DIRECTORY}/src/ice_flash.c
    ${PICO_SDK_DIRECTORY}/src/ice_cram.c
    ${PICO_SDK_DIRECTORY}/src/pico-sdk_ice_hal.c
)

target_include_directories(ice-module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${PICO_SDK_DIRECTORY}/include
)

pico_generate_pio_header(ice-module
    ${CMAKE_CURRENT_LIST_DIR}/ice_cram.pio
    )

target_link_libraries(usermod INTERFACE ice-module)
