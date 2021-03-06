/*------------------------------------------------------------------------------
? A J S Hamilton 2001
------------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "manglefn.h"
#include "defaults.h"

polygon *polys[NPOLYSMAX];

/* getopt options */
const char *optstr = "dqa:b:t:y:m:s:e:v:p:i:o:";

/* local functions */
void	usage(void);
#ifdef	GCC
int	snap(int npoly, polygon *[npoly]);
#else
int	snap(int npoly, polygon *[/*npoly*/]);
#endif

/*------------------------------------------------------------------------------
  Main program.
*/
int main(int argc, char *argv[])
{
    int ifile, nadj, nfiles, npoly, npolys;

    /* default output format */
    fmt.out = keywords[POLYGON];
    /* default is to renumber output polygons with new id numbers */
    fmt.newid = 'n';

    /* parse arguments */
    parse_args(argc, argv);

    /* at least one input and output filename required as arguments */
    if (argc - optind < 2) {
	if (optind > 1 || argc - optind == 1) {
	    fprintf(stderr, "%s requires at least 2 arguments: polygon_infile and polygon_outfile\n", argv[0]);
	    usage();
	    exit(1);
	} else {
	    usage();
	    exit(0);
	}
    }

    msg("---------------- snap ----------------\n");

    /* snap angles */
    scale(&axtol, axunit, 's');
    scale(&btol, bunit, 's');
    scale(&thtol, thunit, 's');
    axunit = 's';
    bunit = 's';
    thunit = 's';
    msg("snap angles: axis %g%c latitude %g%c edge %g%c\n", axtol, axunit, btol, bunit, thtol, thunit);
    scale(&axtol, axunit, 'r');
    scale(&btol, bunit, 'r');
    scale(&thtol, thunit, 'r');
    axunit = 'r';
    bunit = 'r';
    thunit = 'r';

    /* tolerance angle for multiple intersections */
    if (mtol != 0.) {
	scale(&mtol, munit, 's');
	munit = 's';
	msg("multiple intersections closer than %g%c will be treated as coincident\n", mtol, munit);
	scale(&mtol, munit, 'r');
	munit = 'r';
    }

    /* advise data format */
    advise_fmt(&fmt);

    /* read polygons */
    npoly = 0;
    nfiles = argc - 1 - optind;
    for (ifile = optind; ifile < optind + nfiles; ifile++) {
	npolys = rdmask(argv[ifile], &fmt, NPOLYSMAX - npoly, &polys[npoly]);
	if (npolys == -1) exit(1);
	npoly += npolys;
    }
    if (nfiles >= 2) {
	msg("total of %d polygons read\n", npoly);
    }
    if (npoly == 0) {
	msg("STOP\n");
	exit(0);
    }

    /* adjust boundaries of polygons */
    nadj = snap(npoly, polys);

    /* write polygons */
    ifile = argc - 1;
    npoly = wrmask(argv[ifile], &fmt, npoly, polys,0);
    if (npoly == -1) exit(1);

    return(0);
}

/*------------------------------------------------------------------------------
*/
void usage(void)
{
    printf("usage:\n");
    printf("snap [-d] [-q] [-a<a>[u]] [-b<a>[u]] [-t<a>[u]] [-y<r>] [-m<a>[u]] [-s<n>] [-e<n>] [-vo|-vn] [-p[+|-][<n>]] [-i<f>[<n>][u]] [-o<f>[u]] polygon_infile1 [polygon_infile2 ...] polygon_outfile\n");
#include "usage.h"
}

/*------------------------------------------------------------------------------
*/
#include "parse_args.c"

/*------------------------------------------------------------------------------
  Make almost coincident caps of polygons coincide.

  Return value: number of caps adjusted.
*/
int snap(int npoly, polygon *poly[/*npoly*/])
{
#define WARNMAX		8
    int dnadj, dnadjo, i, inull, iprune, j, nadj, pass, snapped, stuck, warn;

    nadj = 0;

    /* snap repeatedly, until no more caps snap together */
    pass = 0;
    stuck = 0;
    do {
	/* snap caps of each pair of polygons in turn, including self-pairs */
	pass++;
	dnadjo = dnadj;
	dnadj = 0;
	warn = 0;
	if (axtol >= 0. || btol >= 0.) {
	    for (i = 0; i < npoly; i++) {
		for (j = i; j < npoly; j++) {
		    snapped = snap_poly(poly[i], poly[j], axtol, btol);
		    if (snapped) {
			if (WARNMAX/2 > 0 && warn == 0)
			    msg("snap stage 1 pass %d: caps of the following polygons were snapped together:\n", pass);
			if (warn < WARNMAX/2) {
			    msg(" (%d %d)", (fmt.newid == 'o')? poly[i]->id : i, (fmt.newid == 'o')? poly[j]->id : j);
			} else if (warn == WARNMAX/2) {
			    msg(" ... more\n");
			}
			dnadj += snapped;
			warn++;
		    }
		}
	    }
	}
	if (WARNMAX/2 > 0 && warn > 0 && warn <= WARNMAX/2) msg("\n");
	msg("snap stage 1 (axes, latitudes) pass %d: %d caps adjusted\n", pass, dnadj);
	nadj += dnadj;
	/* avoid infinite loop */
	if (pass > 1 && dnadj >= dnadjo) stuck++;
    } while (dnadj && stuck < 2);
    if (dnadj) {
	fprintf(stderr, "seem to be stuck in a loop ... exit\n");
    }

    /* trim polygons */
    for (i = 0; i < npoly; i++) {
	trim_poly(poly[i]);
    }

    /* snap repeatedly, until no more caps snap together */
    pass = 0;
    stuck = 0;
    do {
	/* snap edges of each polygon to caps of each polygon in turn */
	pass++;
	dnadjo = dnadj;
	dnadj = 0;
	warn = 0;
	if (thtol >= 0. && ytol >= 0.) {
	    for (i = 0; i < npoly; i++) {
		for (j = 0; j < npoly; j++) {
		    snapped = snap_polyth(poly[i], poly[j], thtol, ytol, mtol);
		    if (snapped) {
			if (WARNMAX/2 > 0 && warn == 0)
			    msg("snap stage 2 pass %d: caps of the following polygons were snapped together:\n", pass);
			if (warn < WARNMAX/2) {
			    msg(" (%d %d)", (fmt.newid == 'o')? poly[i]->id : i, (fmt.newid == 'o')? poly[j]->id : j);
			} else if (warn == WARNMAX/2) {
			    msg(" ... more\n");
			}
			dnadj += snapped;
			warn++;
		    }
		}
	    }
	}
	if (WARNMAX/2 > 0 && warn > 0 && warn <= WARNMAX/2) msg("\n");
	msg("snap stage 2 (edges) pass %d: %d caps adjusted\n", pass, dnadj);
	nadj += dnadj;
	/* avoid infinite loop */
	if (pass > 1 && dnadj >= dnadjo) stuck++;
    } while (dnadj && stuck < 2);
    if (dnadj) {
	fprintf(stderr, "seem to be stuck in a loop ... exit\n");
    }

    /* prune polygons */
    inull = 0;
    for (i = 0; i < npoly; i++) {
	iprune = prune_poly(poly[i], mtol);
	if (iprune >= 2) {
	   if (WARNMAX > 0 && inull == 0)
		msg("warning from snap: the following polygons have zero area:\n");
	   if (inull < WARNMAX) {
		msg(" %d", (fmt.newid == 'o')? poly[i]->id : i);
	   } else if (inull == WARNMAX) {
		msg(" ... more\n");
	   }
	   inull++;
	}
    }
    if (WARNMAX > 0 && inull > 0 && inull <= WARNMAX) msg("\n");
    if (inull > 0) msg("snap: %d snapped polygons have zero area (but are being retained)\n", inull);

    /* assign new polygon id numbers */
    if (fmt.newid == 'n') {
	for (i = 0; i < npoly; i++) {
	    poly[i]->id = i;
	}
    }

    msg("snap: total of %d caps adjusted\n", nadj);

    return(nadj);
}
