/*------------------------------------------------------------------------------
  ? A J S Hamilton 2001
  ------------------------------------------------------------------------------*/
#include "manglefn.h"

/*------------------------------------------------------------------------------
  Parse arguments.

  Included inline to ensure uniform processing of arguments by all programs.
*/
void parse_args(int argc, char *argv[])
{
  char null = '\0';
  char in, opt, out;
  int iscan, tmpint;

  /* turn off getopt complaints */
  opterr = 0;

  /* parse arguments */
  while (1) {
    opt = getopt(argc, argv, optstr);
    switch (opt) {
    case 'd':		/* advise defaults */
	    printf("%s", argv[0]);
	    if (*optstr) {
        if ((strchr(optstr, 'l')) && LMAX < MAXINT) printf(" -l%d", LMAX);
        if (strchr(optstr, 'g')) printf(" -g%g", LSMOOTH);
        if (strchr(optstr, 'c')) printf(" -c%u", SEED);
        if (strchr(optstr, 'r')) printf(" -r%d", NRANDOM);
        if (strchr(optstr, 'a')) printf(" -a%.15g%c", AXTOL, AXUNIT);
        if (strchr(optstr, 'b')) printf(" -b%.15g%c", BTOL, BUNIT);
        if (strchr(optstr, 't')) printf(" -t%.15g%c", THTOL, THUNIT);
        if (strchr(optstr, 'y')) printf(" -y%.15g", YTOL);
        if (strchr(optstr, 'm')) printf(" -m%.15g%c", MTOL, MUNIT);
        if (strchr(optstr, 's')) printf(" -s%d", SKIP);
        if (strchr(optstr, 'e')) printf(" -e%d", END);
        if (strchr(optstr, 'v')) printf(" -v%c", fmt.newid);
        if (strchr(optstr, 'f')) printf(" -f%.15g,%.15g,%.15g%c", AZN, ELN, AZP, TRUNIT);
        if (strchr(optstr, 'u')) printf(" -u%c,%c", INUNIT, OUTUNIT);
        if (strchr(optstr, 'p')) printf(" -p%c%s", OUTPHASE, "auto");
        if (strchr(optstr, 'i')) {
          if (!fmt.in) {
            printf(" -i?%c", INUNITP);
          } else if (fmt.in[0] == 'e') {
            printf(" -i%c%d%c", fmt.in[0], NVE, INUNITP);
          } else if (fmt.in[0] == 'i' || fmt.in[0] == 'w') {
            printf(" -i%c", fmt.in[0]);
          } else {
            printf(" -i%c%c", fmt.in[0], INUNITP);
          }
        }
        if (strchr(optstr, 'o')) {
          if (fmt.out[0] == 'e' || fmt.out[0] == 'g') {
            printf(" -o%c%d%c", fmt.out[0], NVE, OUTUNITP);
          } else if (fmt.out[0] == 'i' || fmt.out[0] == 'w') {
            printf(" -o%c", fmt.out[0]);
          } else {
            printf(" -o%c%c", fmt.out[0], OUTUNITP);
          }
        }
	    }
	    printf("\n");
	    exit(0);
    case 'q':		/* be quiet */
	    verbose = 0;
	    break;
    case 'w':		/* harmonics file */
	    if (Wlm_filename) free(Wlm_filename);
	    Wlm_filename = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
	    sscanf(optarg, "%s", Wlm_filename);
	    break;
    case 'z':		/* survey, or name of file containing weights */
	    if (survey) free(survey);
	    survey = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
	    sscanf(optarg, "%s", survey);
	    break;
    case 'l':		/* maximum harmonic number */
	    iscan = sscanf(optarg, "%d", &lmax);
	    if (iscan != 1) {
        fprintf(stderr, "-%c%s: expecting integer argument\n", opt, optarg);
        exit(1);
	    }
	    if (lmax < 0) {
        fprintf(stderr, "-%c%s: maximum harmonic number %d must >= 0\n", opt, optarg, lmax);
        exit(1);
	    }
	    break;
    case 'g':		/* smoothing harmonic number */
      /* and smoothing exponent (default 2.) */
	    iscan = sscanf(optarg, "%lg %*[,] %lg", &lsmooth, &esmooth);
	    if (iscan < 1) {
        fprintf(stderr, "-%c%s: expecting real argument\n", opt, optarg);
        exit(1);
	    }
	    break;
    case 'c':		/* seed for random number generator */
	    iscan = sscanf(optarg, "%u", &seed);
	    if (iscan != 1) {
        fprintf(stderr, "-%c%s: expecting unsigned integer argument\n", opt, optarg);
        exit(1);
	    }
	    seed_read = 1;
	    break;
    case 'r':		/* number of random points to generate */
	    iscan = sscanf(optarg, "%d", &nrandom);
	    if (iscan != 1) {
        fprintf(stderr, "-%c%s: expecting integer argument\n", opt, optarg);
        exit(1);
	    }
	    if (nrandom < 1) {
        fprintf(stderr, "-%c%s: number of random points %d should >= 1\n", opt, optarg, nrandom);
        exit(1);
	    }
	    break;
    case 'a':		/* axis tolerance */
	    iscan = sscanf(optarg, "%lg %c", &axtol, &axunit);
	    if (iscan < 1) {
        iscan = sscanf(optarg, " %c", &axunit);
	    }
	    if (!strchr(UNITS, axunit)) {
        fprintf(stderr, "-%c%s: unit %c must be one of %s\n", opt, optarg, axunit, UNITS);
        exit(1);
	    }
	    break;
    case 'b':		/* latitude tolerance */
	    iscan = sscanf(optarg, "%lg %c", &btol, &bunit);
	    if (iscan < 1) {
        iscan = sscanf(optarg, " %c", &bunit);
	    }
	    if (!strchr(UNITS, bunit)) {
        fprintf(stderr, "-%c%s: unit %c must be one of %s\n", opt, optarg, bunit, UNITS);
        exit(1);
	    }
	    break;
    case 't':		/* edge tolerance */
	    iscan = sscanf(optarg, "%lg %c", &thtol, &thunit);
	    if (iscan < 1) {
        iscan = sscanf(optarg, " %c", &thunit);
	    }
	    if (!strchr(UNITS, thunit)) {
        fprintf(stderr, "-%c%s: unit %c must be one of %s\n", opt, optarg, thunit, UNITS);
        exit(1);
	    }
	    break;
    case 'y':		/* edge to length tolerance */
	    iscan = sscanf(optarg, "%lg", &ytol);
	    if (iscan < 1) {
        fprintf(stderr, "-%c%s: expecting real argument\n", opt, optarg);
        exit(1);
	    }
	    break;
    case 'm':		/* multiple intersection tolerance */
	    iscan = sscanf(optarg, "%lg %c", &mtol, &munit);
	    if (iscan < 1) {
        iscan = sscanf(optarg, " %c", &munit);
	    }
	    if (!strchr(UNITS, munit)) {
        fprintf(stderr, "-%c%s: unit %c must be one of %s\n", opt, optarg, munit, UNITS);
        exit(1);
	    }
	    break;
    case 'j':		/* keep weights in interval [min, max] */
	    iscan = sscanf(optarg, "%lg %*[,] %lg", &weight_min, &weight_max);
	    if (iscan < 1) {
        iscan = sscanf(optarg, " %*[,] %lg", &weight_max);
        if (iscan < 1) {
          fprintf(stderr, "-%c%s: expecting -%c<min> or -%c<min>,<max> or -%c,<max>\n", opt, optarg, opt, opt, opt);
          exit(1);
        }
        is_weight_max = 1;
	    } else if (iscan == 1) {
        is_weight_min = 1;
	    } else if (iscan == 2) {
        is_weight_min = 1;
        is_weight_max = 1;
	    }
	    break;
    case 'k':		/* keep areas in interval [min, max] */
	    iscan = sscanf(optarg, "%lg %*[,] %lg", &area_min, &area_max);
	    if (iscan < 1) {
        iscan = sscanf(optarg, " %*[,] %lg", &area_max);
        if (iscan < 1) {
          fprintf(stderr, "-%c%s: expecting -%c<min> or -%c<min>,<max> or -%c,<max>\n", opt, optarg, opt, opt, opt);
          exit(1);
        }
        is_area_max = 1;
	    } else if (iscan == 1) {
        is_area_min = 1;
	    } else if (iscan == 2) {
        is_area_min = 1;
        is_area_max = 1;
	    }
	    break;
    case 'n':		/* take intersection of input polygon files */
	    intersect = 1;
	    break;
		case 'x':		/* read in this file to decide which 
									 set of pairs to balkanize */
	    if (fmt.linklist) free(fmt.linklist);
	    fmt.linklist = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
			sscanf(optarg,"%s",fmt.linklist);
			printf("%s\n",fmt.linklist);
	    break;
		case 'X':		 /* don't output parent list */
      fmt.dontoutputparents=1;
	    break;
		case 'H':		/* read in this file to decide which 
									 set of pairs to balkanize */
	    if (fmt.parents) free(fmt.parents);
	    fmt.parents = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
			sscanf(optarg,"%s",fmt.parents);
			printf("%s\n",fmt.parents);
	    break;
    case 's':		/* skip 1st skip characters of lines of data */
	    iscan = sscanf(optarg, "%d", &tmpint);
      fmt.skip=(size_t) tmpint;
	    if (iscan != 1) {
        fprintf(stderr, "-%c%s: expecting integer argument\n", opt, optarg);
        exit(1);
	    }
	    if (fmt.skip < 0) {
        fprintf(stderr, "-%c%s: number of characters %d to skip must be >= 0\n", opt, optarg, (int) fmt.skip);
        exit(1);
	    }
	    break;
    case 'e':		/* read only up to end'th character of line */
	    iscan = sscanf(optarg, "%d", &tmpint);
      fmt.end=(size_t) tmpint;
	    if (iscan != 1) {
        fprintf(stderr, "-%c%s: expecting integer argument\n", opt, optarg);
        exit(1);
	    }
	    if (fmt.end < 0) {
        fprintf(stderr, "-%c%s: last character number %d to read to must be >= 0\n", opt, optarg, (int) fmt.end);
        exit(1);
	    }
	    break;
    case 'v':		/* apply old|new id numbers to output polygons */
	    iscan = sscanf(optarg, " %c", &fmt.newid);
	    if (!strchr("on", fmt.newid)) {
        if (fmt.newid == '-') {
          fprintf(stderr, "-%c: expecting option o (old id) or n (new id)\n", opt);
        } else {
          fprintf(stderr, "-%c%s: option %c should be o (old id) or n (new id)\n", opt, optarg, fmt.newid);
        }
        exit(1);
	    }
	    break;
    case 'f':		/* angular coordinate frame */
	    /* store argument to -f in fopt, for later parsing */
	    if (fopt) free(fopt);
	    if (optarg) {
        fopt = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
        sscanf(optarg, "%s", fopt);
	    } else {
        fopt = &null;
	    }
	    break;
    case 'u':		/* input, output angular units of az, el data */
	    if (strchr(optarg, ',')) {
        iscan = sscanf(optarg, " %c %*[,] %c", &fmt.inunit, &fmt.outunit);
        if (fmt.inunit == ',') {
          fmt.inunit = INUNIT;
          iscan = sscanf(optarg, " %*[,] %c", &fmt.outunit);
        }
	    } else {
        iscan = sscanf(optarg, " %c %c", &fmt.inunit, &fmt.outunit);
        if (iscan == 1 || !fmt.outunit) fmt.outunit = fmt.inunit;
	    }
	    if (!strchr(UNITS, fmt.inunit)) {
        fprintf(stderr, "-%c%s: input angular unit %c must be one of %s\n", opt, optarg, fmt.inunit, UNITS);
        exit(1);
	    }
	    if (!strchr(UNITS, fmt.outunit)) {
        fprintf(stderr, "-%c%s: output angular unit %c must be one of %s\n", opt, optarg, fmt.outunit, UNITS);
        exit(1);
	    }
	    break;
    case 'p':		/* phase of output azimuth, and number of digits after decimal place in output angles */
	    iscan = sscanf(optarg, " %c", &in);
	    switch (in) {
	    case '-':
	    case '+':
        iscan = sscanf(optarg, " %c %d", &fmt.outphase, &fmt.outprecision);
        break;
	    default:
        iscan = sscanf(optarg, "%d", &fmt.outprecision);
        break;
	    }
	    if (iscan < 1) {
        fprintf(stderr, "-%c%s: expecting [+|-]integer argument\n", opt, optarg);
        exit(1);
	    }
	    break;
    case 'h':		/* write only summary to output */
	    summary = 1;
	    break;
    case 'i':		/* format of input files */
	    sscanf(optarg, " %c", &in);
	    switch (in) {
	    case 'a':		/* input data consist of areas */
        fprintf(stderr, "-%c%s: sorry, area format is implemented only as output, not input\n", opt, optarg);
        exit(1);
	    case 'c':		/* input data consist of circles */
        fmt.in = keywords[CIRCLE];
        fmt.single = 0;
        fmt.n = 0;	/* variable number of circles per line */
        fmt.nn = 3;	/* <az> <el> <rad> */
        break;
	    case 'e':		/* input data consist of edges */
        fmt.in = keywords[EDGES];
        fmt.single = 0;
        fmt.n = 0;	/* variable number of edges per line */
        fmt.innve = NVE;/* NVE points per edge */
        fmt.nn = 2;	/* <az> <el> */
        break;
	    case 'g':		/* input data consist of graphics */
        fprintf(stderr, "-%c%s: sorry, graphics format is implemented only as output, not input ('cos it's ambiguous)\n", opt, optarg);
        exit(1);
	    case 'i':		/* input data consist of ids */
        fprintf(stderr, "-%c%s: sorry, id format is implemented only as output, not input\n", opt, optarg);
        exit(1);
	    case 'm':		/* input data consist of midpoints */
        fprintf(stderr, "-%c%s: sorry, midpoint format is implemented only as output, not input\n", opt, optarg);
        exit(1);
	    case 'p':
	    case 's':
        fmt.in = keywords[POLYGON];
        fmt.single = 1;	/* keyword defines only one polygon */
        break;
	    case 'r':		/* input data consist of rectangles */
        fmt.in = keywords[RECTANGLE];
        fmt.single = 0;
        fmt.n = 1;	/* one rectangle per line */
        fmt.nn = 4;	/* <azmin> <azmax> <elmin> <elmax> */
        break;
	    case 'R':
        fmt.in = keywords[REGION];
        fmt.single = 1;	/* keyword defines only one polygon */
        break;
	    case 'v':		/* input data consist of vertices */
        fmt.in = keywords[VERTICES];
        fmt.single = 0;
        fmt.n = 0;	/* variable number of vertices per line */
        fmt.innve = 1;	/* 1 point per edge */
        fmt.nn = 2;	/* <az> <el> */
        break;
	    case 'w':		/* input data consist of weights */
        fprintf(stderr, "-%c%s: sorry, weight format is implemented only as output, not input\n", opt, optarg);
        exit(1);
	    default:
        fprintf(stderr, "-%c%s: format %c must be one of %s\n", opt, optarg, in, RFMTS);
        exit(1);
        break;
	    }
	    optarg++;
	    if (*optarg) {
        if (in == 'e') {
          iscan = sscanf(optarg, "%d %*[,] %d %c", &fmt.innve, &fmt.n, &fmt.inunitp);
          if (iscan <= 1) {
            iscan = sscanf(optarg, "%d %c", &fmt.innve, &fmt.inunitp);
          }
        } else {
          iscan = sscanf(optarg, "%d %c", &fmt.n, &fmt.inunitp);
        }
        if (iscan <= 0) {
          iscan = sscanf(optarg, " %c", &fmt.inunitp);
        }
        if (in == 'r' && fmt.n != 1) {
          fprintf(stderr, "-%c%c%s: number of rectangles %d per line must be 1\n", opt, in, optarg, fmt.n);
          exit(1);
        } else if (fmt.n < 0) {
          fprintf(stderr, "-%c%c%s: number of objects %d per line must be >= 0\n", opt, in, optarg, fmt.n);
          exit(1);
        }
	    }
	    if (in == 'e' && fmt.innve < 1) {
        fprintf(stderr, "-%c%c%s: number of points %d per edges must be >= 1\n", opt, in, optarg, fmt.innve);
        exit(1);
	    }
	    if (!strchr(UNITS, fmt.inunitp)) {
        fprintf(stderr, "-%c%c%s: input angular unit %c must be one of %s\n", opt, in, optarg, fmt.inunitp, UNITS);
        exit(1);
	    }
	    break;
    case 'o':		/* format of output file */
	    sscanf(optarg, " %c", &out);
	    switch (out) {
	    case 'a':	fmt.out = keywords[AREA];	break;
	    case 'b':	fmt.out = keywords[BINARY];	break;
	    case 'c':	fmt.out = keywords[CIRCLE];	break;
	    case 'e':
        fmt.out = keywords[EDGES];
        fmt.outper = 0;	/* outnve interpreted as number of points/edge */
        break;
	    case 'g':
        fmt.out = keywords[GRAPHICS];
        fmt.outper = 1;	/* outnve interpreted as number of points/(2 pi) */
        break;
	    case 'i':	fmt.out = keywords[ID];		break;
	    case 'm':	fmt.out = keywords[MIDPOINT];	break;
	    case 'p':	fmt.out = keywords[POLYGON];	break;
	    case 'r':	fmt.out = keywords[RECTANGLE];	break;
	    case 'R':	fmt.out = keywords[REGION];	break;
	    case 's':	fmt.out = keywords[SPOLYGON];	break;
	    case 'v':
        fmt.out = keywords[VERTICES];
        fmt.outper = 0;	/* outnve interpreted as number of points/edge */
        fmt.outnve = 1;	/* one point/edge */
        break;
	    case 'w':	fmt.out = keywords[WEIGHT];	break;
	    default:
        fprintf(stderr, "-%c%s: outfile format %c must be one of %s\n", opt, optarg, out, WFMTS);
        exit(1);
	    }
	    optarg++;
	    if (*optarg) {
        iscan = 0;
        if (out == 'e' || out == 'g') {
          iscan = sscanf(optarg, "%d %c", &fmt.outnve, &fmt.outunitp);
        }
        if (iscan <= 0) {
          iscan = sscanf(optarg, " %c", &fmt.outunitp);
        }
	    }
	    if (out == 'e' && fmt.outnve < 0) {
        fprintf(stderr, "-%c%c%s: number of points %d per edge must be >= 0\n", opt, out, optarg, fmt.outnve);
        exit(1);
	    } else if (out == 'g' && fmt.outnve < 0) {
        fprintf(stderr, "-%c%c%s: number of points %d per (2 pi) must be >= 0\n", opt, out, optarg, fmt.outnve);
        exit(1);
	    }
	    if (!strchr(UNITS, fmt.outunitp)) {
        fprintf(stderr, "-%c%c%s: angular unit %c must be one of %s\n", opt, out, optarg, fmt.outunitp, UNITS);
        exit(1);
	    }
	    break;
    case ':':
    case '?':
	    if (optopt == 'f') {
        if (fopt) free(fopt);
        fopt = &null;
	    } else if (strchr(optstr, optopt)) {
        fprintf(stderr, "%s: missing parameter to option -%c\n", argv[0], optopt);
        exit(1);
	    } else {
        fprintf(stderr, "%s: option -%c unknown\n", argv[0], optopt);
        exit(1);
	    }
    case -1:
	    return;
    }
  }
}
