#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "libresample/libresample.h"


#define INPUT_FRAMES_PER_BUFFER				40
#define OUTPUT_FRAMES_PER_BUFFER			240
#define OUTPUT_BUFFER_FRAMES				3840

typedef struct test_resample_t {
	FILE*						inf;
	FILE*						outf;
	double						factor;
	void*						resample;
	short*						rs_inbuf;
	float*						rs_inbuf_f;
	float*						rs_outbuf;
	size_t						rs_outbuf_samples;
	size_t						rs_outbuf_capacity;
} test_resample_t ;

static size_t resample_proc(test_resample_t* self,short* out,size_t samples) {
	size_t s = 0;
	size_t rsamples = 0;
	int last = 0;

	do {
		if( self->rs_outbuf_samples ) {
			s = (self->rs_outbuf_samples > (samples - rsamples) ? (samples - rsamples) : self->rs_outbuf_samples);
			for(size_t i=0; i < s; ++i) {
				out[rsamples + i] = (short)( self->rs_outbuf[i] * 32768 );
			}
			rsamples += s;
			self->rs_outbuf_samples -= s;
			if( self->rs_outbuf_samples ) {
				memmove(self->rs_outbuf,self->rs_outbuf + s,self->rs_outbuf_samples);
				break;
			}
		} else {
			s = fread(self->rs_inbuf,sizeof(short),INPUT_FRAMES_PER_BUFFER,self->inf);
			if( 0 == s )
				break;
			for(size_t i=0; i < s; ++i) {
				self->rs_inbuf_f[i] = (float)self->rs_inbuf[i] / 32768.0f;
			}
			last = (s < INPUT_FRAMES_PER_BUFFER);
			int inbuf_used = 0;

			self->rs_outbuf_samples = resample_process(self->resample,self->factor,
					self->rs_inbuf_f,s,last,&inbuf_used,
					self->rs_outbuf,self->rs_outbuf_capacity);
			assert(inbuf_used == s);
		}
	} while( rsamples < samples );

	return rsamples;
}

int main(int argc,char* argv[]) {
	test_resample_t test;
	test.inf = fopen("pcm_8000_16_1.raw","rb");
	test.outf = fopen("pcm_48000_16_1.raw","wb");

	test.factor = 6.0;
	test.resample = resample_open(0,5.8,6.2);
	test.rs_inbuf = (short*) malloc(INPUT_FRAMES_PER_BUFFER * sizeof(short));
	test.rs_inbuf_f = (float*) malloc(INPUT_FRAMES_PER_BUFFER * sizeof(float));
	test.rs_outbuf_capacity = OUTPUT_FRAMES_PER_BUFFER * 2;
	test.rs_outbuf = (float*) malloc( test.rs_outbuf_capacity * sizeof(float) );
	test.rs_outbuf_samples = 0;

	size_t samples = 0;
	short* out = (short*) malloc(OUTPUT_BUFFER_FRAMES * sizeof(short));

	do {
		samples = resample_proc(&test,out,OUTPUT_BUFFER_FRAMES);
		if( samples ) {
			fwrite(out,sizeof(short),samples,test.outf);
		}
	} while( samples == OUTPUT_BUFFER_FRAMES );

	resample_close(test.resample);
	free(test.rs_inbuf);
	free(test.rs_inbuf_f);
	free(test.rs_outbuf);
	free(out);
	fclose(test.inf);
	fclose(test.outf);
}

