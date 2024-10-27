#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                       Code generated with SymPy 1.12                       *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_3518484267284369564) {
   out_3518484267284369564[0] = delta_x[0] + nom_x[0];
   out_3518484267284369564[1] = delta_x[1] + nom_x[1];
   out_3518484267284369564[2] = delta_x[2] + nom_x[2];
   out_3518484267284369564[3] = delta_x[3] + nom_x[3];
   out_3518484267284369564[4] = delta_x[4] + nom_x[4];
   out_3518484267284369564[5] = delta_x[5] + nom_x[5];
   out_3518484267284369564[6] = delta_x[6] + nom_x[6];
   out_3518484267284369564[7] = delta_x[7] + nom_x[7];
   out_3518484267284369564[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_3810114634763932205) {
   out_3810114634763932205[0] = -nom_x[0] + true_x[0];
   out_3810114634763932205[1] = -nom_x[1] + true_x[1];
   out_3810114634763932205[2] = -nom_x[2] + true_x[2];
   out_3810114634763932205[3] = -nom_x[3] + true_x[3];
   out_3810114634763932205[4] = -nom_x[4] + true_x[4];
   out_3810114634763932205[5] = -nom_x[5] + true_x[5];
   out_3810114634763932205[6] = -nom_x[6] + true_x[6];
   out_3810114634763932205[7] = -nom_x[7] + true_x[7];
   out_3810114634763932205[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_217473642515666120) {
   out_217473642515666120[0] = 1.0;
   out_217473642515666120[1] = 0;
   out_217473642515666120[2] = 0;
   out_217473642515666120[3] = 0;
   out_217473642515666120[4] = 0;
   out_217473642515666120[5] = 0;
   out_217473642515666120[6] = 0;
   out_217473642515666120[7] = 0;
   out_217473642515666120[8] = 0;
   out_217473642515666120[9] = 0;
   out_217473642515666120[10] = 1.0;
   out_217473642515666120[11] = 0;
   out_217473642515666120[12] = 0;
   out_217473642515666120[13] = 0;
   out_217473642515666120[14] = 0;
   out_217473642515666120[15] = 0;
   out_217473642515666120[16] = 0;
   out_217473642515666120[17] = 0;
   out_217473642515666120[18] = 0;
   out_217473642515666120[19] = 0;
   out_217473642515666120[20] = 1.0;
   out_217473642515666120[21] = 0;
   out_217473642515666120[22] = 0;
   out_217473642515666120[23] = 0;
   out_217473642515666120[24] = 0;
   out_217473642515666120[25] = 0;
   out_217473642515666120[26] = 0;
   out_217473642515666120[27] = 0;
   out_217473642515666120[28] = 0;
   out_217473642515666120[29] = 0;
   out_217473642515666120[30] = 1.0;
   out_217473642515666120[31] = 0;
   out_217473642515666120[32] = 0;
   out_217473642515666120[33] = 0;
   out_217473642515666120[34] = 0;
   out_217473642515666120[35] = 0;
   out_217473642515666120[36] = 0;
   out_217473642515666120[37] = 0;
   out_217473642515666120[38] = 0;
   out_217473642515666120[39] = 0;
   out_217473642515666120[40] = 1.0;
   out_217473642515666120[41] = 0;
   out_217473642515666120[42] = 0;
   out_217473642515666120[43] = 0;
   out_217473642515666120[44] = 0;
   out_217473642515666120[45] = 0;
   out_217473642515666120[46] = 0;
   out_217473642515666120[47] = 0;
   out_217473642515666120[48] = 0;
   out_217473642515666120[49] = 0;
   out_217473642515666120[50] = 1.0;
   out_217473642515666120[51] = 0;
   out_217473642515666120[52] = 0;
   out_217473642515666120[53] = 0;
   out_217473642515666120[54] = 0;
   out_217473642515666120[55] = 0;
   out_217473642515666120[56] = 0;
   out_217473642515666120[57] = 0;
   out_217473642515666120[58] = 0;
   out_217473642515666120[59] = 0;
   out_217473642515666120[60] = 1.0;
   out_217473642515666120[61] = 0;
   out_217473642515666120[62] = 0;
   out_217473642515666120[63] = 0;
   out_217473642515666120[64] = 0;
   out_217473642515666120[65] = 0;
   out_217473642515666120[66] = 0;
   out_217473642515666120[67] = 0;
   out_217473642515666120[68] = 0;
   out_217473642515666120[69] = 0;
   out_217473642515666120[70] = 1.0;
   out_217473642515666120[71] = 0;
   out_217473642515666120[72] = 0;
   out_217473642515666120[73] = 0;
   out_217473642515666120[74] = 0;
   out_217473642515666120[75] = 0;
   out_217473642515666120[76] = 0;
   out_217473642515666120[77] = 0;
   out_217473642515666120[78] = 0;
   out_217473642515666120[79] = 0;
   out_217473642515666120[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_1574628128844035309) {
   out_1574628128844035309[0] = state[0];
   out_1574628128844035309[1] = state[1];
   out_1574628128844035309[2] = state[2];
   out_1574628128844035309[3] = state[3];
   out_1574628128844035309[4] = state[4];
   out_1574628128844035309[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_1574628128844035309[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_1574628128844035309[7] = state[7];
   out_1574628128844035309[8] = state[8];
}
void F_fun(double *state, double dt, double *out_6350459106931495726) {
   out_6350459106931495726[0] = 1;
   out_6350459106931495726[1] = 0;
   out_6350459106931495726[2] = 0;
   out_6350459106931495726[3] = 0;
   out_6350459106931495726[4] = 0;
   out_6350459106931495726[5] = 0;
   out_6350459106931495726[6] = 0;
   out_6350459106931495726[7] = 0;
   out_6350459106931495726[8] = 0;
   out_6350459106931495726[9] = 0;
   out_6350459106931495726[10] = 1;
   out_6350459106931495726[11] = 0;
   out_6350459106931495726[12] = 0;
   out_6350459106931495726[13] = 0;
   out_6350459106931495726[14] = 0;
   out_6350459106931495726[15] = 0;
   out_6350459106931495726[16] = 0;
   out_6350459106931495726[17] = 0;
   out_6350459106931495726[18] = 0;
   out_6350459106931495726[19] = 0;
   out_6350459106931495726[20] = 1;
   out_6350459106931495726[21] = 0;
   out_6350459106931495726[22] = 0;
   out_6350459106931495726[23] = 0;
   out_6350459106931495726[24] = 0;
   out_6350459106931495726[25] = 0;
   out_6350459106931495726[26] = 0;
   out_6350459106931495726[27] = 0;
   out_6350459106931495726[28] = 0;
   out_6350459106931495726[29] = 0;
   out_6350459106931495726[30] = 1;
   out_6350459106931495726[31] = 0;
   out_6350459106931495726[32] = 0;
   out_6350459106931495726[33] = 0;
   out_6350459106931495726[34] = 0;
   out_6350459106931495726[35] = 0;
   out_6350459106931495726[36] = 0;
   out_6350459106931495726[37] = 0;
   out_6350459106931495726[38] = 0;
   out_6350459106931495726[39] = 0;
   out_6350459106931495726[40] = 1;
   out_6350459106931495726[41] = 0;
   out_6350459106931495726[42] = 0;
   out_6350459106931495726[43] = 0;
   out_6350459106931495726[44] = 0;
   out_6350459106931495726[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_6350459106931495726[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_6350459106931495726[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_6350459106931495726[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_6350459106931495726[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_6350459106931495726[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_6350459106931495726[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_6350459106931495726[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_6350459106931495726[53] = -9.8000000000000007*dt;
   out_6350459106931495726[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_6350459106931495726[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_6350459106931495726[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_6350459106931495726[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_6350459106931495726[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_6350459106931495726[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_6350459106931495726[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_6350459106931495726[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_6350459106931495726[62] = 0;
   out_6350459106931495726[63] = 0;
   out_6350459106931495726[64] = 0;
   out_6350459106931495726[65] = 0;
   out_6350459106931495726[66] = 0;
   out_6350459106931495726[67] = 0;
   out_6350459106931495726[68] = 0;
   out_6350459106931495726[69] = 0;
   out_6350459106931495726[70] = 1;
   out_6350459106931495726[71] = 0;
   out_6350459106931495726[72] = 0;
   out_6350459106931495726[73] = 0;
   out_6350459106931495726[74] = 0;
   out_6350459106931495726[75] = 0;
   out_6350459106931495726[76] = 0;
   out_6350459106931495726[77] = 0;
   out_6350459106931495726[78] = 0;
   out_6350459106931495726[79] = 0;
   out_6350459106931495726[80] = 1;
}
void h_25(double *state, double *unused, double *out_4195343927427872218) {
   out_4195343927427872218[0] = state[6];
}
void H_25(double *state, double *unused, double *out_1575838110186472730) {
   out_1575838110186472730[0] = 0;
   out_1575838110186472730[1] = 0;
   out_1575838110186472730[2] = 0;
   out_1575838110186472730[3] = 0;
   out_1575838110186472730[4] = 0;
   out_1575838110186472730[5] = 0;
   out_1575838110186472730[6] = 1;
   out_1575838110186472730[7] = 0;
   out_1575838110186472730[8] = 0;
}
void h_24(double *state, double *unused, double *out_7628279402621442542) {
   out_7628279402621442542[0] = state[4];
   out_7628279402621442542[1] = state[5];
}
void H_24(double *state, double *unused, double *out_730841501402777331) {
   out_730841501402777331[0] = 0;
   out_730841501402777331[1] = 0;
   out_730841501402777331[2] = 0;
   out_730841501402777331[3] = 0;
   out_730841501402777331[4] = 1;
   out_730841501402777331[5] = 0;
   out_730841501402777331[6] = 0;
   out_730841501402777331[7] = 0;
   out_730841501402777331[8] = 0;
   out_730841501402777331[9] = 0;
   out_730841501402777331[10] = 0;
   out_730841501402777331[11] = 0;
   out_730841501402777331[12] = 0;
   out_730841501402777331[13] = 0;
   out_730841501402777331[14] = 1;
   out_730841501402777331[15] = 0;
   out_730841501402777331[16] = 0;
   out_730841501402777331[17] = 0;
}
void h_30(double *state, double *unused, double *out_6790841382032791073) {
   out_6790841382032791073[0] = state[4];
}
void H_30(double *state, double *unused, double *out_5340852231305144025) {
   out_5340852231305144025[0] = 0;
   out_5340852231305144025[1] = 0;
   out_5340852231305144025[2] = 0;
   out_5340852231305144025[3] = 0;
   out_5340852231305144025[4] = 1;
   out_5340852231305144025[5] = 0;
   out_5340852231305144025[6] = 0;
   out_5340852231305144025[7] = 0;
   out_5340852231305144025[8] = 0;
}
void h_26(double *state, double *unused, double *out_9051497370868062892) {
   out_9051497370868062892[0] = state[7];
}
void H_26(double *state, double *unused, double *out_1728687859574327871) {
   out_1728687859574327871[0] = 0;
   out_1728687859574327871[1] = 0;
   out_1728687859574327871[2] = 0;
   out_1728687859574327871[3] = 0;
   out_1728687859574327871[4] = 0;
   out_1728687859574327871[5] = 0;
   out_1728687859574327871[6] = 0;
   out_1728687859574327871[7] = 1;
   out_1728687859574327871[8] = 0;
}
void h_27(double *state, double *unused, double *out_1030563288407452729) {
   out_1030563288407452729[0] = state[3];
}
void H_27(double *state, double *unused, double *out_3166088919504719114) {
   out_3166088919504719114[0] = 0;
   out_3166088919504719114[1] = 0;
   out_3166088919504719114[2] = 0;
   out_3166088919504719114[3] = 1;
   out_3166088919504719114[4] = 0;
   out_3166088919504719114[5] = 0;
   out_3166088919504719114[6] = 0;
   out_3166088919504719114[7] = 0;
   out_3166088919504719114[8] = 0;
}
void h_29(double *state, double *unused, double *out_6654229424015914898) {
   out_6654229424015914898[0] = state[1];
}
void H_29(double *state, double *unused, double *out_5851083575619536209) {
   out_5851083575619536209[0] = 0;
   out_5851083575619536209[1] = 1;
   out_5851083575619536209[2] = 0;
   out_5851083575619536209[3] = 0;
   out_5851083575619536209[4] = 0;
   out_5851083575619536209[5] = 0;
   out_5851083575619536209[6] = 0;
   out_5851083575619536209[7] = 0;
   out_5851083575619536209[8] = 0;
}
void h_28(double *state, double *unused, double *out_137180512668689008) {
   out_137180512668689008[0] = state[0];
}
void H_28(double *state, double *unused, double *out_3629672824434362493) {
   out_3629672824434362493[0] = 1;
   out_3629672824434362493[1] = 0;
   out_3629672824434362493[2] = 0;
   out_3629672824434362493[3] = 0;
   out_3629672824434362493[4] = 0;
   out_3629672824434362493[5] = 0;
   out_3629672824434362493[6] = 0;
   out_3629672824434362493[7] = 0;
   out_3629672824434362493[8] = 0;
}
void h_31(double *state, double *unused, double *out_3567921329392800223) {
   out_3567921329392800223[0] = state[8];
}
void H_31(double *state, double *unused, double *out_5500837140325344523) {
   out_5500837140325344523[0] = 0;
   out_5500837140325344523[1] = 0;
   out_5500837140325344523[2] = 0;
   out_5500837140325344523[3] = 0;
   out_5500837140325344523[4] = 0;
   out_5500837140325344523[5] = 0;
   out_5500837140325344523[6] = 0;
   out_5500837140325344523[7] = 0;
   out_5500837140325344523[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_3518484267284369564) {
  err_fun(nom_x, delta_x, out_3518484267284369564);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_3810114634763932205) {
  inv_err_fun(nom_x, true_x, out_3810114634763932205);
}
void car_H_mod_fun(double *state, double *out_217473642515666120) {
  H_mod_fun(state, out_217473642515666120);
}
void car_f_fun(double *state, double dt, double *out_1574628128844035309) {
  f_fun(state,  dt, out_1574628128844035309);
}
void car_F_fun(double *state, double dt, double *out_6350459106931495726) {
  F_fun(state,  dt, out_6350459106931495726);
}
void car_h_25(double *state, double *unused, double *out_4195343927427872218) {
  h_25(state, unused, out_4195343927427872218);
}
void car_H_25(double *state, double *unused, double *out_1575838110186472730) {
  H_25(state, unused, out_1575838110186472730);
}
void car_h_24(double *state, double *unused, double *out_7628279402621442542) {
  h_24(state, unused, out_7628279402621442542);
}
void car_H_24(double *state, double *unused, double *out_730841501402777331) {
  H_24(state, unused, out_730841501402777331);
}
void car_h_30(double *state, double *unused, double *out_6790841382032791073) {
  h_30(state, unused, out_6790841382032791073);
}
void car_H_30(double *state, double *unused, double *out_5340852231305144025) {
  H_30(state, unused, out_5340852231305144025);
}
void car_h_26(double *state, double *unused, double *out_9051497370868062892) {
  h_26(state, unused, out_9051497370868062892);
}
void car_H_26(double *state, double *unused, double *out_1728687859574327871) {
  H_26(state, unused, out_1728687859574327871);
}
void car_h_27(double *state, double *unused, double *out_1030563288407452729) {
  h_27(state, unused, out_1030563288407452729);
}
void car_H_27(double *state, double *unused, double *out_3166088919504719114) {
  H_27(state, unused, out_3166088919504719114);
}
void car_h_29(double *state, double *unused, double *out_6654229424015914898) {
  h_29(state, unused, out_6654229424015914898);
}
void car_H_29(double *state, double *unused, double *out_5851083575619536209) {
  H_29(state, unused, out_5851083575619536209);
}
void car_h_28(double *state, double *unused, double *out_137180512668689008) {
  h_28(state, unused, out_137180512668689008);
}
void car_H_28(double *state, double *unused, double *out_3629672824434362493) {
  H_28(state, unused, out_3629672824434362493);
}
void car_h_31(double *state, double *unused, double *out_3567921329392800223) {
  h_31(state, unused, out_3567921329392800223);
}
void car_H_31(double *state, double *unused, double *out_5500837140325344523) {
  H_31(state, unused, out_5500837140325344523);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
