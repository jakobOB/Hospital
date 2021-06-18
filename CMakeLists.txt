set(A2_SOURCE_FILES
    hospital.c
    queue.c
    queue.h
    resources.h
    definitions.h)

add_executable(hospital ${A2_SOURCE_FILES})
target_link_libraries(hospital pthread)
