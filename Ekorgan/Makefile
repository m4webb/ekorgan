all: pink spectrum

spectrum: MainAlsa.o MainAlsaSpectrum.o SignalSpectrum.o SignalTree.o
	gcc -lm -lasound MainAlsa.o MainAlsaSpectrum.o SignalSpectrum.o SignalTree.o -o spectrum 

pink: MainAlsa.o MainAlsaPink.o SignalPinkNoise.o SignalTree.o
	gcc -lasound MainAlsa.o MainAlsaPink.o SignalPinkNoise.o SignalTree.o -o pink

MainAlsa.o: MainAlsa.c
	gcc -c MainAlsa.c

MainAlsaPink.o: MainAlsaPink.c
	gcc -c MainAlsaPink.c

MainAlsaSpectrum.o: MainAlsaSpectrum.c
	gcc -c MainAlsaSpectrum.c

SignalSpectrum.o: SignalSpectrum.c
	gcc -c SignalSpectrum.c

SignalPinkNoise.o: SignalPinkNoise.c
	gcc -c SignalPinkNoise.c

SignalTree.o: SignalTree.c
	gcc -c SignalTree.c

clean:
	rm -f *.o pink spectrum
