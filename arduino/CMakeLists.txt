cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME arduino)
project(${PROJECT_NAME})

set(${CMAKE_PROJECT_NAME}_SKETCH arduino.ino)

#### Uncomment below additional settings as needed.
# set(${CMAKE_PROJECT_NAME}_BOARD due)
# set(${CMAKE_PROJECT_NAME}_PORT /dev/ttyACM0)
# set(mega.build.mcu atmega2560)
# set(mega.upload.protocol wiring)
# set(mega.upload.speed 115200)

#generate_arduino_firmware(${CMAKE_PROJECT_NAME})

get_board_id(board_id due)

add_arduino_executable(${PROJECT_NAME} ${board_id} arduino.ino)
upload_arduino_target(${PROJECT_NAME} "${board_id}" COM3)