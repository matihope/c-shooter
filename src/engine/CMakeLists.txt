add_library(c-engine CNG/window/window.c CNG/window/window.h
        CNG/utils/utils.h
        CNG/shapes/shapes.h
        CNG/math/vector.h
        CNG/CNG.h
        CNG/shapes/shapes.c)
target_include_directories(c-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
