
<!-- README.md is generated from README.Rmd. Please edit that file -->

# Using RcppParallel to calculate distance matrices proof of concept

<!-- badges: start -->

<!-- badges: end -->

``` r
# devtools::install_github("njtierney/maxcovr")
library(geodist)
library(maxcovr)
library(microbenchmark)
library(ggplot2)
library(geosphere)
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
#>       expr      min        lq      mean   median       uq      max neval
#>    geodist  78.4149  80.40535  83.65290  81.7729  85.3753 106.7524   100
#>    maxcovr 100.3360 103.50390 105.99087 104.5579 105.9978 181.9124   100
#>  geosphere 268.4761 276.77770 285.78689 280.1866 285.2736 389.7740   100
#>   parallel  16.5436  16.93295  18.47658  17.2765  19.0490  38.5175   100
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
#> [1,]    1    2  11356.34
#> [2,]    2    6  42026.72
#> [3,]    3   50 183882.49
#> [4,]    4    4  79926.93
#> [5,]    5   37 192083.93
#> [6,]    6   35 121297.81

mb2 <- microbenchmark::microbenchmark(
  maxcovr = maxcovr::nearest_facility_dist(y, x),
  parallel = rcpp_parallel_nearest_facility(x, y)
)

mb2
#> Unit: milliseconds
#>      expr     min       lq      mean  median     uq     max neval cld
#>   maxcovr 51.2825 52.98555 54.387980 53.5355 54.624 62.0949   100   b
#>  parallel  7.6778  7.81520  8.141604  7.9338  8.257 10.8152   100  a

autoplot(mb2)
```

![](README_files/figure-gfm/nearest_facility_compare-1.png)<!-- -->
