
# Get some errors on compilation which I have never figured out how
# to fix, but it still works ¯\_(ツ)_/¯
Rcpp::sourceCpp("pargeodist.cpp")

# devtools::install_github("njtierney/maxcovr")
library(geodist)
library(maxcovr)

set.seed(10)
n <- 5
x <- cbind (-10 + 20 * stats::runif (n), -10 + 20 * stats::runif (n))
y <- cbind (-10 + 20 * stats::runif (2 * n), -10 + 20 * stats::runif (2 * n))
colnames (x) <- colnames (y) <- c ("x", "y")



test_dist_matrix <- function() {


  parallel_res <- rcpp_parallel_distm_C(x, y) * 1609.34

  geodist_res <- geodist::geodist(x, y, measure = "haversine")
  print(parallel_res)
  print(geodist_res)
}

test_nearest_facility <- function() {
  set.seed(10)
  n <- 5
  x <- cbind (-10 + 20 * stats::runif (n), -10 + 20 * stats::runif (n))
  y <- cbind (-10 + 20 * stats::runif (2 * n), -10 + 20 * stats::runif (2 * n))
  colnames (x) <- colnames (y) <- c ("x", "y")

  parallel_res <- rcpp_parallel_distm_C_min(y, x, 5000)

  maxcovr_res <- maxcovr::nearest_facility_dist(x, y)
  print(parallel_res)
  print(maxcovr_res)
}
