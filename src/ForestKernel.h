#ifndef FORESTKERNEL_H_
#define FORESTKERNEL_H_

#include <limits>

#include "Forest.h"
#include "ApproxFeatures.h"

namespace ranger {

class ForestKernel: public Forest {
public:
  ForestKernel() = default;
  ForestKernel(const ForestKernel&) = delete;
  ForestKernel& operator=(const ForestKernel&) = delete;
  ~ForestKernel() override = default;

  std::vector<std::vector<std::vector<double>>> getTerminalOutputMeans() const;
  
private:
  void initInternal() override;
  void growInternal() override;
  void allocatePredictMemory() override;
  void predictInternal(size_t sample_idx) override;
  void computePredictionErrorInternal() override;
  void writeOutputInternal() override;
  void writeConfusionFile() override;
  void writePredictionFile() override;
  void saveToFileInternal(std::ofstream& outfile) override;
  void loadFromFileInternal(std::ifstream& infile) override;

  void initializeApproxFeatures();
  const ApproxFeatures* approxFeaturesForTree() const override {
    return &approx_features;
  }

  // Forest-owned approximation initialized before tree growth.
  ApproxFeatures approx_features;
};

} // namespace ranger

#endif /* FORESTKERNEL_H_ */
