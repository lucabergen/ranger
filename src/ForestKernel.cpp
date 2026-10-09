#include <fstream>
#include <stdexcept>

#include "ForestKernel.h"
#include "TreeKernel.h"

namespace ranger {

void ForestKernel::initInternal() {
  throw std::logic_error("ForestKernel is not implemented yet.");
}

void ForestKernel::growInternal() {
  trees.reserve(num_trees);
  for (size_t i = 0; i < num_trees; ++i) {
    trees.push_back(std::make_unique<TreeKernel>());
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
