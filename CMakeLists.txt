idf_component_register(
    SRCS
        "src/DW1000Ng.cpp"
        "src/DW1000NgUtils.cpp"
        "src/DW1000NgRanging.cpp"
        "src/DW1000NgRTLS.cpp"
        "src/DW1000NgTime.cpp"
    REQUIRES driver
    INCLUDE_DIRS "src"
)

target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++23)