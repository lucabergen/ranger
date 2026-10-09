#ifndef TREEKERNEL_H_
#define TREEKERNEL_H_

#include <vector>

#include "TreeRegression.h"

namespace ranger {

class TreeKernel: public TreeRegression {
public:
  TreeKernel() = default;
  TreeKernel(const TreeKernel&) = delete;
  TreeKernel& operator=(const TreeKernel&) = delete;
  ~TreeKernel() override = default;

private:
  bool splitNodeInternal(size_t nodeID,
      std::vector<size_t>& possible_split_varIDs) override;
};

} // namespace ranger

#endif /* TREEKERNEL_H_ */
