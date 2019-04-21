#ifndef DQPlots_H
#define DQPlots_H

#include "Analysis.h"

class DQPlots: public Analysis {

public:
  DQPlots();
  ~DQPlots();
  void Initialize();
  void Execute();
  void Finalize();

private:

};

#endif
