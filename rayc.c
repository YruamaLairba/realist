#include <stdio.h>
#include <math.h>
#include <string.h>

#include "vec.h"
#include "vecc.h"

enum { SPHCENT, SPHX = SPHCENT, SPHY, SPHZ, SPHRAD, SPHCOL, SPHR = SPHCOL, SPHG, SPHB};
// scene
double sph[][7] = {
    { 0, -0.1, 0, 0.05, 0.8, 0.8, 0.8},
    { 0, 0, 0, 0.05,   0.8, 0.8, 0.8},
    { 0, 0.1, 0,   0.05,   0.8, 0.8, 0.8},
    { 0.1, -0.05, 0,   0.05,   0.8, 0, 0},
    { 0.1, 0.05, 0,    0.05,   0, 0, 0.8},
    { 0.2, 0, 0,   0.05,   0, 0.8, 0},
    { 0.05, -0.05, 0.1,    0.05,   0.8, 0.8, 0.8},
    { 0.05, 0.05, 0.1, 0.05,   0.8, 0.8, 0.8},
    { 0, -0.5, 0.5,    0.02,   1, 1, 0},
};
unsigned nsph = sizeof( sph) / sizeof(sph[0]);

double Intersec( double *sph, const v3 o, const v3 v) {
	double result = HUGE_VAL;
	double m_r = sph[SPHRAD];
	double *m_c = &sph[SPHCENT];
	double sr2 = m_r * m_r;
	double a, b, c;
	v3 vt;
	vsub( vt, o, m_c);
	a = vdot( v, v);
	b = 2 * vdot( v, vt);
	c = vdot( vt, vt) - sr2;
	double t1 = 0, t2 = 0;
	int sol = solvetri( a, b, c, &t1, &t2);
	if (sol >= 1) {
		if (sol > 1) {
			if (t1 < t2) {
				result = t1;
			} else {
				result = t2;
			}
		}
		else {
			result = t1;
		}
	}
	return result;
}

void Trace( const v3 o, const v3 v, v3 color) {
#define TMAX 1E10
	double tmin = HUGE_VAL;
	double *omin = NULL;
//	unsigned imin = 0;
	for (unsigned ii = 0; ii < nsph; ii++) {
		double tres = Intersec( sph[ii], o, v);
		if ((tres > 0) && (tres < tmin)) {
			tmin = tres;
			omin = &sph[ii][SPHCOL];
//			imin = ii;
		}
	}
	if (tmin < TMAX) {
//		printf( "[HIT %u]", imin);
		// intersected object color
		vcopy( color, omin);
	}
}

void Render( unsigned w, unsigned h, char *fnameout) {
	// screen
	double m_ww = 1;
	double m_hh = m_ww * (double)h / (double)w;
	// camera
	v3 cam[] = {
		{ 0.4, 0, 0.4},				// eye
		{ -1, 0, -1},				// front towards screen
		{ -0.707107, 0, 0.707107},	// up along screen
	};
	v3 m_e;	// eye position
	v3 m_f;	// front towards screen
	v3 m_u;	// up along screen
	v3 m_r;	// right along screen (computed)
	int i = 0;
	vcopy( m_e, cam[i++]);
	vcopy( m_f, cam[i++]);
	vcopy( m_u, cam[i++]);

	vnormalize( m_u);
	vcross( m_r, m_f, m_u);		// compute right
	vcross( m_u, m_r, m_f);		// re-compute up
	vnormalize( m_u);
	vnormalize( m_r);
	FILE *fout = stdout;
	if (fnameout) {
		fout = fopen( fnameout, "wb");
	}
	fprintf( fout, "P3\n");
	fprintf( fout, "%d %d\n", w, h);
	fprintf( fout, "%d\n", 100);

	v3 v;
	for (unsigned jj = 0; jj < h; jj++) {
		v3 vu;
		vmult( vu, m_u, ((double)h - (double)jj - 1.0 - (double)h / 2.0) / (double)h * m_hh);
//		vprintn( "rayc vu", vu);
//		getchar();
		for (unsigned ii = 0; ii < w; ii++) {
			v3 vr;
			vmult( vr, m_r, ((double)ii - (double)w / 2.0) / (double)w * m_ww);
			vadd( v, vu, vr);
			vadd( v, v, m_f);
			vnormalize( v);
			v3 color = { 0, 0, 0};
			Trace( m_e, v, color);
			fprintf( fout, "%2.lf %2.lf %2.lf   ", 100*color[0], 100*color[1], 100*color[2]);
		}
		fprintf( fout, "\n");
	}
	if (fout != 0 && fout != stdout) {
		fflush( fout);
		fclose( fout);
	}
}

int main( int argc, char *argv[]) {
	unsigned w = 10, h = 10;
	char *fnameout = 0;
	int arg = 1;
	if (arg < argc) {
		sscanf( argv[arg++], "%d", &w);
		if (arg < argc) {
			sscanf( argv[arg++], "%d", &h);
			if (arg < argc) {
				fnameout = argv[arg++];
			}
		}
	}
	Render( w, h, fnameout);
	return 0;
}
