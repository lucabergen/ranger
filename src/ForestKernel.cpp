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

} // namespace ranger
