# Implementation of Huffman Compression  

These programs implement David Huffman's lossless compression algorithm. It is implemented using sevaral differnt modules that are used to build a huffman tree, encode said tree, dump the tree, rebuild the tree, and decompress the file.

# Building

Run the following to build the 'encode' and 'decode' executables
```
make
```

or 

```
make all
```

Run the following to delete executables and objects

```
make clean
```

# Running encode
```
./encode
```

# Running decode
```
./decode
```

## Files

### encode.c

This file contains my implementation of the Huffman encoder.

- `-h`: Shows help
- `-i infile`: Input containing graph (default: stdin)
- `-o outfile`: Output of computed path (default: stdout)
- `-v`: Enable verbose printing of statistics.

### decode.c

This file contains my implementation of the Huffman decoder.

- `-h`: Shows help
- `-i infile`: Input containing graph (default: stdin)
- `-o outfile`: Output of computed path (default: stdout)
- `-v`: Enable verbose printing of statistics.

### node.c,h

Contains the source and header files for the node ADT

### stack.c,h

Contains the source and header files for the stack ADT

### code.c,h

Contains the source and header file for the code ADT

### pq.c,h

Contains the sorce and header file for the priority queues ADT

### huffman.c,h

Contains the source and header file for the huffman functions 

### Makefile

Makefile contains the commands to complie, clean, and format the files

### header.h

Contains header defines

### defines.h

Contains macros
