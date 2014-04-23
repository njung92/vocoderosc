#include <stdlib.h>
#include <stdio.h>
#include "synth.h"
#include "math.h"

Synth* synth_new(int fs, int numvoices) {
	Synth* synth = malloc(sizeof(Synth));
	synth->poly = numvoices;
	synth->saws = malloc(numvoices*sizeof(Saw*));
	int i;
	for (i = 0; i < numvoices; i++) {
		Saw* s_i = saw_new(fs);
		synth->saws[i] = s_i;
	}
	return synth;
}

void synth_on(int key, Synth* s) {
	Saw** saws = s->saws;
	int i;
	for (i = 0; i < s->poly; i++) {
		if (check_key(-1, saws[i])) { // this saw is not active
			saw_on(key, saws[i]);
			printf("%d -> %d    %d\n", i, key, 1);
			// printf("Key: %d    Saw: %d    ON\n", key, i);
			return;
		}
	}
	printf("could not find available saw\n");
	return;
}

void synth_off(int key, Synth* s) {
	Saw** saws = s->saws;
	int i;
	for (i = 0; i < s->poly; i++) {
		if (check_key(key, saws[i])) {
			saw_off(saws[i]);
			printf("%d -> %d    %d\n", i, key, 0);
			// printf("Key: %d    Saw: %d    OFF\n", key, i);
			return;
		}
	}
	printf("couldn't find key %d\n", key);
	return;
}

void synth_free(Synth* s) {
	free(s);
}

float* synth_getBuffer(int bufLength, Synth* s) {
	int i, j;
	Saw** saws = s->saws;
	float* outputBuffer = malloc(bufLength*sizeof(float));
	for (i = 0; i < bufLength; i++) {
		float sample = 0.0;
		for (j = 0; j < s->poly; j++) {
			sample += saw_getNext(saws[j]);
		}
		outputBuffer[i] = sample;
	}
	free(outputBuffer);
	return outputBuffer;
}

// int main() {
// 	Synth* s = synth_new(44100, 12);
// 	int i;
// 	for (i = 1; i <= 12; i++) {
// 		synth_on(i,s);
// 	}
// 	for (i = 1; i <= 12; i++) {
// 		synth_on(i,s);
// 	}
// 	for (i = 1; i <= 12; i++) {
// 		synth_off(i,s);
// 	}
// 	for (i = 1; i <= 12; i++) {
// 		synth_off(i,s);
// 	}
// 	for (i = 1; i <= 12; i++) {
// 		synth_on(i,s);
// 		synth_off(i,s);
// 	}
// 	return 0;
// }