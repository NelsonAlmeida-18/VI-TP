all:
	gcc -O2 -c -I../../deps/miniz ../../deps/miniz/miniz.c
	g++ -O2 -o rgba2exr -I../common -I../../ -I../../deps/miniz rgba2exr.cc ../../tinyexr.cc miniz.o
