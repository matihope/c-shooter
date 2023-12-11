add_library(c-shooter client_main.c
        game/game.h
        game/game.c
        player/player.h
        player/player.c
        event/event.h
        event/event.c)
target_link_libraries(c-shooter SDL2 c_engine)

add_executable(client client_main.c)
target_link_libraries(client c_engine)

add_executable(server server_main.c)
target_link_libraries(server c_engine)
