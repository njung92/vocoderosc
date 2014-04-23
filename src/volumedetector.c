#include <stdlib.h>
#include "volumedetector.h"

typedef struct {
	void (*callback)(float volume, void* params);
	void* callbackparams;
	float avg;
} Vd;

void* vd_new(void (*callback)(float, void*), void* callbackparams) {
	Vd* v = malloc(sizeof(Vd));
	v->avg = 0;
	v->callback = callback;
	v->callbackparams = callbackparams;
	return (void*)v;
}

float vd_findVolume(float input, int i, int bufLength, void* data) {
	Vd* v = (Vd*) data;
	if(i == bufLength){
		v->callback(v->avg, v->callbackparams);
		v->avg = 0;
	}
	else if (i < bufLength) {
		// printf("before = %f\n", v->avg);
		// printf("input = %f\n", input);
		v->avg += (input * input) / (float)(bufLength);
		// printf("after = %f\n", v->avg);	
	}
	return v->avg;
}

void vd_free(void* data) {
	Vd* v = (Vd*) data;
	free(v->callbackparams);
	free (v);
}

// Testing

// void callback(float volume, void* params) {
// 	printf("volume = %f\n", volume);
// }


// int main() {
// 	void* params;
// 	void* v = vd_new(callback, params);
// 	for (int i = 0; i <= 10; i++) {
// 		float f = 1.0 / (i+1);
// 		vd_findVolume(f, i, 10, v);		
// 	}
// 	return 1;
// }

int main() {
	return 0;
}
