#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main() { int sock_fd = socket(AF_INET, SOCK_DGRAM, 0); }
