#ifndef MyAnalysis_H
#define MyAnalysis_H

#include "Analysis.h"

class MyAnalysis: public Analysis {

public:
  MyAnalysis();
  ~MyAnalysis();
  void Initialize();
  void Execute();
  void Finalize();

private:

};

#endif
