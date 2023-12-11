add_library(c_engine
        src/CNG/graphics/window.c
        include/CNG/graphics/window.h
        src/CNG/common/collection.c
        include/CNG/common/collection.h
        src/CNG/common/timer.c
        include/CNG/common/timer.h
        src/CNG/graphics/shapes.c
        include/CNG/graphics/shapes.h
        include/CNG/common/math.h
        src/CNG/network/server.c
        include/CNG/network/server.h
        src/CNG/network/game_server.c
        include/CNG/network/game_server.h
        include/CNG/network.h
        include/CNG/common.h
        include/CNG/graphics.h
        include/CNG/graphics/base.h
)
target_include_directories(c_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(c_engine SDL2)
