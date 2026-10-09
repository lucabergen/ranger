#ifndef FORESTKERNEL_H_
#define FORESTKERNEL_H_

#include "Forest.h"

namespace ranger {

class ForestKernel: public Forest {
public:
  ForestKernel() = default;
  ForestKernel(const ForestKernel&) = delete;
  ForestKernel& operator=(const ForestKernel&) = delete;
  ~ForestKernel() override = default;

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
};

} // namespace ranger

#endif /* FORESTKERNEL_H_ */
