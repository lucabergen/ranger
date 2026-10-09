#ifndef FORESTKERNEL_H_
#define FORESTKERNEL_H_

#include "ForestRegression.h"

namespace ranger {

class ForestKernel: public ForestRegression {
public:
  ForestKernel() = default;
  ForestKernel(const ForestKernel&) = delete;
  ForestKernel& operator=(const ForestKernel&) = delete;
  ~ForestKernel() override = default;

private:
  void initInternal() override;
  void growInternal() override;
};

} // namespace ranger

#endif /* FORESTKERNEL_H_ */
