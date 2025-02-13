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
void err_fun(double *nom_x, double *delta_x, double *out_2543192712933755862) {
   out_2543192712933755862[0] = delta_x[0] + nom_x[0];
   out_2543192712933755862[1] = delta_x[1] + nom_x[1];
   out_2543192712933755862[2] = delta_x[2] + nom_x[2];
   out_2543192712933755862[3] = delta_x[3] + nom_x[3];
   out_2543192712933755862[4] = delta_x[4] + nom_x[4];
   out_2543192712933755862[5] = delta_x[5] + nom_x[5];
   out_2543192712933755862[6] = delta_x[6] + nom_x[6];
   out_2543192712933755862[7] = delta_x[7] + nom_x[7];
   out_2543192712933755862[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_456116514451130776) {
   out_456116514451130776[0] = -nom_x[0] + true_x[0];
   out_456116514451130776[1] = -nom_x[1] + true_x[1];
   out_456116514451130776[2] = -nom_x[2] + true_x[2];
   out_456116514451130776[3] = -nom_x[3] + true_x[3];
   out_456116514451130776[4] = -nom_x[4] + true_x[4];
   out_456116514451130776[5] = -nom_x[5] + true_x[5];
   out_456116514451130776[6] = -nom_x[6] + true_x[6];
   out_456116514451130776[7] = -nom_x[7] + true_x[7];
   out_456116514451130776[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_8329058010871792226) {
   out_8329058010871792226[0] = 1.0;
   out_8329058010871792226[1] = 0;
   out_8329058010871792226[2] = 0;
   out_8329058010871792226[3] = 0;
   out_8329058010871792226[4] = 0;
   out_8329058010871792226[5] = 0;
   out_8329058010871792226[6] = 0;
   out_8329058010871792226[7] = 0;
   out_8329058010871792226[8] = 0;
   out_8329058010871792226[9] = 0;
   out_8329058010871792226[10] = 1.0;
   out_8329058010871792226[11] = 0;
   out_8329058010871792226[12] = 0;
   out_8329058010871792226[13] = 0;
   out_8329058010871792226[14] = 0;
   out_8329058010871792226[15] = 0;
   out_8329058010871792226[16] = 0;
   out_8329058010871792226[17] = 0;
   out_8329058010871792226[18] = 0;
   out_8329058010871792226[19] = 0;
   out_8329058010871792226[20] = 1.0;
   out_8329058010871792226[21] = 0;
   out_8329058010871792226[22] = 0;
   out_8329058010871792226[23] = 0;
   out_8329058010871792226[24] = 0;
   out_8329058010871792226[25] = 0;
   out_8329058010871792226[26] = 0;
   out_8329058010871792226[27] = 0;
   out_8329058010871792226[28] = 0;
   out_8329058010871792226[29] = 0;
   out_8329058010871792226[30] = 1.0;
   out_8329058010871792226[31] = 0;
   out_8329058010871792226[32] = 0;
   out_8329058010871792226[33] = 0;
   out_8329058010871792226[34] = 0;
   out_8329058010871792226[35] = 0;
   out_8329058010871792226[36] = 0;
   out_8329058010871792226[37] = 0;
   out_8329058010871792226[38] = 0;
   out_8329058010871792226[39] = 0;
   out_8329058010871792226[40] = 1.0;
   out_8329058010871792226[41] = 0;
   out_8329058010871792226[42] = 0;
   out_8329058010871792226[43] = 0;
   out_8329058010871792226[44] = 0;
   out_8329058010871792226[45] = 0;
   out_8329058010871792226[46] = 0;
   out_8329058010871792226[47] = 0;
   out_8329058010871792226[48] = 0;
   out_8329058010871792226[49] = 0;
   out_8329058010871792226[50] = 1.0;
   out_8329058010871792226[51] = 0;
   out_8329058010871792226[52] = 0;
   out_8329058010871792226[53] = 0;
   out_8329058010871792226[54] = 0;
   out_8329058010871792226[55] = 0;
   out_8329058010871792226[56] = 0;
   out_8329058010871792226[57] = 0;
   out_8329058010871792226[58] = 0;
   out_8329058010871792226[59] = 0;
   out_8329058010871792226[60] = 1.0;
   out_8329058010871792226[61] = 0;
   out_8329058010871792226[62] = 0;
   out_8329058010871792226[63] = 0;
   out_8329058010871792226[64] = 0;
   out_8329058010871792226[65] = 0;
   out_8329058010871792226[66] = 0;
   out_8329058010871792226[67] = 0;
   out_8329058010871792226[68] = 0;
   out_8329058010871792226[69] = 0;
   out_8329058010871792226[70] = 1.0;
   out_8329058010871792226[71] = 0;
   out_8329058010871792226[72] = 0;
   out_8329058010871792226[73] = 0;
   out_8329058010871792226[74] = 0;
   out_8329058010871792226[75] = 0;
   out_8329058010871792226[76] = 0;
   out_8329058010871792226[77] = 0;
   out_8329058010871792226[78] = 0;
   out_8329058010871792226[79] = 0;
   out_8329058010871792226[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_7259258364871350970) {
   out_7259258364871350970[0] = state[0];
   out_7259258364871350970[1] = state[1];
   out_7259258364871350970[2] = state[2];
   out_7259258364871350970[3] = state[3];
   out_7259258364871350970[4] = state[4];
   out_7259258364871350970[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_7259258364871350970[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_7259258364871350970[7] = state[7];
   out_7259258364871350970[8] = state[8];
}
void F_fun(double *state, double dt, double *out_8361658207755630191) {
   out_8361658207755630191[0] = 1;
   out_8361658207755630191[1] = 0;
   out_8361658207755630191[2] = 0;
   out_8361658207755630191[3] = 0;
   out_8361658207755630191[4] = 0;
   out_8361658207755630191[5] = 0;
   out_8361658207755630191[6] = 0;
   out_8361658207755630191[7] = 0;
   out_8361658207755630191[8] = 0;
   out_8361658207755630191[9] = 0;
   out_8361658207755630191[10] = 1;
   out_8361658207755630191[11] = 0;
   out_8361658207755630191[12] = 0;
   out_8361658207755630191[13] = 0;
   out_8361658207755630191[14] = 0;
   out_8361658207755630191[15] = 0;
   out_8361658207755630191[16] = 0;
   out_8361658207755630191[17] = 0;
   out_8361658207755630191[18] = 0;
   out_8361658207755630191[19] = 0;
   out_8361658207755630191[20] = 1;
   out_8361658207755630191[21] = 0;
   out_8361658207755630191[22] = 0;
   out_8361658207755630191[23] = 0;
   out_8361658207755630191[24] = 0;
   out_8361658207755630191[25] = 0;
   out_8361658207755630191[26] = 0;
   out_8361658207755630191[27] = 0;
   out_8361658207755630191[28] = 0;
   out_8361658207755630191[29] = 0;
   out_8361658207755630191[30] = 1;
   out_8361658207755630191[31] = 0;
   out_8361658207755630191[32] = 0;
   out_8361658207755630191[33] = 0;
   out_8361658207755630191[34] = 0;
   out_8361658207755630191[35] = 0;
   out_8361658207755630191[36] = 0;
   out_8361658207755630191[37] = 0;
   out_8361658207755630191[38] = 0;
   out_8361658207755630191[39] = 0;
   out_8361658207755630191[40] = 1;
   out_8361658207755630191[41] = 0;
   out_8361658207755630191[42] = 0;
   out_8361658207755630191[43] = 0;
   out_8361658207755630191[44] = 0;
   out_8361658207755630191[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_8361658207755630191[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_8361658207755630191[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8361658207755630191[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8361658207755630191[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_8361658207755630191[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_8361658207755630191[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_8361658207755630191[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_8361658207755630191[53] = -9.8000000000000007*dt;
   out_8361658207755630191[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_8361658207755630191[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_8361658207755630191[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8361658207755630191[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8361658207755630191[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_8361658207755630191[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_8361658207755630191[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_8361658207755630191[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8361658207755630191[62] = 0;
   out_8361658207755630191[63] = 0;
   out_8361658207755630191[64] = 0;
   out_8361658207755630191[65] = 0;
   out_8361658207755630191[66] = 0;
   out_8361658207755630191[67] = 0;
   out_8361658207755630191[68] = 0;
   out_8361658207755630191[69] = 0;
   out_8361658207755630191[70] = 1;
   out_8361658207755630191[71] = 0;
   out_8361658207755630191[72] = 0;
   out_8361658207755630191[73] = 0;
   out_8361658207755630191[74] = 0;
   out_8361658207755630191[75] = 0;
   out_8361658207755630191[76] = 0;
   out_8361658207755630191[77] = 0;
   out_8361658207755630191[78] = 0;
   out_8361658207755630191[79] = 0;
   out_8361658207755630191[80] = 1;
}
void h_25(double *state, double *unused, double *out_8066659053161303442) {
   out_8066659053161303442[0] = state[6];
}
void H_25(double *state, double *unused, double *out_7798991958014807135) {
   out_7798991958014807135[0] = 0;
   out_7798991958014807135[1] = 0;
   out_7798991958014807135[2] = 0;
   out_7798991958014807135[3] = 0;
   out_7798991958014807135[4] = 0;
   out_7798991958014807135[5] = 0;
   out_7798991958014807135[6] = 1;
   out_7798991958014807135[7] = 0;
   out_7798991958014807135[8] = 0;
}
void h_24(double *state, double *unused, double *out_1246392366224867809) {
   out_1246392366224867809[0] = state[4];
   out_1246392366224867809[1] = state[5];
}
void H_24(double *state, double *unused, double *out_4072180309103226380) {
   out_4072180309103226380[0] = 0;
   out_4072180309103226380[1] = 0;
   out_4072180309103226380[2] = 0;
   out_4072180309103226380[3] = 0;
   out_4072180309103226380[4] = 1;
   out_4072180309103226380[5] = 0;
   out_4072180309103226380[6] = 0;
   out_4072180309103226380[7] = 0;
   out_4072180309103226380[8] = 0;
   out_4072180309103226380[9] = 0;
   out_4072180309103226380[10] = 0;
   out_4072180309103226380[11] = 0;
   out_4072180309103226380[12] = 0;
   out_4072180309103226380[13] = 0;
   out_4072180309103226380[14] = 1;
   out_4072180309103226380[15] = 0;
   out_4072180309103226380[16] = 0;
   out_4072180309103226380[17] = 0;
}
void h_30(double *state, double *unused, double *out_8341853115445809331) {
   out_8341853115445809331[0] = state[4];
}
void H_30(double *state, double *unused, double *out_8129419157187495854) {
   out_8129419157187495854[0] = 0;
   out_8129419157187495854[1] = 0;
   out_8129419157187495854[2] = 0;
   out_8129419157187495854[3] = 0;
   out_8129419157187495854[4] = 1;
   out_8129419157187495854[5] = 0;
   out_8129419157187495854[6] = 0;
   out_8129419157187495854[7] = 0;
   out_8129419157187495854[8] = 0;
}
void h_26(double *state, double *unused, double *out_3340179847784963686) {
   out_3340179847784963686[0] = state[7];
}
void H_26(double *state, double *unused, double *out_4057488639140750911) {
   out_4057488639140750911[0] = 0;
   out_4057488639140750911[1] = 0;
   out_4057488639140750911[2] = 0;
   out_4057488639140750911[3] = 0;
   out_4057488639140750911[4] = 0;
   out_4057488639140750911[5] = 0;
   out_4057488639140750911[6] = 0;
   out_4057488639140750911[7] = 1;
   out_4057488639140750911[8] = 0;
}
void h_27(double *state, double *unused, double *out_8675922967799421912) {
   out_8675922967799421912[0] = state[3];
}
void H_27(double *state, double *unused, double *out_5905825086003552637) {
   out_5905825086003552637[0] = 0;
   out_5905825086003552637[1] = 0;
   out_5905825086003552637[2] = 0;
   out_5905825086003552637[3] = 1;
   out_5905825086003552637[4] = 0;
   out_5905825086003552637[5] = 0;
   out_5905825086003552637[6] = 0;
   out_5905825086003552637[7] = 0;
   out_5905825086003552637[8] = 0;
}
void h_29(double *state, double *unused, double *out_2999985879559778768) {
   out_2999985879559778768[0] = state[1];
}
void H_29(double *state, double *unused, double *out_7619187812873103670) {
   out_7619187812873103670[0] = 0;
   out_7619187812873103670[1] = 1;
   out_7619187812873103670[2] = 0;
   out_7619187812873103670[3] = 0;
   out_7619187812873103670[4] = 0;
   out_7619187812873103670[5] = 0;
   out_7619187812873103670[6] = 0;
   out_7619187812873103670[7] = 0;
   out_7619187812873103670[8] = 0;
}
void h_28(double *state, double *unused, double *out_3674249700138576267) {
   out_3674249700138576267[0] = state[0];
}
void H_28(double *state, double *unused, double *out_5745157243766917372) {
   out_5745157243766917372[0] = 1;
   out_5745157243766917372[1] = 0;
   out_5745157243766917372[2] = 0;
   out_5745157243766917372[3] = 0;
   out_5745157243766917372[4] = 0;
   out_5745157243766917372[5] = 0;
   out_5745157243766917372[6] = 0;
   out_5745157243766917372[7] = 0;
   out_5745157243766917372[8] = 0;
}
void h_31(double *state, double *unused, double *out_6219779230878167226) {
   out_6219779230878167226[0] = state[8];
}
void H_31(double *state, double *unused, double *out_7829637919891767563) {
   out_7829637919891767563[0] = 0;
   out_7829637919891767563[1] = 0;
   out_7829637919891767563[2] = 0;
   out_7829637919891767563[3] = 0;
   out_7829637919891767563[4] = 0;
   out_7829637919891767563[5] = 0;
   out_7829637919891767563[6] = 0;
   out_7829637919891767563[7] = 0;
   out_7829637919891767563[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_2543192712933755862) {
  err_fun(nom_x, delta_x, out_2543192712933755862);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_456116514451130776) {
  inv_err_fun(nom_x, true_x, out_456116514451130776);
}
void car_H_mod_fun(double *state, double *out_8329058010871792226) {
  H_mod_fun(state, out_8329058010871792226);
}
void car_f_fun(double *state, double dt, double *out_7259258364871350970) {
  f_fun(state,  dt, out_7259258364871350970);
}
void car_F_fun(double *state, double dt, double *out_8361658207755630191) {
  F_fun(state,  dt, out_8361658207755630191);
}
void car_h_25(double *state, double *unused, double *out_8066659053161303442) {
  h_25(state, unused, out_8066659053161303442);
}
void car_H_25(double *state, double *unused, double *out_7798991958014807135) {
  H_25(state, unused, out_7798991958014807135);
}
void car_h_24(double *state, double *unused, double *out_1246392366224867809) {
  h_24(state, unused, out_1246392366224867809);
}
void car_H_24(double *state, double *unused, double *out_4072180309103226380) {
  H_24(state, unused, out_4072180309103226380);
}
void car_h_30(double *state, double *unused, double *out_8341853115445809331) {
  h_30(state, unused, out_8341853115445809331);
}
void car_H_30(double *state, double *unused, double *out_8129419157187495854) {
  H_30(state, unused, out_8129419157187495854);
}
void car_h_26(double *state, double *unused, double *out_3340179847784963686) {
  h_26(state, unused, out_3340179847784963686);
}
void car_H_26(double *state, double *unused, double *out_4057488639140750911) {
  H_26(state, unused, out_4057488639140750911);
}
void car_h_27(double *state, double *unused, double *out_8675922967799421912) {
  h_27(state, unused, out_8675922967799421912);
}
void car_H_27(double *state, double *unused, double *out_5905825086003552637) {
  H_27(state, unused, out_5905825086003552637);
}
void car_h_29(double *state, double *unused, double *out_2999985879559778768) {
  h_29(state, unused, out_2999985879559778768);
}
void car_H_29(double *state, double *unused, double *out_7619187812873103670) {
  H_29(state, unused, out_7619187812873103670);
}
void car_h_28(double *state, double *unused, double *out_3674249700138576267) {
  h_28(state, unused, out_3674249700138576267);
}
void car_H_28(double *state, double *unused, double *out_5745157243766917372) {
  H_28(state, unused, out_5745157243766917372);
}
void car_h_31(double *state, double *unused, double *out_6219779230878167226) {
  h_31(state, unused, out_6219779230878167226);
}
void car_H_31(double *state, double *unused, double *out_7829637919891767563) {
  H_31(state, unused, out_7829637919891767563);
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
