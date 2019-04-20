#ifndef DQAnalysis_H
#define DQAnalysis_H

#include "Analysis.h"

class DQAnalysis: public Analysis {

public:
  DQAnalysis();
  ~DQAnalysis();
  void Initialize();
  void Execute();
  void Finalize();

private:

};

#endif
