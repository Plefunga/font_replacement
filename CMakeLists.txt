cmake_minimum_required(VERSION 2.8)
project( fontreplacement )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories(lib)

add_library(iniparser INTERFACE lib/iniparser.hpp)
include_directories(iniparser)

add_library(colour STATIC src/colour.cpp lib/colour.h)
include_directories(colour)


add_library(config STATIC src/config.cpp lib/config.h)
target_link_libraries(config ${OpenCV_LIBS})
target_link_libraries(config iniparser)
include_directories(config)

add_library(textbox STATIC src/textbox.cpp lib/textbox.h)
target_link_libraries(textbox ${OpenCV_LIBS})
target_link_libraries(textbox colour)
include_directories(textbox)

add_library(frame STATIC src/frame.cpp lib/frame.h)
target_link_libraries(frame ${OpenCV_LIBS})
target_link_libraries(frame colour)
target_link_libraries(frame textbox)
target_link_libraries(frame config)
include_directories(frame)

add_executable( fontreplacement fontreplacement.cpp )
target_link_libraries( fontreplacement ${OpenCV_LIBS} )



target_link_libraries(fontreplacement colour)
target_link_libraries(fontreplacement config)
target_link_libraries(fontreplacement frame)
target_link_libraries(fontreplacement textbox)
target_link_libraries(fontreplacement iniparser)