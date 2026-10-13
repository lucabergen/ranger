#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>

#include "ForestKernel.h"
#include "TreeKernel.h"
#include "Data.h"

namespace ranger {

void ForestKernel::initInternal() {
  if (prediction_mode) {
    throw std::runtime_error("Prediction mode for OKranger is not implemented yet.");
  }

  if (output_representation != OUTPUT_REPRESENTATION_FEATURES && output_representation != OUTPUT_REPRESENTATION_GRAM) {
    throw std::runtime_error("ForestKernel requires Phi or K output data.");
  }

  // Use the regression forest defaults for tree-growth parameters
  if (mtry == 0) {
    unsigned long temp = sqrt((double) num_independent_variables);
    mtry = std::max((unsigned long) 1, temp);
  }

  if (min_node_size.size() == 1 && min_node_size[0] == 0) {
    min_node_size[0] = DEFAULT_MIN_NODE_SIZE_REGRESSION;
  }

  if (min_bucket.size() == 1 && min_bucket[0] == 0) {
    min_bucket[0] = DEFAULT_MIN_BUCKET;
  }

  // Use the regression forests predictor preprocessing
  if (!memory_saving_splitting) {
    data->sort();
  }

  // Construct a forest-level feature approximation on the full training data
  initializeApproxFeatures();
}

void ForestKernel::initializeApproxFeatures() {
  const size_t n = data->getNumRows();
  const size_t output_cols = data->getNumColsY();

  if (output_representation == OUTPUT_REPRESENTATION_FEATURES) {
    approx_features = approximateFromFeatures(*data, n, output_cols, approx_tolerance);
  } else if (output_representation == OUTPUT_REPRESENTATION_GRAM) {
    approx_features = approximateFromGram(*data, n, approx_tolerance);
  } else {
    // This indicates an internal dispatch/configuration error; R validates the
    // user-facing arguments before constructing a ForestKernel
    throw std::logic_error("ForestKernel requires Phi or K output data.");
  }
}

void ForestKernel::growInternal() {
  trees.reserve(num_trees);
  for (size_t i = 0; i < num_trees; ++i) {
    trees.push_back(std::make_unique<TreeKernel>(output_representation));
  }
}

void ForestKernel::allocatePredictMemory() {
  throw std::logic_error("ForestKernel prediction is not implemented yet.");
}

void ForestKernel::predictInternal(size_t sample_idx) {
  (void) sample_idx;
  throw std::logic_error("ForestKernel prediction is not implemented yet.");
}

void ForestKernel::computePredictionErrorInternal() {
  throw std::logic_error("ForestKernel OOB prediction is not implemented yet.");
}

void ForestKernel::writeOutputInternal() {
  if (verbose_out) {
    *verbose_out << "Tree type:                         Kernel regression" << std::endl;
  }
}

void ForestKernel::writeConfusionFile() {
  // Kernel regression has no confusion matrix.
}

void ForestKernel::writePredictionFile() {
  throw std::logic_error("ForestKernel prediction output is not implemented yet.");
}

void ForestKernel::saveToFileInternal(std::ofstream& outfile) {
  (void) outfile;
  throw std::logic_error("ForestKernel serialization is not implemented yet.");
}

void ForestKernel::loadFromFileInternal(std::ifstream& infile) {
  (void) infile;
  throw std::logic_error("ForestKernel loading is not implemented yet.");
}

} // namespace ranger
