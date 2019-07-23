
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
n <- 5000
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
#> [1] "Mean relative difference: 0.002858014"
all.equal(maxcovr_res, parallel_res)
#> [1] "Mean relative difference: 0.00286122"
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
#> Unit: seconds
#>       expr       min        lq      mean    median        uq       max
#>    geodist  8.370764  8.610591  8.750009  8.735267  8.876369  9.282945
#>    maxcovr 10.096687 10.299988 10.691648 10.583608 11.017114 11.627757
#>  geosphere 19.944979 20.426758 21.037501 20.990131 21.516573 23.482105
#>   parallel  1.728775  1.758184  1.878221  1.824284  1.945294  2.494596
#>  neval  cld
#>    100  b  
#>    100   c 
#>    100    d
#>    100 a

autoplot(mb1)
```

![](README_files/figure-gfm/distance_matrix_compare-1.png)<!-- -->

# Comparing to maxcovr::nearest\_facility\_dist functionality

``` r
set.seed(10)
n_users <- 20000
n_sites <- 250
x <- cbind (-10 + 20 * runif (n_users), -10 + 20 * runif (n_users))
y <- cbind (-10 + 20 * runif (2 * n_sites), -10 + 20 * runif (2 * n_sites))
colnames (x) <- colnames (y) <- c ("x", "y")

head(maxcovr::nearest_facility_dist(y, x))
#>      [,1] [,2]      [,3]
#> [1,]    1  143  66317.70
#> [2,]    2  321  55602.46
#> [3,]    3  338  69220.63
#> [4,]    4  241  59111.24
#> [5,]    5  228 100389.84
#> [6,]    6  275  25261.91

# The distances are off since this uses the same Haversine formula, but
# the idea is the same and the nearest site (at least in these limited cases)
# end up being the same
head(rcpp_parallel_nearest_facility(x, y))
#>      [,1] [,2]      [,3]
#> [1,]    1  143  65976.83
#> [2,]    2  321  55665.52
#> [3,]    3  338  69315.58
#> [4,]    4  241  59162.38
#> [5,]    5  228 100673.45
#> [6,]    6  275  25368.55

mb2 <- microbenchmark::microbenchmark(
  maxcovr = maxcovr::nearest_facility_dist(y, x),
  parallel = rcpp_parallel_nearest_facility(x, y)
)

mb2
#> Unit: milliseconds
#>      expr       min        lq      mean   median        uq       max neval
#>   maxcovr 2063.5472 2097.4554 2131.3308 2124.951 2140.3765 2355.9278   100
#>  parallel  309.2106  317.2172  328.0342  329.000  336.7175  359.1827   100
#>  cld
#>    b
#>   a

autoplot(mb2)
```

![](README_files/figure-gfm/nearest_facility_compare-1.png)<!-- -->
