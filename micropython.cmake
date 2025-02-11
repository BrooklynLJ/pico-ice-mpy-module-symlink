add_library(ice-module INTERFACE)

target_sources(ice-module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ice_module.c
)

target_include_directories(ice-module INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(usermod INTERFACE ice-module)
