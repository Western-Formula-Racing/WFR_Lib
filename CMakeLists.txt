idf_component_register(SRCS "./src/CAN.cpp" "./src/CAN_Config.cpp"
                  INCLUDE_DIRS "./include"
                  REQUIRES "driver" "esp_timer")
Include(FetchContent)

FetchContent_Declare(
  etl
  GIT_REPOSITORY https://github.com/ETLCPP/etl
  GIT_TAG        20.42.2
)

FetchContent_MakeAvailable(etl)
target_link_libraries(${COMPONENT_LIB} PRIVATE etl::etl)
target_include_directories(${COMPONENT_LIB} INTERFACE "${etl_SOURCE_DIR}/include")