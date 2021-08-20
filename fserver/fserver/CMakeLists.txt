cmake_minimum_required(VERSION 3.5)

project(fserver LANGUAGES CXX)

file(WRITE "${CMAKE_SOURCE_DIR}/QtCreatorDeployment.txt" "/home/pi/Downloads")
macro(add_deployment_file SRC DEST)
    file(RELATIVE_PATH path ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    file(APPEND "${CMAKE_SOURCE_DIR}/QtCreatorDeployment.txt" "${path}/${SRC}:${DEST}\n")
endmacro()

macro(add_deployment_directory SRC DEST)
    file(GLOB_RECURSE files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${SRC}/*")
    foreach(filename ${files})
        get_filename_component(path ${filename} PATH)
        add_deployment_file("/home/jinho/Downloads/Example/build-fserver-Raspberry_pi-Debug/fserver" "${DEST}/${path}")
    endforeach(filename)
endmacro()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(fserver main.cpp)
