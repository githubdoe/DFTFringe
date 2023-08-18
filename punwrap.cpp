
/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "punwrap.h"
#include <math.h>
#include <algorithm>
#include <queue>
#include <cstring>
int xsize,ysize;
double* qmap = NULL;
double* phase;
double* unwrapped;

#include <stdlib.h>
#include <math.h>

#define WRAP(x) (((x) > 0.5) ? ((x)-1.0) : (((x) <= -0.5) ? ((x)+1.0) : (x)))
//double WRAP(double x){
//   if (x > .5){
//       return x - 1.;
//   }
//   else if (x <= -.5){
//       return x+1.0;

//   }
//   return x;
//}

char* flags = NULL;
#define BORDER      0x1
#define UNWRAPPED   0x2

#define SWAP(a, i, j)                           \
    {                                           \
        int t = a[i];                           \
        a[i] = a[j];                            \
        a[j] = t;                               \
    }

static
void todo_push (int ndx, double *qmap, int *todo, int *end)
{
    int child;
    todo[*end] = ndx;
    child = (*end)++;
    while (child > 0) {
        int parent = (child-1) / 2;
        if (qmap[todo[parent]] < qmap[todo[child]]) {
            SWAP (todo, parent, child);
            child = parent;
        }
        else
            break;
    }
}

static
int todo_pop(double *qmap, int *todo, int *end)
{
    int result = todo[0], root;
    --(*end);
   SWAP (todo, 0, *end);
    root = 0;
    while (root*2+1 < *end) {
        int child = root*2+1; // left child
        if (child+1 < *end && qmap[todo[child]] < qmap[todo[child+1]])
            ++child;
        if (qmap[todo[root]] < qmap[todo[child]]) {
            SWAP (todo, root, child);
            root = child;
        }
        else
            break;
    }
    return (result);
}

#define unwrap_and_insert(ndx, val)             \
    {                                           \
        unwrapped[ndx] = val;                   \
        flags[ndx] |= UNWRAPPED;                \
        path[ndx] = order++;                    \
        todo_push (ndx, qmap, todo, &end);      \
    }

// Quality-guided path following phase unwrapper.
void qg_path_follower (int nx, int ny, double *phase, double *qmap,
                       double *unwrapped, double *path)
{
    int *todo;
    int end;
    int order = 0;
    int size = nx * ny;

    // Initialize the to do list.
    todo = (int*)calloc (size, sizeof (int));
    end = 0;

    // Repeat while still elements to unwrap (handles disjoint regions).
    while (1) {

        // Find the point of highest quality.
        double m = -HUGE_VAL;
        int mndx = 0;
        for (int k=0; k < size; ++k)
            if (qmap[k] > m && ! flags[k])
                m = qmap[mndx = k];
        if (m == -HUGE_VAL) break;

        // Unwrap the first point.
        unwrap_and_insert (mndx, phase[mndx]);

        // Unwrap the rest of the points in order of quality.
        while (end) {
            int ndx = todo_pop (qmap, todo, &end);
            int x = ndx%nx;
            int y = ndx/nx;
            double val = unwrapped[ndx];
            // west
            if (x > 0 && ! flags[ndx-1])
                unwrap_and_insert (ndx-1, val+WRAP(phase[ndx-1]-phase[ndx]));
            // east
            if (x < nx-1 && ! flags[ndx+1])
                unwrap_and_insert (ndx+1, val+WRAP(phase[ndx+1]-phase[ndx]));
            // north
            if (y > 0 && ! flags[ndx-nx])
                unwrap_and_insert (ndx-nx, val+WRAP(phase[ndx-nx]-phase[ndx]));
            // south
            if (y < ny-1 && ! flags[ndx+nx])
                unwrap_and_insert (ndx+nx, val+WRAP(phase[ndx+nx]-phase[ndx]));
        }
    }


    free (todo);
}


void dv_quality_map (double *pphase,int width, double *qmap, int nx, int ny)
{
  double *dx;
  double *dy;

  dx = (double*)calloc (nx*ny, sizeof (double));
  dy = (double*)calloc (nx*ny, sizeof (double));

  // Calculate the arrays of gradients.
  for (int x=0; x < nx; ++x)
    for (int y=0; y < ny; ++y) {
      int ndx = y*nx+x;
      if (x == nx-1)
        dx[ndx] = 0.0;
      else
        dx[ndx] = WRAP (pphase[ndx+1] - pphase[ndx]);
      if (y == ny-1)
        dy[ndx] = 0.0;
      else
        dy[ndx] = WRAP (pphase[ndx+nx] - pphase[ndx]);
    }

  // Calculate the variance of the gradients over the moving window.
  int start = -(width/2);
  int end = start+width;
  int size = width * width;
  double* ex = new double[size];
  double* ey = new double[size];

  for ( int x=0; x < nx; ++x)
    for (int y=0; y < ny; ++y) {
      int n = 0;
      // Load elements from window into ex and ey.
      for (int i = std::max(x+start, 0); i < std::min(x+end, nx-1); ++i)
            for (int j = std::max(y+start, 0); j < std::min(y+end, ny-1); ++j) {
                int ndx = j*nx + i;
                if (0.0 != (dx[ndx]) && 0.0 != (dy[ndx])) {
                    ex[n] = dx[ndx];
                    ey[n] = dy[ndx];
                    n++;
                }
            }
      int ndx = y*nx + x;
      if (n < 1)
        qmap[ndx] = 0;
      else {
        double mx = 0, my = 0;
        for (int k=0; k < n; k++) {
            mx += ex[k];
            my += ey[k];
        }
        mx /= n;
        my /= n;
        double sx = 0;
        double sy = 0;
        for (int k=0; k < n; k++) {
            sx += pow (ex[k]-mx, 2);
            sy += pow (ey[k]-my, 2);
        }
        qmap[ndx] = (sqrt(sx) + sqrt(sy)) / (size);
      }
    }

  free (dx);
  free (dy);
  delete[] ex;
  delete[] ey;

}



void pc_quality_map (int nx, int ny, double *phase, int width, double *qmap)
{
  // Calculate the pseudo-correlation over the moving window.
  int start = -(width/2);
  int end = start+width;
  double* ep = new double[width*width];

  for (int x=0; x < nx; ++x)
    for (int y=0; y < ny; ++y) {

      int n = 0;
      // Load elements from window into ex and ey.
      for (int i = std::max(x+start, 0); i < std::min(x+end, nx-1); ++i)
    for (int j = std::max(y+start, 0); j < std::min(y+end, ny-1); ++j) {
      int ndx = j*nx + i;
      if (0.0 != (phase[ndx])) {
        ep[n] = phase[ndx];
        n++;
      }
    }
      int ndx = y*nx + x;
      if (n < 1)
    qmap[ndx] = 0;
      else {
    double sp = 0, cp = 0;
    for (int k=0; k < n; k++) {
      sp += sin (ep[k]);
      cp += cos (ep[k]);
    }
    qmap[ndx] = 1 - sqrt(sp*sp + cp*cp) / (width*width);
      }
    }
}






class qual
{
public:
    int m_ndx;
    double m_q;
    qual(int ndx, double qual) : m_ndx(ndx), m_q(qual){};
};

//Function object to do a less on two points in the quality map
class comp_qual
{
public:

    int operator() (const qual &p1, const qual &p2)
    {
    return (p1.m_q < p2.m_q);
    }
};

std::priority_queue<qual,std::vector<qual>,comp_qual> To_Do;
/* input is normalized from 0 to 1 */

#define GRAD(x) (((x) > 0.5) ? ((x)-1.0) : (((x) <= -0.5) ? ((x)+1.0) : (x)))

/* Insert new pixel into the to do deque. Sorted by quality.  Best at end */
inline void Insert_To_Do(double val, int index, double q)
{
  unwrapped[index] = val;
  To_Do.push(qual(index, q));
  flags[index] =1;
  return;
}

/* Insert the four neighboring pixels of x,y
into the to do list.  The quality value for x,y  is val */
void Add_Neighbors(int mid_ndx, double val)
{

    double  grad;
    int x = mid_ndx % xsize;
    int y = mid_ndx / xsize;

    int ndx = mid_ndx -1;

    // left neighbor
    if ((x-1)>= 0 && !(flags[ndx]))
    {
        grad = GRAD(phase[ndx]- phase[mid_ndx]);  // left - middle
        Insert_To_Do( val + grad, ndx, -fabs(grad));
    }

    // right neighbor
    ndx += 2;
    if ((x+1) < xsize && !(flags[ndx])) {
        grad = GRAD(phase[ndx] -phase[mid_ndx]); //right - middle
        Insert_To_Do(val + grad, ndx, -fabs(grad));
    }

    //up neighbor
    ndx = mid_ndx - xsize;
    if ((y-1) >= 0  && !(flags[ndx]))
    {
        grad = GRAD(phase[ndx] -phase[mid_ndx]);  //up - middle
        Insert_To_Do( val + grad, ndx, -fabs(grad));
    }

    // down neighbor
    ndx= (mid_ndx + xsize);
    if ((y+1) < ysize && !(flags[ndx])) {
        grad = GRAD(phase[ndx] - phase[mid_ndx]); // down - middle
        Insert_To_Do(val + grad, ndx, -fabs(grad));
    }
}

/* quality guided path following for phase unwrapping */
void QGPathFollower()
{
    int size = xsize * ysize;

    for (int cur = 0; cur < size; ++cur)
    {
        if (!(flags[cur]))
        {
            double value = unwrapped[cur] = phase[cur];

            // set unwrapped flag
            flags[cur] =1;

            Add_Neighbors( cur, value);

            while (!To_Do.empty())
            {
                int ndx = To_Do.top().m_ndx;
                To_Do.pop();

                value = unwrapped[ndx];

                Add_Neighbors( ndx, value);

            }  //while
        }

    }

}
void qg_path_follower (cv::Mat phase, double *qmap,
                       double *unwrapped, double *path);

void dv_quality_map (double *phase, int width, double *qmap, int nx, int ny);
double* g_qqmap = NULL;
double* path = NULL;


/* main entrypoint for unwrapping. Input phase is scaled from 0 to 1 */
void unwrap(double * pphase, double *punwrapped, char* bflags, int nx, int ny)
{

    xsize = nx;
    ysize = ny;
    unwrapped = punwrapped;
    phase = pphase;

  const int size = xsize * ysize;
  flags = bflags;



  /* initialize unwrapped array */

   double* qmap;
  // make the quality map
   if (g_qqmap)
    delete[] g_qqmap;

  g_qqmap = qmap = new double[size];
  memset(qmap,0, sizeof(double)*size);

  // TODO JST 20230815 this limits memory leak but should be refactored with a object oriented programming
  if(path){
    delete[] path;
  }
  path = new double[size];
  memset(path,0,sizeof(double)*size);

  dv_quality_map(pphase, 5, qmap, nx, ny);
  for (int i = 0; i < size; ++i)
      qmap[i] *= -1.;


  qg_path_follower(nx,ny,pphase, qmap, unwrapped, path);

}

void vortex_rho_theta(int width, int height, double* rho, double* theta)
{
    double hx = width/2;
    double hy = height/2;

    for (int y = 0; y < height; ++y)
    {
        double ry = hy-y;
        int iy = y * width;
        for (int x = 0; x < width; ++x)
        {
            double rx = hx-x;
            rho[x + iy] = sqrt(rx * rx + ry* ry);
            theta[x+iy] = atan2(ry,rx);
        }
    }

}
