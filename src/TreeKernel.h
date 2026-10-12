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

  virtual ~TreeKernel() override = default;

  void allocateMemory() override;

  void appendToFileInternal(std::ofstream& file) override;

private:
  bool splitNodeInternal(size_t nodeID, std::vector<size_t>& possible_split_varIDs) override;
  void createEmptyNodeInternal() override;

  double computePredictionAccuracyInternal(std::vector<double>* prediction_error_casewise) override;

  // Called by splitNodeInternal(). Sets split_varIDs and split_values.
  bool findBestSplit(size_t nodeID, std::vector<size_t>& possible_split_varIDs);
  void findBestSplitValue(size_t nodeID, size_t varID, size_t num_samples_node,
    const std::vector<double>& sum_total, double& best_value, size_t& best_varID, double& best_score);

  // Helper function; computes the projected-feature score of one node
  double kernelScore(const std::vector<double>& sum, size_t n) const;
  // Returns sum of projected-feature scores of left and right child nodes
  double splitScore(const std::vector<double>& sum_left,
      const std::vector<double>& sum_right, size_t n_left,
      size_t n_right) const;

  void addImpurityImportance(size_t nodeID, size_t varID, double best_score, const std::vector<double>& sum_total);

  void cleanUpInternal() override;

};

} // namespace ranger

#endif /* TREEKERNEL_H_ */
