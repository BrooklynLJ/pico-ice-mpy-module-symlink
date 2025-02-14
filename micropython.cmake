add_library(ice-module INTERFACE)

include(${CMAKE_CURRENT_LIST_DIR}/pico-ice-sdk/cmake/preinit_pico_ice_sdk.cmake)

target_sources(ice-module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ice_module.c
)

target_include_directories(ice-module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(usermod INTERFACE ice-module)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pico-ice-sdk pico-ice-sdk)
target_link_libraries(ice-module INTERFACE pico_ice_sdk)
