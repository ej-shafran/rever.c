configure_file(config.h.in config.h @ONLY ESCAPE_QUOTES)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_sources(${PROJECT_NAME}-cli PRIVATE cli.c reverc.c)
target_sources(${PROJECT_NAME} PRIVATE gui.c reverc.c)
