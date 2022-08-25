cmake_minimum_required (VERSION 3.13)

project ("ebook")

file(GLOB SOURCE
    "src/*.cpp"
    )

add_library(ebook ${SOURCE})

target_include_directories(ebook PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(ebook PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/libmobipocket/include
    ${CMAKE_SOURCE_DIR}/expat/include
    ${CMAKE_SOURCE_DIR}/skia)

target_link_libraries(ebook
    PUBLIC
    mobipocket
    expathtml
    )