#include <fstream>
#include <stdexcept>
#include <vector>

#include "TreeKernel.h"

namespace ranger {

void TreeKernel::allocateMemory() {
}

void TreeKernel::appendToFileInternal(std::ofstream& file) {
  (void) file;
  throw std::logic_error("TreeKernel serialization is not implemented yet.");
}

bool TreeKernel::splitNodeInternal(
    size_t nodeID, std::vector<size_t>& possible_split_varIDs) {
  (void) nodeID;
  (void) possible_split_varIDs;
  throw std::logic_error("TreeKernel split search is not implemented yet.");
}

void TreeKernel::createEmptyNodeInternal() {
}

double TreeKernel::computePredictionAccuracyInternal(
    std::vector<double>* prediction_error_casewise) {
  (void) prediction_error_casewise;
  throw std::logic_error("TreeKernel prediction accuracy is not implemented yet.");
}

void TreeKernel::cleanUpInternal() {
}

} // namespace ranger
