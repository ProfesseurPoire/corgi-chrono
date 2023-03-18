cmake_minimum_required (VERSION 3.13.0)

project(test-corgi-chrono)

add_executable(${PROJECT_NAME} "") 

# I'm not sure I understand all the advantages on using target_sources but at least
# it's one way to avoid adding variables, and to define our target early
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)

target_link_libraries(${PROJECT_NAME} corgi-chrono corgi-test)

find_package(corgi-test CONFIG)

set_property(TARGET ${PROJECT_NAME}  PROPERTY CXX_STANDARD 20)

add_test( NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
