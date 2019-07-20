
<!-- README.md is generated from README.Rmd. Please edit that file -->

# Using RcppParallel to calculate distance matrices proof of concept

<!-- badges: start -->

<!-- badges: end -->

``` r
# devtools::install_github("njtierney/maxcovr")
library(geodist)
#> Warning: package 'geodist' was built under R version 3.6.1
library(maxcovr)
library(microbenchmark)
#> Warning: package 'microbenchmark' was built under R version 3.6.1
library(ggplot2)
library(geosphere)
#> Warning: package 'geosphere' was built under R version 3.6.1
Rcpp::sourceCpp("pargeodist.cpp")
```

``` r
# Create some dummy data to use in calculations
set.seed(10)
n <- 500
x <- cbind (-10 + 20 * stats::runif (n), -10 + 20 * stats::runif (n))
y <- cbind (-10 + 20 * stats::runif (2 * n), -10 + 20 * stats::runif (2 * n))
colnames (x) <- colnames (y) <- c ("x", "y")

geodist_res <- geodist::geodist(x, y, measure = "haversine")
maxcovr_res <- maxcovr::distance_matrix_cpp(y, x)
geosphere_res <- geosphere::distm(x, y, fun = geosphere::distHaversine)
parallel_res <- rcpp_parallel_distm_C(x, y) * 1609.34

all.equal(geodist_res, parallel_res)
#> [1] TRUE
all.equal(geodist_res, maxcovr_res)
#> [1] "Mean relative difference: 0.002942463"
all.equal(maxcovr_res, parallel_res)
#> [1] "Mean relative difference: 0.002945609"
all.equal(geodist_res, geosphere_res)
#> [1] TRUE
all.equal(parallel_res, geosphere_res)
#> [1] TRUE

mb1 <-
  microbenchmark(
    geodist = geodist::geodist(x, y, measure = "haversine"),
    maxcovr = maxcovr::distance_matrix_cpp(y, x),
    geosphere = geosphere::distm(x, y, fun = geosphere::distHaversine),
    parallel = rcpp_parallel_distm_C(x, y) * 1609.34
  )

mb1
#> Unit: milliseconds
#>       expr      min        lq      mean   median        uq      max neval
#>    geodist  79.1307  81.32285  85.91390  84.7408  89.52365 110.0912   100
#>    maxcovr 102.3519 105.18415 109.33866 107.4032 111.31085 128.8871   100
#>  geosphere 276.3577 301.71860 320.56993 312.8268 325.98920 419.1602   100
#>   parallel  16.6982  18.95315  20.77203  20.1964  21.94405  31.1643   100
#>   cld
#>   b  
#>    c 
#>     d
#>  a

autoplot(mb1)
```

![](README_files/figure-gfm/distance_matrix_compare-1.png)<!-- -->

# Comparing to maxcovr::nearest\_facility\_dist functionality

``` r
set.seed(10)
n_users <- 5000
n_sites <- 25
x <- cbind (-10 + 20 * runif (n_users), -10 + 20 * runif (n_users))
y <- cbind (-10 + 20 * runif (2 * n_sites), -10 + 20 * runif (2 * n_sites))
colnames (x) <- colnames (y) <- c ("x", "y")

head(maxcovr::nearest_facility_dist(y, x))
#>      [,1] [,2]      [,3]
#> [1,]    1    2  11377.15
#> [2,]    2    6  41884.89
#> [3,]    3   50 183700.33
#> [4,]    4    4  79928.23
#> [5,]    5   37 190458.37
#> [6,]    6   35 120671.06

# The distances are off since this uses the same Haversine formula, but
# the idea is the same and the nearest site (at least in these limited cases)
# end up being the same
head(rcpp_parallel_nearest_facility(x, y))
#>      [,1] [,2]      [,3]
#> [1,]   50    2  11356.34
#> [2,]   50    6  42026.72
#> [3,]   50   50 183882.49
#> [4,]   50    4  79926.93
#> [5,]   50   37 192083.93
#> [6,]   50   35 121297.81

mb2 <- microbenchmark::microbenchmark(
  maxcovr = maxcovr::nearest_facility_dist(y, x),
  parallel = rcpp_parallel_nearest_facility(x, y)
)

mb2
#> Unit: milliseconds
#>      expr       min        lq      mean    median        uq     max neval
#>   maxcovr 52.345701 53.503301 56.101099 55.111202 58.468601 68.0661   100
#>  parallel  7.829202  8.943952  9.474358  9.185751  9.529701 15.3292   100
#>  cld
#>    b
#>   a

autoplot(mb2)
```

![](README_files/figure-gfm/nearest_facility_compare-1.png)<!-- -->
