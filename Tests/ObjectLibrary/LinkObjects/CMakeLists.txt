add_executable(LinkObjects main.c)

# Link TARGET_OBJECTS through LINK_LIBRARIES.
add_library(LinkObjectsAObj OBJECT a_obj.c)
add_library(LinkObjectsADep STATIC a_dep.c)
target_compile_definitions(LinkObjectsAObj PUBLIC OBJA)
target_link_libraries(LinkObjects PRIVATE LinkObjectsADep $<TARGET_OBJECTS:LinkObjectsAObj>)

# Link TARGET_OBJECTS through INTERFACE_LINK_LIBRARIES.
add_library(LinkObjectsB INTERFACE)
add_library(LinkObjectsBObj OBJECT b_obj.c)
add_library(LinkObjectsBDep STATIC b_dep.c)
target_compile_definitions(LinkObjectsBObj PUBLIC OBJB)
target_link_libraries(LinkObjectsB INTERFACE LinkObjectsBObj $<TARGET_OBJECTS:LinkObjectsBObj>)
target_link_libraries(LinkObjectsBObj PRIVATE LinkObjectsBDep)
target_link_libraries(LinkObjects PRIVATE LinkObjectsB)

# Link TARGET_OBJECTS through both LINK_LIBRARIES and INTERFACE_LINK_LIBRARIES, deduplicated.
add_library(LinkObjectsC INTERFACE)
add_library(LinkObjectsCObj OBJECT c_obj.c)
add_library(LinkObjectsCDep STATIC c_dep.c)
target_compile_definitions(LinkObjectsCObj PUBLIC OBJC)
target_link_libraries(LinkObjectsC INTERFACE LinkObjectsCObj $<TARGET_OBJECTS:LinkObjectsCObj>)
target_link_libraries(LinkObjectsCObj PRIVATE LinkObjectsCDep)
target_link_libraries(LinkObjects PRIVATE $<TARGET_OBJECTS:LinkObjectsCObj> LinkObjectsC)

# Link TARGET_OBJECTS through STATIC library private dependency.
add_library(LinkObjectsD INTERFACE)
add_library(LinkObjectsDObj OBJECT d_obj.c)
add_library(LinkObjectsDDep STATIC d_dep.c)
add_library(LinkObjectsDStatic STATIC d_lib.c)
target_compile_definitions(LinkObjectsDObj PUBLIC OBJD)
target_link_libraries(LinkObjectsD INTERFACE LinkObjectsDObj $<TARGET_OBJECTS:LinkObjectsDObj>)
target_link_libraries(LinkObjectsDObj PRIVATE LinkObjectsDDep)
target_link_libraries(LinkObjectsDStatic PRIVATE LinkObjectsD)
target_link_libraries(LinkObjects PRIVATE LinkObjectsDStatic)
