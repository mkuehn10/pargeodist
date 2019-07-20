// [[Rcpp::depends(RcppParallel)]]
#define STRICT_R_HEADERS
#include <RcppParallel.h>
#include <Rcpp.h>
using namespace RcppParallel;
using namespace Rcpp;

// [[Rcpp::export]]
double dist_haversine(double lon_x, double lat_x, double lon_y, double lat_y) {
  double R = 6378137.0;
  double phi_1, phi_2, delta_phi, delta_lambda, a, c;

  // convert to radians
  lon_x = lon_x * (M_PI / 180);
  lat_x = lat_x * (M_PI / 180);
  lon_y = lon_y * (M_PI / 180);
  lat_y = lat_y * (M_PI / 180);

  // great-circle distance
  // https://en.wikipedia.org/wiki/Haversine_formula
  phi_1 = lat_x;
  phi_2 = lat_y;
  delta_phi = (lat_y - lat_x);
  delta_lambda = (lon_y - lon_x);
  a = sin(delta_phi / 2.0) * sin(delta_phi / 2.0) +
    cos(phi_1) * cos(phi_2) * sin(delta_lambda / 2.0) *
    sin(delta_lambda / 2.0);
  c = 2.0 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}

struct myDistanceVector : public Worker {
  RMatrix<double> x;
  RMatrix<double> y;
  RVector<int> rvec;

  int miles;

  myDistanceVector(NumericMatrix x, NumericMatrix y, IntegerVector rvec,
                   int miles)
    : x(x), y(y), rvec(rvec), miles(miles) {}

  void operator()(std::size_t begin, std::size_t end) {
    double dist = 0;
    double min = 0;
    int pos = 0;

    for (std::size_t i = begin; i < end; i++) {
      for (std::size_t j = 0; j < y.nrow(); j++) {
        dist = dist_haversine(x(i, 0), x(i, 1), y(j, 0), y(j, 1));

        // if on first position in row, set minimum to the first value
        // set pos to 0 + 1 (adjust by 1 for returning back to R)
        if (j == 0) {
          min = dist;
          pos = j + 1;

          // else if encounter a smaller distance, set min and pos to
          // current distance and position (+1 again to account for array
          // indexing)
        } else if (dist < min) {
          min = dist;
          pos = j + 1;
        }
      }

      // if the minimum distance is greater than miles
      // don't assign an actual position to it
      if ((min * 0.0006213711922) > miles) {
        rvec[i] = 0;
      } else {
        rvec[i] = pos;
      }
    }
  }
};

// [[Rcpp::export]]
IntegerVector rcpp_parallel_distm_C_min(NumericMatrix x, NumericMatrix y,
                                        int miles) {
  IntegerVector rvec(x.nrow());

  myDistanceVector my_distance_vector(x, y, rvec, miles);

  parallelFor(0, x.nrow(), my_distance_vector, 1);

  return rvec;
}

struct myDistanceMatrix : public Worker {
  RMatrix<double> x;
  RMatrix<double> y;
  RMatrix<double> rmat;

  myDistanceMatrix(NumericMatrix x, NumericMatrix y, NumericMatrix rmat)
    : x(x), y(y), rmat(rmat) {}

  void operator()(std::size_t begin, std::size_t end) {
    for (std::size_t i = begin; i < end; i++) {
      for (std::size_t j = 0; j < y.nrow(); j++) {
        double dist = dist_haversine(x(i, 0), x(i, 1), y(j, 0), y(j, 1));
        rmat(i, j) = dist / 1609.34;
      }
    }
  }
};

// [[Rcpp::export]]
NumericMatrix rcpp_parallel_distm_C(NumericMatrix x, NumericMatrix y) {
  NumericMatrix rmat(x.nrow(), y.nrow());

  myDistanceMatrix my_distance_matrix(x, y, rmat);

  parallelFor(0, rmat.nrow(), my_distance_matrix, 1);

  return rmat;
}
