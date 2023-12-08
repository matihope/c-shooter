# add_library(server)

add_executable(server main.c
        server/server.h
        server/server.c
        utils/timer.h
        utils/timer.c
        utils/collection.h
        utils/collection.c
)
