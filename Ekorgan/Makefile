all: pink

pink: MainAlsaPink.o SignalPinkNoise.o SignalTree.o
	gcc -lasound MainAlsaPink.o SignalPinkNoise.o SignalTree.o -o pink

MainAlsaPink.o: MainAlsaPink.c
	gcc -c MainAlsaPink.c

SignalPinkNoise.o: SignalPinkNoise.c
	gcc -c SignalPinkNoise.c

SignalTree.o: SignalTree.c
	gcc -c SignalTree.c

clean:
	rm *.o pink
