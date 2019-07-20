# Hello, world!
#
# This is an example function named 'hello'
# which prints 'Hello, world!'.
#
# You can learn more about package authoring with RStudio at:
#
#   http://r-pkgs.had.co.nz/
#
# Some useful keyboard shortcuts for package authoring:
#
#   Install Package:           'Ctrl + Shift + B'
#   Check Package:             'Ctrl + Shift + E'
#   Test Package:              'Ctrl + Shift + T'

test_dist_matrix <- function() {
  set.seed(10)
  n <- 5
  x <- cbind (-10 + 20 * stats::runif (n), -10 + 20 * stats::runif (n))
  y <- cbind (-10 + 20 * stats::runif (2 * n), -10 + 20 * stats::runif (2 * n))
  colnames (x) <- colnames (y) <- c ("x", "y")

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
