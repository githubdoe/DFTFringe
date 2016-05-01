#ifndef ZERNIKES_H
#define ZERNIKES_H
#include <vector>
#define I(row, col, rowlen) (row)*rowlen+(col)


const char* const zernsNames[] =
{ "Piston",
  "X Tilt",
  "Y Tilt",
  "Defocus",
  "X Astig",
  "Y Astig",
  "X Coma",
  "Y Coma",
  "Spherical",
  "X Trefoi",
  "Y Trefoil",
  "X 2nd Astig",
  "Y 2nd Astig",
  "X 2nd Coma",
  "Y 2nd Coma",
  "2nd Spherical",
  "X Tetrafoi",
  "Y Tetrafoi",
  "2nd X Trefoi",
  "2nd Y Trefoi",
  "3rd X Astig",
  "3rd Y Astig",
  "3rd X Coma",
  "3rd Y Coma",
  "3rd Spherical",
  "25",
  "26",
  "27",
  "28",
  "29",
  "30",
  "31",
  "32",
  "33",
  "34",
  "4th Spherical",
  "36",
  "37",
  "38",
  "5th Spherical",
  "6th Spherical",
  "7th Spherical",
  "8th Spherical",
  // thats all for now
  "42",
  "43",
  "44",
  "45",
  "46",
  "47",
  "48"
};
double computeRMS(int term, double v);
class zern_spec
{
public:
    int* m_n;
    int* m_m;
    int m_nterms;
    unsigned char *m_s; /* 0 = symmetric, 1=cos, 2=sin */


public:
    zern_spec(int order);
    void dump();
    static int rix(int n, int m);
};

class zern_generator
{
public:
    int m_size;
    double* m_rho;
    double* m_theta;
    double* m_zpoly;
    zern_spec* m_spec;
    std::vector<double> m_zcoef;
public:
    ~zern_generator();
    zern_generator(int size);
    void new_size(int size);
    void set_spec(int order);
    void fill_rho();
    void fill_theta();
    void zpoly_list ();
    void dump_zpoly();
    double* zern_eval();
    void set_zcoefs(double *data);
    inline int get_terms_cnt() {return m_spec->m_nterms;};
    inline double get_zpoly(int term, int x, int y)
    {
        return m_zpoly[I(x+y*m_size,term,m_spec->m_nterms)];
    };
};
#endif
