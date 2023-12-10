add_executable(client main.c
        game/game.h
        game/game.c
        player/player.h
        player/player.c)
target_link_libraries(client SDL2)
target_link_libraries(client c_engine)
