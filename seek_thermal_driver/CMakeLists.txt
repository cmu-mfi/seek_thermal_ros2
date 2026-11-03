cmake_minimum_required(VERSION 3.10)
project(seek_thermal_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

# libseek-thermal accesses the camera through libusb-1.0
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(LibUSB)
if(NOT LIBUSB_FOUND)
  message(FATAL_ERROR
    "libusb-1.0 not found. Install it with: sudo apt install libusb-1.0-0-dev")
endif()

# ---------------------------------------------------------------------------
# Vendored libseek-thermal library
# ---------------------------------------------------------------------------
option(SEEK_DEBUG "Enable verbose libseek-thermal debug messages" OFF)

set(SEEK_HEADERS
  src/libseek/SeekCam.h
  src/libseek/SeekDevice.h
  src/libseek/seek.h
  src/libseek/SeekLogging.h
  src/libseek/SeekThermal.h
  src/libseek/SeekThermalPro.h
)

set(SEEK_SOURCES
  src/libseek/SeekCam.cpp
  src/libseek/SeekDevice.cpp
  src/libseek/SeekThermal.cpp
  src/libseek/SeekThermalPro.cpp
)

# libusb uses non-standard zero-size flexible array members, so include it as
# a system header to keep its warnings out of our builds under -Wpedantic.
include_directories(SYSTEM ${LIBUSB_INCLUDE_DIRS})

add_library(seek_static STATIC ${SEEK_SOURCES} ${SEEK_HEADERS})
target_compile_features(seek_static PUBLIC cxx_std_11)
target_include_directories(seek_static
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/libseek>
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(seek_static
  PUBLIC
    ${OpenCV_LIBS}
  PRIVATE
    ${LIBUSB_LIBRARIES}
)
if(SEEK_DEBUG)
  target_compile_definitions(seek_static PRIVATE SEEK_DEBUG)
endif()

# ---------------------------------------------------------------------------
# ROS 2 driver
# ---------------------------------------------------------------------------
add_executable(seek_thermal_driver src/seek_thermal_driver.cpp)
ament_target_dependencies(seek_thermal_driver rclcpp sensor_msgs cv_bridge OpenCV)
target_link_libraries(seek_thermal_driver seek_static)

add_executable(seek_thermal_driver_node src/seek_thermal_driver.cpp)
ament_target_dependencies(seek_thermal_driver_node rclcpp sensor_msgs cv_bridge OpenCV)
target_link_libraries(seek_thermal_driver_node seek_static)

install(
  TARGETS
    seek_thermal_driver
    seek_thermal_driver_node
  DESTINATION
    lib/${PROJECT_NAME}
)

ament_package()
