
#ifndef OUTPUTKERNELFEATURES_H_
#define OUTPUTKERNELFEATURES_H_

#include <cstddef>
#include <vector>

namespace ranger {

// Projected output features in row-major order; sample IDs are sorted and unique
struct OutputFeatures {
  std::vector<size_t> sample_ids;
  size_t rank;
  std::vector<double> z;

  OutputFeatures() : rank(0) {}

  size_t rowForSample(size_t sample_id) const;

  double operator()(size_t row, size_t feature) const {
    return z[row * rank + feature];
  }
};

// Input matrices are column-major, as in R matrices
OutputFeatures approximateFromGram(
    const std::vector<double>& K,
    size_t n,
    const std::vector<size_t>& root_ids,
    double tau);

OutputFeatures approximateFromFeatures(
    const std::vector<double>& Phi,
    size_t n,
    size_t d,
    const std::vector<size_t>& root_ids,
    double tau);

} // namespace ranger
#endif
