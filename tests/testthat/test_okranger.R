library(ranger)
context("okranger")

test_that("Phi OOB predictions are a matrix and match the reported error", {
  
  n <- 60
  x <- data.frame(x1 = runif(n), x2 = runif(n))
  Phi <- cbind(
    feature1 = x$x1 + x$x2,
    feature2 = x$x1^2,
    feature3 = sin(x$x2)
  )
  
  fit <- OKranger(Phi = Phi, x = x, approx.tolerance = 1e-8,
                           oob.error = TRUE)
  
  expect_true(is.matrix(fit$predictions))
  expect_equal(dim(fit$predictions), c(n, ncol(Phi)))
  expect_equal(colnames(fit$predictions), colnames(Phi))
  
  valid <- apply(is.finite(fit$predictions), 1, all)
  expect_true(any(valid))
  
  expected_error <- mean(rowSums(
    (fit$predictions[valid, , drop = FALSE] -Phi[valid, , drop = FALSE])^2
  ))
  
  expect_equal(fit$prediction.error, expected_error, tolerance = 1e-10)
})

test_that("oob.error = FALSE skips OOB outputs", {

  n <- 30
  x <- data.frame(x1 = runif(n), x2 = runif(n))
  Phi <- cbind(feature1 = x$x1, feature2 = x$x2)
  
  fit <- OKranger(
    Phi = Phi,
    x = x,
    approx.tolerance = 1e-8,
    num.trees = 5,
    seed = 31,
    verbose = FALSE,
    write.forest = FALSE,
    oob.error = FALSE
  )
  
  expect_null(fit$predictions)
  expect_null(fit$prediction.error)
})

test_that("Node statistics have correct output means for every Phi node", {
  
  n <- 30
  x <- data.frame(x1 = runif(n), x2 = runif(n))
  Phi <- cbind(feature1 = x$x1, feature2 = x$x2)
  
  fit <- OKranger(Phi = Phi, x = x, approx.tolerance = 1e-8, 
                          node.stats = TRUE, keep.inbag = TRUE)
  
  expect_equal(length(fit$forest$terminal.output.means), fit$num.trees)
  
  for (tree_index in seq_len(fit$num.trees)) {
    tree_means <- fit$forest$terminal.output.means[[tree_index]]
    expect_true(length(tree_means) > 0L)
    expect_true(all(lengths(tree_means) == ncol(Phi)))
    
    # The root mean must use in-bag counts, including bootstrap duplicates
    inbag_counts <- fit$inbag.counts[[tree_index]]
    expected_root_mean <- unname(colSums(Phi * inbag_counts) / sum(inbag_counts))
    
    expect_equal(tree_means[[1]], expected_root_mean, tolerance = 1e-10)
  }
})

test_that("Gram input requires OOB computation to be disabled in this phase", {
  
  n <- 20
  x <- data.frame(x1 = runif(n), x2 = runif(n))
  Phi <- cbind(x$x1^2, x$x2^3)
  K <- tcrossprod(Phi)
  
  expect_error(OKranger(K = K, x = x, approx.tolerance = 1e-8,
                                 num.trees = 3, verbose = FALSE, oob.error = TRUE),
               "supported only when 'Phi' is supplied")
})

test_that("Phi OOB predictions equal regression forest OOB predictions", {
  
  n <- 300
  x <- matrix(rnorm(n*7,0,1),ncol = 7)
  y <- 0.6*x[,1] - 0.2*x[,2] + 0.3*x[,4] + 0.9*x[,5] - 0.5*x[,6] + rnorm(n)
  x <- as.data.frame(x)
  Phi <- matrix(y, ncol = 1)
  
  fit_y <- ranger(y = y, x = x, 
                          write.forest = TRUE, keep.inbag = TRUE,
                          oob.error = TRUE, seed = 123)
  
  fit_Phi <- OKranger(Phi = Phi, x = x, 
                              approx.tolerance = 1e-16,
                              oob.error = TRUE, seed = 123)
  
  expect_equal(fit_y$predictions, as.vector(fit_Phi$predictions),
               tolerance = 1e-10)
})

test_that("Phi OOB predictions equal probability forest OOB predictions", {
  
  n <- 300
  x <- matrix(rnorm(n * 7, 0, 1), ncol = 7)
  eta <- 0.8 * x[, 1] - 0.5 * x[, 3] + 0.6 * x[, 5] - 0.3 * x[, 6]
  cuts <- quantile(eta + rnorm(n), probs = c(0, 1/3, 2/3, 1), names = FALSE)
  labels <- c("low", "middle", "high")
  y <- cut(eta, breaks = cuts, include.lowest = TRUE, labels = labels)
  x <- as.data.frame(x)
  class_order <- unique(as.character(y))
  # One-got encode y
  Phi <- vapply(class_order, function(cl) as.numeric(y == cl), 
                numeric(length(y)))
  colnames(Phi) <- class_order
  
  fit_prob <- ranger(y = y, x = x, probability = TRUE,
                                    mtry = 7, min.node.size = 3, min.bucket = 1, 
                                    oob.error = T, seed = 123)
  
  fit_Phi <- OKranger(Phi = Phi, x = x, approx.tolerance = 1e-16, 
                              mtry = 7, min.node.size = 3, min.bucket = 1, 
                              oob.error = T, seed = 123)
  
  expect_equal(fit_prob$predictions[, labels, drop = FALSE],
               fit_Phi$predictions[, labels, drop = FALSE],
               tolerance = 1e-10)
})
