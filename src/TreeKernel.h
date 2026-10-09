#ifndef TREEKERNEL_H_
#define TREEKERNEL_H_

#include <vector>

#include "Tree.h"

namespace ranger {

class TreeKernel: public Tree {
public:
  TreeKernel() = default;
  TreeKernel(const TreeKernel&) = delete;
  TreeKernel& operator=(const TreeKernel&) = delete;
  ~TreeKernel() override = default;

private:
  void allocateMemory() override;
  void appendToFileInternal(std::ofstream& file) override;
  bool splitNodeInternal(size_t nodeID,
      std::vector<size_t>& possible_split_varIDs) override;
  void createEmptyNodeInternal() override;
  double computePredictionAccuracyInternal(
      std::vector<double>* prediction_error_casewise) override;
  void cleanUpInternal() override;
};

} // namespace ranger

#endif /* TREEKERNEL_H_ */
