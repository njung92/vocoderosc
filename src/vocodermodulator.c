#pragma once

#include "fx.h"
#include "volumedetector.h"
#include "fx_multiband.h"
#include "bandvolume.h"

typedef struct {
	int numBands; 
	Fx* mb;
} Vcm;

void* new_bandvolume(int band, void* data) {
	BandVolume* bv = malloc(sizeof(BandVolume));
	bv->band = band;
	bv->data = data;
	return (void*) bv;
}

void* vcm_new(float f_low, float f_high, int numBands, int fs,
	VdCallback bandVolumeCallback, void* callbackdata) {
	Vcm* vcm = malloc(sizeof(Vcm));
	vcm->numBands = numBands;
	Fx* mb = fx_new(mb_filter, mb_free, mb_new(f_low, f_high, numBands, fs));
	for (int i = 0; i < numBands; i++) {
		Fx* vd = fx_new(vd_findVolume, vd_free,
			vd_new(bandVolumeCallback, new_bandvolume(i, callbackdata)));
		mb_addFx(vd, i, mb);
	}
	vcm->mb = mb;
	return (void*)vcm;
}

float vcm_filter(float input, int i, int bufLength, void* data) {
	(Vcm*) vcm = (Vcm*) data;
	return fx_process(vcm->mb, input, i, bufLength);
}

void vcm_free(void* data) {
	(Vcm*) vcm = (Vcm*) data;
	fx_free(vcm->mb);
	free(vcm);
}