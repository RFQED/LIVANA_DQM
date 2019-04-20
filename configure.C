void configure(TString filename = "data/lz_201703010000_000010_0003_lzap.root") { //data/lz_201704010000_001000_lzap.root") {
  gROOT->ProcessLine(".x common/configure.C(\"" + filename + "\")");
  return;
}
