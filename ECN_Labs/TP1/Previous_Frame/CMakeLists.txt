cmake_minimum_required(VERSION 2.8)
project( TP1 )
find_package( OpenCV REQUIRED )
add_executable( main2 main2.cpp )
target_link_libraries( main2 ${OpenCV_LIBS} )


