#include "OutputKernelFeatures.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#ifndef USE_FC_LEN_T
#define USE_FC_LEN_T
#endif

#include <Rconfig.h>
#include <R_ext/RS.h>
#include <R_ext/Lapack.h>

#ifndef FCONE
#define FCONE
#endif

namespace ranger {
namespace {

std::vector<size_t> uniqueIDs(
    const std::vector<size_t>& ids, size_t n) {
  std::vector<size_t> out(ids);

  if (out.empty()) {
    throw std::runtime_error("Empty root sample.");
    }
  for (size_t id : out) {
    if (id >= n) {
      throw std::runtime_error("Invalid root sample index.");
      }
  }

  std::sort(out.begin(), out.end());
  out.erase(std::unique(out.begin(), out.end()), out.end());
  return out;
  }

double toleranceFor(double tau, double scale, size_t dimension) {
  if (!std::isfinite(tau) || tau < 0) {
    throw std::runtime_error("tau must be finite and non-negative.");
    }
  if (tau > 0) {
    return tau;
    }

  return 64 * std::numeric_limits<double>::epsilon() * scale *
    static_cast<double>(std::max(static_cast<size_t>(1), dimension));
  }

La_INT lapackSize(size_t n) {
  if (n > static_cast<size_t>(std::numeric_limits<La_INT>::max())) {
    throw std::runtime_error("Matrix dimension exceeds LAPACK integer range.");
    }
  return static_cast<La_INT>(n);
  }

} // namespace

size_t OutputFeatures::rowForSample(size_t sample_id) const {
  std::vector<size_t>::const_iterator it =
    std::lower_bound(sample_ids.begin(), sample_ids.end(), sample_id);

  if (it == sample_ids.end() || *it != sample_id) {
    throw std::runtime_error("Sample is not in this tree's root sample.");
    }
  return static_cast<size_t>(it - sample_ids.begin());
  }

OutputFeatures approximateFromGram(
    const std::vector<double>& K,
    size_t n,
    const std::vector<size_t>& root_ids,
    double tau) {
  const std::vector<size_t> ids = uniqueIDs(root_ids, n);
  const size_t m = ids.size();

  if (K.size() != n * n) {
    throw std::runtime_error("K must be n by n.");
  }

  double scale = 0;
  for (size_t i = 0; i < m; ++i) {
    const double diagonal = K[ids[i] + ids[i] * n];
    if (!std::isfinite(diagonal)) {
      throw std::runtime_error("Non-finite K entry.");
    }
    scale = std::max(scale, std::abs(diagonal));
  }

  const double tol = toleranceFor(tau, scale, m);
  const La_INT order = lapackSize(m);
  const La_INT lda = std::max<La_INT>(1, order);
  const char uplo = 'U';

  // DPSTRF overwrites A; copy only the root submatrix into LAPACK layout.
  std::vector<double> a(m * m);
  for (size_t j = 0; j < m; ++j) {
    for (size_t i = 0; i < m; ++i) {
      a[i + j * m] = K[ids[i] + ids[j] * n];
    }
  }

  std::vector<La_INT> pivot(m);
  std::vector<double> work(std::max(static_cast<size_t>(1), 2 * m));
  La_INT rank_lapack = 0;
  La_INT info = 0;
  double lapack_tol = tol;

  F77_CALL(dpstrf)(
      &uplo, &order, a.data(), &lda, pivot.data(),
      &rank_lapack, &lapack_tol, work.data(), &info FCONE);

      if (info < 0) {
        throw std::runtime_error("DPSTRF argument error.");
      }

      const size_t r = static_cast<size_t>(rank_lapack);
      OutputFeatures out;
      out.sample_ids = ids;
      out.rank = r;
      out.z.assign(m * r, 0);

      // Scatter U^T into row-major order; LAPACK pivots are 1-based.
      for (size_t j = 0; j < m; ++j) {
        const size_t row = static_cast<size_t>(pivot[j] - 1);
        for (size_t k = 0; k < r; ++k) {
          out.z[row * r + k] = (k <= j ? a[k + j * m] : 0.0);
        }
      }

      return out;
}

OutputFeatures approximateFromFeatures(
    const std::vector<double>& Phi,
    size_t n,
    size_t d,
    const std::vector<size_t>& root_ids,
    double tau) {
  const std::vector<size_t> ids = uniqueIDs(root_ids, n);
  const size_t m = ids.size();

  if (d == 0 || Phi.size() != n * d) {
    throw std::runtime_error("Phi must be n by d.");
  }

  double scale = 0;
  for (size_t i = 0; i < m; ++i) {
    double norm2 = 0;
    for (size_t j = 0; j < d; ++j) {
      const double value = Phi[ids[i] + j * n];
      if (!std::isfinite(value)) {
        throw std::runtime_error("Non-finite Phi entry.");
      }
      norm2 += value * value;
    }
    scale = std::max(scale, norm2);
  }

  const double tol = toleranceFor(tau, scale, std::max(m, d));

  // DGEQP3 on Phi_root^T (d x m); columns are observations.
  const La_INT rows = lapackSize(d);
  const La_INT cols = lapackSize(m);
  const La_INT lda = std::max<La_INT>(1, rows);
  const La_INT kmax = std::min(rows, cols);

  std::vector<double> a(d * m);
  for (size_t j = 0; j < m; ++j) {
    for (size_t i = 0; i < d; ++i) {
      a[i + j * d] = Phi[ids[j] + i * n];
    }
  }

  std::vector<La_INT> pivot(m, 0);
  std::vector<double> tau_qr(static_cast<size_t>(kmax));
  La_INT info = 0;
  La_INT lwork = -1;
  double work_query = 0;

  F77_CALL(dgeqp3)(
      &rows, &cols, a.data(), &lda, pivot.data(),
      tau_qr.data(), &work_query, &lwork, &info);

  if (info != 0 || !std::isfinite(work_query)) {
    throw std::runtime_error("DGEQP3 workspace query failed.");
  }

  lwork = static_cast<La_INT>(std::ceil(work_query));
  std::vector<double> work(static_cast<size_t>(lwork));

  F77_CALL(dgeqp3)(
      &rows, &cols, a.data(), &lda, pivot.data(),
      tau_qr.data(), work.data(), &lwork, &info);

  if (info != 0) {
    throw std::runtime_error("DGEQP3 failed.");
  }

  // DGEQP3 computes full QR; truncate by squared diagonal entries of R.
  size_t r = 0;
  for (; r < static_cast<size_t>(kmax); ++r) {
    const double diagonal = a[r + r * d];
    if (!std::isfinite(diagonal) || diagonal * diagonal <= tol) {
      break;
    }
  }

  OutputFeatures out;
  out.sample_ids = ids;
  out.rank = r;
  out.z.assign(m * r, 0);

  for (size_t j = 0; j < m; ++j) {
    const size_t row = static_cast<size_t>(pivot[j] - 1);
    for (size_t k = 0; k < r; ++k) {
      const double sign = a[k + k * d] < 0 ? -1.0 : 1.0;
      out.z[row * r + k] =
        sign * (k <= j ? a[k + j * d] : 0.0);
    }
  }

  return out;
}

} // namespace ranger
