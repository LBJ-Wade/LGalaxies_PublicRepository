/*------------------------------------------------------------------------------
  ? A J S Hamilton 2001
  ------------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "manglefn.h"

#define WARNMAX		8
#define	AZEL_STR_LEN	32

/* suppress error messages from garea */
const int verb = 0;

/* initial angular tolerance within which to merge multiple intersections */
extern double mtol;

/* min, max weights to keep */
extern int is_weight_min, is_weight_max;
extern double weight_min, weight_max;

/* min, max areas to keep */
extern int is_area_min, is_area_max;
extern double area_min, area_max;

/*------------------------------------------------------------------------------
  Write mask data.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  fmt = pointer to format structure;
  if null, defaults to polygon format.
  polys = polygons to write.
  npolys = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wrmask(char *filename, format *fmt, int npolys, 
           polygon *polys[/*npolys*/], int selfdestruct)
{
  int npoly;

  /* discard polygons with weight or area outside specified limits */
  npoly = discard_poly(npolys, polys);

  fprintf(stderr,"output format is %s\n", fmt->out);

  /* default polygon format */
  if (!fmt || strcmp(fmt->out, "polygon") == 0
      || strcmp(fmt->out, "spolygon") == 0) {
    npoly = wr_poly(filename, fmt, npolys, polys, npoly, selfdestruct);
      
    /* binary polygon format */
  } else if (!fmt || strcmp(fmt->out, "binary") == 0) {
    npoly = wr_binary(filename, fmt, npolys, polys, npoly, selfdestruct);
  
    /* circle format */
  } else if (strcmp(fmt->out, "circle") == 0) {
    npoly = wr_circ(filename, fmt, npolys, polys, npoly);

    /* edges, graphics, or vertices format */
  } else if (strcmp(fmt->out, "edges") == 0
             || strcmp(fmt->out, "graphics") == 0
             || strcmp(fmt->out, "vertices") == 0) {
    npoly = wr_edge(filename, fmt, npolys, polys, npoly);

    /* rectangle format */
  } else if (strcmp(fmt->out, "rectangle") == 0) {
    npoly = wr_rect(filename, fmt, npolys, polys, npoly);

    /* Region format */
  } else if (strcmp(fmt->out, "Region") == 0) {
    npoly = wr_Reg(filename, npolys, polys, npoly);

    /* area format */
  } else if (strcmp(fmt->out, "area") == 0) {
    npoly = wr_area(filename, fmt, npolys, polys, npoly);

    /* id format */
  } else if (strcmp(fmt->out, "id") == 0) {
    npoly = wr_id(filename, npolys, polys, npoly);

    /* midpoint format */
  } else if (strcmp(fmt->out, "midpoint") == 0) {
    npoly = wr_midpoint(filename, fmt, npolys, polys, npoly);

    /* weight format */
  } else if (strcmp(fmt->out, "weight") == 0) {
    npoly = wr_weight(filename, fmt, npolys, polys, npoly);

  } else {
    fprintf(stderr, "wrmask: format %s not recognized\n", fmt->out);
    npoly = -1;

  }

  return(npoly);
}

/*------------------------------------------------------------------------------
  Write mask data in circle format.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  fmt = pointer to format structure.
  polys = polygons to write.
  npolys = number of polygons.
  npolyw = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wr_circ(char *filename, format *fmt, int npolys, polygon *polys[/*npolys*/], int npolyw)
{
  char unit;
  char az_str[AZEL_STR_LEN], el_str[AZEL_STR_LEN], th_str[AZEL_STR_LEN];
  int i, ier, ip, ipoly, nbadarea, npoly;
  double area, angle[3], tol;
  FILE *file;
  char *circle_fmt = "circle %d ( %d caps, %.15lg weight, %.15lf str):\n";

  /* open filename for writing */
  if (!filename || strcmp(filename, "-") == 0) {
    file = stdout;
  } else {
    file = fopen(filename, "w");
    if (!file) {
      fprintf(stderr, "wr_circ: cannot open %s for writing\n", filename);
      return(-1);
    }
  }

  /* write number of polygons */
  fprintf(file, "%d polygons\n", npolyw);

  /* write angular unit */
  fprintf(file, "unit %c\n", fmt->outunitp);

  npoly = 0;
  nbadarea = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    /* discard null polygons */
    if (!polys[ipoly]) continue;
    /* area of polygon */
    tol = mtol;
    ier = garea(polys[ipoly], &tol, verb, &area);
    if (ier == -1) return(-1);
    if (ier) {
      fprintf(stderr, "wr_circ: area of polygon %d is incorrect\n", polys[ipoly]->id);
      nbadarea++;
    }
    /* number of caps, weight, and area of polygon */
    fprintf(file, circle_fmt,
            polys[ipoly]->id, polys[ipoly]->np, polys[ipoly]->weight, area);
    /* write boundaries of polygon */
    for (ip = 0; ip < polys[ipoly]->np; ip++) {
      rpcm_to_circ(polys[ipoly]->rp[ip], &polys[ipoly]->cm[ip], angle);
      switch (fmt->outphase) {
      case '+':	if (angle[0] < 0.) angle[0] += TWOPI;	break;
      case '-':	if (angle[0] > PI) angle[0] -= TWOPI;	break;
      }
      for (i = 0; i < 3; i++) {
        unit = fmt->outunitp;
        if (i > 0 && fmt->outunitp == 'h') unit = 'd';
        scale(&angle[i], 'r', unit);
      }
      wrangle(angle[0], fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, az_str);
      wrangle(angle[1], fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, el_str);
      wrangle(angle[2], (fmt->outunitp == 'h')? 'd' : fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, th_str);
      fprintf(file, " %s %s %s", az_str, el_str, th_str);
    }
    fprintf(file, "\n");
    /* increment polygon count */
    npoly++;
  }

  /* warn about polygons with incorrect area */
  if (nbadarea > 0) {
    msg("%d polygons have incorrect area, but kept\n", nbadarea);
  }

  /* advise */
  msg("%d polygons written to %s\n",
      npoly, (file == stdout)? "output": filename);

  /* close file */
  if (file != stdout) fclose(file);

  return(npoly);
}

/*------------------------------------------------------------------------------
  Write mask data in edges or vertices format.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  fmt = pointer to format structure.
  polys = polygons to write.
  npolys = number of polygons.
  npolyw = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wr_edge(char *filename, format *fmt, int npolys, polygon *polys[/*npolys*/], int npolyw)
{
  const int per = 0;
  const int nve = 2;
  const char *edges_fmt = "edges %d ( %d points/edge, %d edges, %.15lg weight, %s %s mid):\n";
  const char *graphics_fmt = "graphics %d ( %d points, %d edges, %.15lg weight, %s %s mid):\n";
  const char *vertices_fmt = "vertices %d ( %d vertices, %.15lg weight, %s %s mid):\n";
  char az_str[AZEL_STR_LEN], el_str[AZEL_STR_LEN];
  int do_vcirc, i, ier, imid, ipoly, iv, ive, ivm, jv, manybounds, nbadverts, nev, nev0, npoly, npt, nv, nvm;
  int *ipv, *gp, *ev;
  double azo, tol;
  double *angle;
  vec *ve, *vm;
  azel v;
  FILE *file;

  /* open filename for writing */
  if (!filename || strcmp(filename, "-") == 0) {
    file = stdout;
  } else {
    file = fopen(filename, "w");
    if (!file) {
	    fprintf(stderr, "wr_edge: cannot open %s for writing\n", filename);
	    return(-1);
    }
  }

  /* whether to write vertices also for circles with no intersections */
  if (strcmp(fmt->out, "vertices") == 0) {
    do_vcirc = 0;
  } else {
    do_vcirc = 1;
  }

  /* write number of polygons */
  fprintf(file, "%d polygons\n", npolyw);

  /* write angular unit */
  fprintf(file, "unit %c\n", fmt->outunitp);

  manybounds = 0;
  npoly = 0;
  nbadverts = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    /* discard null polygons */
    if (!polys[ipoly]) continue;

    /* point somewhere in the middle of the polygon */
    tol = mtol;
    ier = gverts(polys[ipoly], do_vcirc, &tol, per, nve, &nv, &ve, &angle, &ipv, &gp, &nev, &nev0, &ev);
    if (ier == -1) return(-1);
    imid = vmid(polys[ipoly], tol, nv, nve, ve, ipv, ev, &nvm, &vm);
    if (imid == -1) return(-1);
    /* check found a point inside the polygon */
    imid = 0;
    for (ivm = 0; ivm < nvm; ivm++) {
	    if (vm[ivm][0] != 0. || vm[ivm][1] != 0. || vm[ivm][2] != 0.) {
        imid = 1;
        if (ivm > 0) for (i = 0; i < 3; i++) vm[0][i] = vm[ivm][i];
        break;
	    }
    }
    /* found a point */
    if (imid == 1) {
	    rp_to_azel(vm[0], &v);
	    switch (fmt->outphase) {
	    case '+':	if (v.az < 0.) v.az += TWOPI;	break;
	    case '-':	if (v.az > PI) v.az -= TWOPI;	break;
	    }
	    scale_azel(&v, 'r', fmt->outunitp);
    }

    /* points on edges of polygon */
    tol = mtol;
    ier = gverts(polys[ipoly], do_vcirc, &tol, fmt->outper, fmt->outnve, &nv, &ve, &angle, &ipv, &gp, &nev, &nev0, &ev);
    if (ier == -1) return(-1);
    if (ier) {
	    nbadverts++;
	    continue;
    }

    /* warn about multi-boundary polygon */
    if (nev > 1) {
	    if (WARNMAX > 0 && manybounds == 0) {
        msg("the following polygons have > 1 boundary (not simply-connected)\n");
        msg("   separate boundaries will be split over separate lines:\n");
	    }
	    if (manybounds < WARNMAX) {
        msg(" %d", polys[ipoly]->id);
	    } else if (manybounds == WARNMAX) {
        msg(" ... more\n");
	    }
	    manybounds++;
    }

    /* count number of points */
    npt = 0;
    for (iv = jv = 0; iv < nv; jv++) {
	    for (; iv < ev[jv]; iv++) {
        for (ive = 0; ive < fmt->outnve; ive++) {
          i = iv * fmt->outnve + ive;
          if (ve[i][0] == 0. && ve[i][1] == 0. && ve[i][2] == 0.) break;
          npt++;
        }
	    }
    }

    /* number of edges, weight, and midpoint of polygon */
    wrangle(v.az, fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, az_str);
    wrangle(v.el, fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, el_str);
    if (strcmp(fmt->out, "edges") == 0) {
	    fprintf(file, edges_fmt,
              polys[ipoly]->id, fmt->outnve, nv, polys[ipoly]->weight, az_str, el_str);
    } else if (strcmp(fmt->out, "graphics") == 0) {
	    fprintf(file, graphics_fmt,
              polys[ipoly]->id, npt, nv, polys[ipoly]->weight, az_str, el_str);
    } else {
	    fprintf(file, vertices_fmt,
              polys[ipoly]->id, nv, polys[ipoly]->weight, az_str, el_str);
    }

    /* write points, splitting separate boundaries over separate lines */
    for (iv = jv = 0; iv < nv; jv++) {
	    for (; iv < ev[jv]; iv++) {
        for (ive = 0; ive < fmt->outnve; ive++) {
          i = iv * fmt->outnve + ive;
          if (ve[i][0] == 0. && ve[i][1] == 0. && ve[i][2] == 0.) break;
          /* convert unit vector to azel vertex */
          rp_to_azel(ve[i], &v);
          /* set azimuth of first point */
          if (iv == 0 && ive == 0) {
            switch (fmt->outphase) {
            case '+':	if (v.az < 0.) v.az += TWOPI;	break;
            case '-':	if (v.az > PI) v.az -= TWOPI;	break;
            }
            /* phase azimuth of each subsequent point to the previous point */
          } else {
            v.az -= rint((v.az - azo) / TWOPI) * TWOPI;
          }
          azo = v.az;
          scale_azel(&v, 'r', fmt->outunitp);
          wrangle(v.az, fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, az_str);
          wrangle(v.el, fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, el_str);
          fprintf(file, " %s %s", az_str, el_str);
        }
	    }
	    fprintf(file, "\n");
    }
    /* increment polygon count */
    npoly++;
  }
  /* warn about multi-boundary polygon */
  if (WARNMAX > 0 && manybounds > 0 && manybounds <= WARNMAX) msg("\n");
  if (manybounds > 0) msg("%d polygons had more than one boundary (not simply-connected)\n", manybounds);

  /* warn about polygons producing fatal error */
  if (nbadverts > 0) {
    msg("%d polygons producing fatal error in gvert discarded\n");
  }

  /* advise */
  msg("%d polygons written to %s\n",
      npoly, (file == stdout)? "output": filename);

  /* close file */
  if (file != stdout) fclose(file);

  return(npoly);
}

/*------------------------------------------------------------------------------
  Write mask data in rectangle format.
  Only polygons which are rectangles are written: other polygons are discarded.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  fmt = pointer to format structure.
  polys = polygons to write.
  npolys = number of polygons.
  npolyw = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wr_rect(char *filename, format *fmt, int npolys, polygon *polys[/*npolys*/], int npolyw)
{
  char unit;
  char azmin_str[AZEL_STR_LEN], azmax_str[AZEL_STR_LEN], elmin_str[AZEL_STR_LEN], elmax_str[AZEL_STR_LEN];
  int ier, ipoly, isrect, nbadarea, nrect;
  double area, azmin, azmax, elmin, elmax, tol;
  FILE *file;
  char *rect_fmt = "rectangle %d ( %d caps, %.15lg weight, %.15lf str):\n";

  /* count how many polygons are rectangles */
  nrect = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    if (!polys[ipoly]) continue;
    isrect = poly_to_rect(polys[ipoly], &azmin, &azmax, &elmin, &elmax);
    if (isrect) nrect++;
  }

  /* no rectangles */
  if (nrect == 0) {
    msg("there are no rectangles among the %d polygons\n", npolyw);
    return(0);
  } else if (nrect < npolyw) {
    msg("%d of %d polygons are rectangles; discarding %d non-rectangle polygons\n", nrect, npolyw, npolyw - nrect);
  } else if (nrect == npolyw) {
    msg("all %d polygons are rectangles\n", npolyw);
  }

  /* open filename for writing */
  if (!filename || strcmp(filename, "-") == 0) {
    file = stdout;
  } else {
    file = fopen(filename, "w");
    if (!file) {
	    fprintf(stderr, "wr_rect: cannot open %s for writing\n", filename);
	    return(-1);
    }
  }

  /* write number of rectangles */
  fprintf(file, "%d rectangles\n", nrect);

  /* write angular unit */
  fprintf(file, "unit %c\n", fmt->outunitp);

  nrect = 0;
  nbadarea = 0;
  /* write rectangles */
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    /* discard null polygons */
    if (!polys[ipoly]) continue;

    /* is polygon a rectangle? */
    isrect = poly_to_rect(polys[ipoly], &azmin, &azmax, &elmin, &elmax);
    /* skip polygons that are not rectangles */
    if (!isrect) continue;

    /* set phase of azimuth */
    switch (fmt->outphase) {
    case '+':	if (azmax < 0.) azmin += TWOPI, azmax += TWOPI;	break;
    case '-':	if (azmax > PI) azmin -= TWOPI, azmax -= TWOPI;	break;
    }

    /* area of polygon */
    tol = mtol;
    ier = garea(polys[ipoly], &tol, verb, &area);
    if (ier == -1) return(-1);
    if (ier) {
	    fprintf(stderr, "wr_rect: area of polygon %d is incorrect\n", polys[ipoly]->id);
	    nbadarea++;
    }

    /* number of caps, weight, and area of polygon */
    fprintf(file, rect_fmt,
            polys[ipoly]->id, polys[ipoly]->np, polys[ipoly]->weight, area);

    /* scale angles to desired units */
    unit = fmt->outunitp;
    scale(&azmin, 'r', unit);
    scale(&azmax, 'r', unit);
    if (fmt->outunitp == 'h') unit = 'd';
    scale(&elmin, 'r', unit);
    scale(&elmax, 'r', unit);

    /* write rectangle */
    unit = fmt->outunitp;
    wrangle(azmin, fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, azmin_str);
    wrangle(azmax, fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, azmax_str);
    wrangle(elmin, fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, elmin_str);
    wrangle(elmax, fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, elmax_str);
    fprintf(file, " %s %s %s %s\n", azmin_str, azmax_str, elmin_str, elmax_str);

    /* increment rectangle count */
    nrect++;
  }

  /* warn about rectangles with incorrect area */
  if (nbadarea > 0) {
    msg("%d rectangles have incorrect area, but kept\n", nbadarea);
  }

  /* advise */
  msg("%d rectangles written to %s\n",
      nrect, (file == stdout)? "output": filename);

  /* close file */
  if (file != stdout) fclose(file);

  return(nrect);
}

/*------------------------------------------------------------------------------
  Write mask data in polygon format.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  fmt = pointer to format structure.
  polys = polygons to write.
  npolys = number of polygons.
  npolyw = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wr_poly(char *filename, format *fmt, int npolys, polygon *polys[/*npolys*/], int npolyw, int selfdestruct)
{
  int ier, ip, ipoly, nbadarea, npoly;
  double area, tol;
  FILE *file;
  char *poly_fmt;
  char *polygon_fmt = "polygon %d ( %d caps, %.15lg weight, %.15lf str):\n";
  char *spolygon_fmt = "%d %d %.15lg %.15lf\n";

  /* open filename for writing */
  if (!filename || strcmp(filename, "-") == 0) {
    file = stdout;
  } else {
    file = fopen(filename, "w");
    if (!file) {
	    fprintf(stderr, "wr_poly: cannot open %s for writing\n", filename);
	    return(-1);
    }
  }

  /* format */
  if (fmt && strcmp(fmt->out, "spolygon") == 0) {
    poly_fmt = spolygon_fmt;
  } else {
    poly_fmt = polygon_fmt;
  }

  /* write number of polygons */
  fprintf(file, "%d polygons\n", npolyw);

  npoly = 0;
  nbadarea = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    /* discard null polygons */
    if (!polys[ipoly]) continue;

    /* area of polygon */
    tol = mtol;
    ier = garea(polys[ipoly], &tol, verb, &area);
    if (ier == -1) return(-1);
    if (ier) {
	    fprintf(stderr, "wr_poly: area of polygon %d is incorrect\n", polys[ipoly]->id);
	    nbadarea++;
    }

    /* number of caps, weight, and area of polygon */
    fprintf(file, poly_fmt,
            polys[ipoly]->id, polys[ipoly]->np, polys[ipoly]->weight, area);

    /* write boundaries of polygon */
    for (ip = 0; ip < polys[ipoly]->np; ip++) {
	    fprintf(file, " %19.16f %19.16f %19.16f %.16g\n",
              polys[ipoly]->rp[ip][0], polys[ipoly]->rp[ip][1], polys[ipoly]->rp[ip][2], polys[ipoly]->cm[ip]);
    }
    if(selfdestruct==1) free_poly(polys[ipoly]);

    /* increment polygon count */
    npoly++;
  }

  /* warn about polygons with incorrect area */
  if (nbadarea > 0) {
    msg("%d polygons have incorrect area, but kept\n", nbadarea);
  }

  /* advise */
  msg("%d polygons written to %s\n",
      npoly, (file == stdout)? "output": filename);

  /* close file */
  if (file != stdout) fclose(file);

  return(npoly);

}
/*------------------------------------------------------------------------------
  Write mask data in polygon binary format.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  fmt = pointer to format structure.
  polys = polygons to write.
  npolys = number of polygons.
  npolyw = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wr_binary(char *filename, format *fmt, int npolys,  
              polygon *polys[/*npolys*/], int npolyw, int selfdestruct)
{
  int ier, ip, ipoly, nbadarea, npoly;
  double area, tol;
  FILE *file;
    
  /* open filename for writing */
  if (!filename || strcmp(filename, "-") == 0) {
    file = stdout;
  } else {
    file = fopen(filename, "w");
    if (!file) {
      fprintf(stderr, "wr_binary: cannot open %s for writing\n", filename);
      return(-1);
    }
  }
    
  /* write number of polygons */
  fwrite((void *) (&npolyw), sizeof(npolyw), 1, file);

  npoly = 0;
  nbadarea = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    /* discard null polygons */
    if (!polys[ipoly]) continue;
      
    /* area of polygon */
    tol = mtol;
    ier = garea(polys[ipoly], &tol, verb, &area);
    if (ier == -1) return(-1);
    if (ier) {
      fprintf(stderr, "wr_binary: area of polygon %d is incorrect\n", 
              polys[ipoly]->id);
      nbadarea++;
    }

    /* number of caps, weight, and area of polygon */
    fwrite((void *) (&(polys[ipoly]->id)), sizeof(polys[ipoly]->id), 
           1, file);
    fwrite((void *) (&(polys[ipoly]->np)), sizeof(polys[ipoly]->np), 
           1, file);
    fwrite((void *) (&(polys[ipoly]->weight)), sizeof(polys[ipoly]->weight), 
           1, file);
    fwrite((void *) (&area), sizeof(area), 1, file);
      
    /* write boundaries of polygon */
    for (ip = 0; ip < polys[ipoly]->np; ip++) 
      fwrite((void *) polys[ipoly]->rp[ip], sizeof(polys[ipoly]->rp[ip]), 1, 
             file);
    for (ip = 0; ip < polys[ipoly]->np; ip++) 
      fwrite((void *) (&(polys[ipoly]->cm[ip])), 
             sizeof(polys[ipoly]->cm[ip]), 1, file);
      
    if(selfdestruct==1) free_poly(polys[ipoly]);
      
    /* increment polygon count */
    npoly++;
  }

  /* warn about polygons with incorrect area */
  if (nbadarea > 0) {
    msg("%d polygons have incorrect area, but kept\n", nbadarea);
  }

  /* advise */
  msg("%d polygons written to %s\n",
      npoly, (file == stdout)? "output": filename);
    
  /* close file */
  if (file != stdout) fclose(file);
    
  return(npoly);
}

/*------------------------------------------------------------------------------
  Write mask data in Max Tegmark's Region format.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  polys = polygons to write.
  npolys = number of polygons.
  npolyw = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wr_Reg(char *filename, int npolys, polygon *polys[/*npolys*/], int npolyw)
{
  int ip, ipoly, npoly;
  FILE *file;
  /* there are no holes in our treatment */
  int nholes = 0;
  char *Region_fmt = " Region %d ( %d caps, %d holes):\n";

  /* open filename for writing */
  if (!filename || strcmp(filename, "-") == 0) {
    file = stdout;
  } else {
    file = fopen(filename, "w");
    if (!file) {
	    fprintf(stderr, "wr_Reg: cannot open %s for writing\n", filename);
	    return(-1);
    }
  }

  /* write number of polygons */
  fprintf(file, " %d\n", npolyw);

  /* write number of caps */
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    if (!polys[ipoly]) continue;
    fprintf(file, " %d", polys[ipoly]->np);
    if ((ipoly + 1) % 40 == 0 || ipoly == npolys - 1) fprintf(file, "\n");
  }

  /* write number of holes */
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    if (!polys[ipoly]) continue;
    fprintf(file, " %d", nholes);
    if ((ipoly + 1) % 40 == 0 || ipoly == npolys - 1) fprintf(file, "\n");
  }

  /* write polygons */
  npoly = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    /* discard null polygons */
    if (!polys[ipoly]) continue;

    /* blank line */
    fprintf(file, "\n");

    /* id line */
    fprintf(file, Region_fmt,
            polys[ipoly]->id, polys[ipoly]->np, nholes);

    /* write boundaries of polygon */
    for (ip = 0; ip < polys[ipoly]->np; ip++) {
	    fprintf(file, " %19.16f %19.16f %19.16f %19.16f\n",
              polys[ipoly]->rp[ip][0], polys[ipoly]->rp[ip][1], polys[ipoly]->rp[ip][2], polys[ipoly]->cm[ip]);
    }

    /* increment polygon count */
    npoly++;
  }

  /* advise */
  msg("%d polygons written to %s\n",
      npoly, (file == stdout)? "output": filename);

  /* close file */
  if (file != stdout) fclose(file);

  return(npoly);
}

/*------------------------------------------------------------------------------
  Write areas.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  fmt = pointer to format structure.
  polys = polygons to write.
  npolys = number of polygons.
  npolyw = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wr_area(char *filename, format *fmt, int npolys, polygon *polys[/*npolys*/], int npolyw)
{
#undef	PRECISION
#define	PRECISION	15
  int idmin, idmax, idwidth, ier, ipoly, nbadarea, npoly, precision, width;
  double area, tol;
  FILE *file;

  /* open filename for writing */
  if (!filename || strcmp(filename, "-") == 0) {
    file = stdout;
  } else {
    file = fopen(filename, "w");
    if (!file) {
	    fprintf(stderr, "wr_area: cannot open %s for writing\n", filename);
	    return(-1);
    }
  }

  /* largest width of polygon id number */
  idmin = 0;
  idmax = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    if (!polys[ipoly]) continue;
    if (polys[ipoly]->id < idmin) idmin = polys[ipoly]->id;
    if (polys[ipoly]->id > idmax) idmax = polys[ipoly]->id;
  }
  idmin = ((idmin < 0)? floor(log10((double)-idmin)) + 2 : 1);
  idmax = ((idmax > 0)? floor(log10((double)idmax)) + 1 : 1);
  idwidth = ((idmin > idmax)? idmin : idmax);

  /* width of area in steradians */
  precision = (fmt->outprecision >= 0)? fmt->outprecision : PRECISION;
  width = precision + 3;
  if (precision == 0) width--;

  /* write header */
  fprintf(file, "area of %d polygons\n", npolyw);
  fprintf(file, "%*s %*s\n", width, "area(str)", idwidth, "id");

  npoly = 0;
  nbadarea = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    /* discard null polygons */
    if (!polys[ipoly]) continue;

    /* area of polygon */
    tol = mtol;
    ier = garea(polys[ipoly], &tol, verb, &area);
    if (ier == -1) return(-1);
    if (ier) {
	    fprintf(stderr, "wr_poly: area of polygon %d is incorrect\n", polys[ipoly]->id);
	    nbadarea++;
    }

    /* write area */
    fprintf(file, "% *.*f %*d\n", width, precision, area, idwidth, polys[ipoly]->id);

    /* increment polygon count */
    npoly++;
  }

  /* warn about polygons with incorrect area */
  if (nbadarea > 0) {
    msg("%d polygons have incorrect area, but kept\n", nbadarea);
  }

  /* advise */
  msg("%d areas written to %s\n",
      npoly, (file == stdout)? "output": filename);

  /* close file */
  if (file != stdout) fclose(file);

  return(npoly);
}

/*------------------------------------------------------------------------------
  Write ids.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  polys = polygons to write.
  npolys = number of polygons.
  npolyw = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wr_id(char *filename, int npolys, polygon *polys[/*npolys*/], int npolyw)
{
  int idmin, idmax, idwidth, ipoly, npoly;
  FILE *file;

  /* open filename for writing */
  if (!filename || strcmp(filename, "-") == 0) {
    file = stdout;
  } else {
    file = fopen(filename, "w");
    if (!file) {
	    fprintf(stderr, "wr_id: cannot open %s for writing\n", filename);
	    return(-1);
    }
  }

  /* largest width of polygon id number */
  idmin = 0;
  idmax = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    if (!polys[ipoly]) continue;
    if (polys[ipoly]->id < idmin) idmin = polys[ipoly]->id;
    if (polys[ipoly]->id > idmax) idmax = polys[ipoly]->id;
  }
  idmin = ((idmin < 0)? floor(log10((double)-idmin)) + 2 : 1);
  idmax = ((idmax > 0)? floor(log10((double)idmax)) + 1 : 1);
  idwidth = ((idmin > idmax)? idmin : idmax);

  /* write header */
  fprintf(file, "id of %d polygons\n", npolyw);
  fprintf(file, "%*s\n", idwidth, "id");

  npoly = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    /* discard null polygons */
    if (!polys[ipoly]) continue;

    /* write id */
    fprintf(file, "%*d\n", idwidth, polys[ipoly]->id);

    /* increment polygon count */
    npoly++;
  }

  /* advise */
  msg("%d ids written to %s\n",
      npoly, (file == stdout)? "output": filename);

  /* close file */
  if (file != stdout) fclose(file);

  return(npoly);
}

/*------------------------------------------------------------------------------
  Write midpoints.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  fmt = pointer to format structure.
  polys = polygons to write.
  npolys = number of polygons.
  npolyw = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wr_midpoint(char *filename, format *fmt, int npolys, polygon *polys[/*npolys*/], int npolyw)
{
  const int per = 0;
  const int nve = 2;
  const int do_vcirc = 0;
  char az_str[AZEL_STR_LEN], el_str[AZEL_STR_LEN];
  int i, idmin, idmax, idwidth, ier, imid, ipoly, ivm, nev, nev0, npoly, nv, nvm, width;
  int *ipv, *gp, *ev;
  double tol;
  double *angle;
  vec *ve, *vm;
  azel v;
  FILE *file;

  /* open filename for writing */
  if (!filename || strcmp(filename, "-") == 0) {
    file = stdout;
  } else {
    file = fopen(filename, "w");
    if (!file) {
	    fprintf(stderr, "wr_midpoint: cannot open %s for writing\n", filename);
	    return(-1);
    }
  }

  /* largest width of polygon id number */
  idmin = 0;
  idmax = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    if (!polys[ipoly]) continue;
    if (polys[ipoly]->id < idmin) idmin = polys[ipoly]->id;
    if (polys[ipoly]->id > idmax) idmax = polys[ipoly]->id;
  }
  idmin = ((idmin < 0)? floor(log10((double)-idmin)) + 2 : 1);
  idmax = ((idmax > 0)? floor(log10((double)idmax)) + 1 : 1);
  idwidth = ((idmin > idmax)? idmin : idmax);

  /* write header */
  wrangle(0., fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, az_str);
  width = strlen(az_str);
  if (fmt->outunitp == 'h') {
    sprintf(az_str, "az(hms)");
    sprintf(el_str, "el(dms)");
  } else {
    sprintf(az_str, "az(%c)", fmt->outunitp);
    sprintf(el_str, "el(%c)", fmt->outunitp);
  }
  fprintf(file, "midpoint of %d polygons\n", npolyw);
  fprintf(file, "%*s %*s %*s\n", width, az_str, width, el_str, idwidth, "id");

  npoly = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    /* discard null polygons */
    if (!polys[ipoly]) continue;

    /* point somewhere in the middle of the polygon */
    tol = mtol;
    ier = gverts(polys[ipoly], do_vcirc, &tol, per, nve, &nv, &ve, &angle, &ipv, &gp, &nev, &nev0, &ev);
    if (ier == -1) return(-1);
    imid = vmid(polys[ipoly], tol, nv, nve, ve, ipv, ev, &nvm, &vm);
    if (imid == -1) return(-1);
    /* check found a point inside the polygon */
    imid = 0;
    for (ivm = 0; ivm < nvm; ivm++) {
	    if (vm[ivm][0] != 0. || vm[ivm][1] != 0. || vm[ivm][2] != 0.) {
        imid = 1;
        if (ivm > 0) for (i = 0; i < 3; i++) vm[0][i] = vm[ivm][i];
        break;
	    }
    }
    /* found a point */
    if (imid == 1) {
	    rp_to_azel(vm[0], &v);
	    switch (fmt->outphase) {
	    case '+':	if (v.az < 0.) v.az += TWOPI;	break;
	    case '-':	if (v.az > PI) v.az -= TWOPI;	break;
	    }
	    scale_azel(&v, 'r', fmt->outunitp);
    }

    /* write midpoint of polygon */
    wrangle(v.az, fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, az_str);
    wrangle(v.el, fmt->outunitp, fmt->outprecision, AZEL_STR_LEN, el_str);
    fprintf(file, "%s %s %*d\n", az_str, el_str, idwidth, polys[ipoly]->id);

    /* increment polygon count */
    npoly++;
  }

  /* advise */
  msg("%d midpoints written to %s\n",
      npoly, (file == stdout)? "output": filename);

  /* close file */
  if (file != stdout) fclose(file);

  return(npoly);
}

/*------------------------------------------------------------------------------
  Write weights.

  Input: filename = name of file to write to;
  "" or "-" means write to standard output.
  fmt = pointer to format structure.
  polys = polygons to write.
  npolys = number of polygons.
  npolyw = number of polygons to write.
  Return value: number of polygons written,
  or -1 if error occurred.
*/
int wr_weight(char *filename, format *fmt, int npolys, polygon *polys[/*npolys*/], int npolyw)
{
#undef	PRECISION
#define	PRECISION	6
  int idmin, idmax, idwidth, ipoly, npoly, precision, width;
  double weightmax;
  FILE *file;

  /* open filename for writing */
  if (!filename || strcmp(filename, "-") == 0) {
    file = stdout;
  } else {
    file = fopen(filename, "w");
    if (!file) {
	    fprintf(stderr, "wr_weight: cannot open %s for writing\n", filename);
	    return(-1);
    }
  }

  /* largest width of polygon id number */
  idmin = 0;
  idmax = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    if (!polys[ipoly]) continue;
    if (polys[ipoly]->id < idmin) idmin = polys[ipoly]->id;
    if (polys[ipoly]->id > idmax) idmax = polys[ipoly]->id;
  }
  idmin = ((idmin < 0)? floor(log10((double)-idmin)) + 2 : 1);
  idmax = ((idmax > 0)? floor(log10((double)idmax)) + 1 : 1);
  idwidth = ((idmin > idmax)? idmin : idmax);

  /* largest width of weight */
  weightmax = 0.;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    if (!polys[ipoly]) continue;
    if (weightmax < fabs(polys[ipoly]->weight)) weightmax = fabs(polys[ipoly]->weight);
  }
  precision = (fmt->outprecision >= 0)? fmt->outprecision : PRECISION;
  width = ((weightmax >= 10.)? floor(log10(weightmax)) : 0) + precision + 3;
  if (precision == 0) width--;

  /* write header */
  fprintf(file, "weight of %d polygons\n", npolyw);
  fprintf(file, "%*s %*s\n", width, "weight", idwidth, "id");

  npoly = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    /* discard null polygons */
    if (!polys[ipoly]) continue;

    /* write weight */
    fprintf(file, "% *.*f %*d\n", width, precision, polys[ipoly]->weight, idwidth, polys[ipoly]->id);

    /* increment polygon count */
    npoly++;
  }

  /* advise */
  msg("%d weights written to %s\n",
      npoly, (file == stdout)? "output": filename);

  /* close file */
  if (file != stdout) fclose(file);

  return(npoly);
}

/*------------------------------------------------------------------------------
  Discard polygons with weight or area outside specified limits.

  Input: polys = polygons.
  npolys = number of polygons.
  Return value: number of polygons retained,
  or -1 if error occurred.
*/
int discard_poly(int npolys, polygon *polys[/*npolys*/])
{
  int discard, ier, ipoly, nbadarea, noutarea, noutweight, npoly;
  double area, tol;

  if (is_weight_min || is_weight_max || is_area_min || is_area_max) {
    noutweight = 0;
    nbadarea = 0;
    noutarea = 0;
    for (ipoly = 0; ipoly < npolys; ipoly++) {
	    discard = 0;

	    /* discard polygons with weights outside interval */
	    if (is_weight_min && is_weight_max) {
        /* min <= max */
        if (weight_min <= weight_max) {
          if (polys[ipoly]->weight < weight_min
              || polys[ipoly]->weight > weight_max) {
            discard = 1;
          }
          /* min > max */
        } else {
          if (polys[ipoly]->weight < weight_min
              && polys[ipoly]->weight > weight_max) {
            discard = 1;
          }
        }
	    } else if (is_weight_min) {
        if (polys[ipoly]->weight < weight_min) {
          discard = 1;
        }
	    } else if (is_weight_max) {
        if (polys[ipoly]->weight > weight_max) {
          discard = 1;
        }
	    }
	    if (discard) {
        noutweight++;
        free_poly(polys[ipoly]);
        polys[ipoly] = 0x0;
        continue;
	    }

	    /* area of polygon */
	    tol = mtol;
	    ier = garea(polys[ipoly], &tol, verb, &area);
	    if (ier == -1) return(-1);
	    if (ier) {
        nbadarea++;

        /* discard polygons with areas outside interval */
	    } else if (is_area_min && is_area_max) {
        /* min <= max */
        if (area_min <= area_max) {
          if (area < area_min
              || area > area_max) {
            discard = 1;
          }
          /* min > max */
        } else {
          if (area < area_min
              && area > area_max) {
            discard = 1;
          }
        }
	    } else if (is_area_min) {
        if (area < area_min) {
          discard = 1;
        }
	    } else if (is_area_max) {
        if (area > area_max) {
          discard = 1;
        }
	    }
	    if (discard) {
        noutarea++;
        free_poly(polys[ipoly]);
        polys[ipoly] = 0x0;
        continue;
	    }

    }

    /* warn about discarded polygons */
    if (noutweight > 0) {
	    if (is_weight_min && is_weight_max) {
        if (weight_min < weight_max) {
          msg("%d polygons with weights outside [%g, %g] discarded\n",
              noutweight, weight_min, weight_max);
        } else {
          msg("%d polygons with weights inside (%g, %g) discarded\n",
              noutweight, weight_max, weight_min);
        }
	    } else if (is_weight_min) {
        msg("%d polygons with weights < %g discarded\n",
            noutweight, weight_min);
	    } else if (is_weight_max) {
        msg("%d polygons with weights > %g discarded\n",
            noutweight, weight_max);
	    }
    }
    if (noutarea > 0) {
	    if (is_area_min && is_area_max) {
        if (area_min < area_max) {
          msg("%d polygons with areas outside [%g, %g] discarded\n",
              noutarea, area_min, area_max);
        } else {
          msg("%d polygons with areas inside (%g, %g) discarded\n",
              noutarea, area_max, area_min);
        }
	    } else if (is_area_min) {
        msg("%d polygons with areas < %g discarded\n",
            noutarea, area_min);
	    } else if (is_area_max) {
        msg("%d polygons with areas > %g discarded\n",
            noutarea, area_max);
	    }
    }

  }

  /* count non-null polygons */
  npoly = 0;
  for (ipoly = 0; ipoly < npolys; ipoly++) {
    if (polys[ipoly]) npoly++;
  }

  return(npoly);
}
