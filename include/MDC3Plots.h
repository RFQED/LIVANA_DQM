#ifndef MDC3Plots_H
#define MDC3Plots_H

#include "Analysis.h"

class MDC3Plots: public Analysis {

public:
  MDC3Plots();
  ~MDC3Plots();
  void Initialize();
  void Execute();
  void Finalize();

private:

};

#endif
