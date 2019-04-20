void configure(TString filename = "/projecta/projectdirs/lz/data/MDC2/background/LZAP-3.1.0-PHYSICS-3.1.0/20170401/lz_201704010000_lzap.root")
{
  TFile* f = TFile::Open(filename);
  f->MakeProject("rqlib","*","update+");
}
