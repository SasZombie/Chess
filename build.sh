g++ main.cpp -o main -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wmissing-declarations -Winline -Wundef -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic -std=c++23 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3
./main
#-fsanitize=address -fsanitize=leak