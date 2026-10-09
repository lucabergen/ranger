#include <stdexcept>
#include <vector>

#include "TreeKernel.h"

namespace ranger {

bool TreeKernel::splitNodeInternal(
    size_t nodeID, std::vector<size_t>& possible_split_varIDs) {
  (void) nodeID;
  (void) possible_split_varIDs;
  throw std::logic_error(
      "TreeKernel split search has not been implemented yet.");
}

} // namespace ranger
