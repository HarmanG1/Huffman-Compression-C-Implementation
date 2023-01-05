CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic -Ofast

all: encode decode

encode: encode.o code.o huffman.o io.o node.o pq.o stack.o 
	$(CC) -o $@ $^ -lm

decode: decode.o code.o huffman.o io.o node.o pq.o stack.o
	$(CC) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o 

spotless: clean
	rm -f encode decode

format:
	clang-format -i -style=file *.[c]
