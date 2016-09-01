
INCLUDE=.

CC=/home/mihawk/phoenix/toolchains/mips-gcc472-glibc216/bin/mips-linux-gnu
CFLAGS=--std c99 -Wa,-mmxu -I$(INCLUDE) -lm -O3 -mips32
#CFLAGS=--std c99 -Wa,-mmxu -I$(INCLUDE) -g -pg -O3 -mips32
#CFLAGS=--std c99  -I$(INCLUDE) -g -pg 

test_lstm_forward: test_lstm_forward.c 
	$(CC)-gcc $(CFLAGS)  test_lstm_forward.c -o test_lstm_forward
	$(CC)-strip test_lstm_forward

clean:
	rm test_lstm_forward
