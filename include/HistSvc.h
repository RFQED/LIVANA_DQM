#ifndef HistSvc_h
#define HistSvc_h

#include <TH1.h>
#include <TFile.h>
#include <TTree.h>

#include <map>
using std::map;
#include <string>
using std::string;
#include <string>
using std::vector;
//#include <tr1/unordered_map>
//using std::trl::unordered_map;
// For C++11
#include <unordered_map>
using std::unordered_map;

class HistSvc {

public:

  HistSvc(string prefix = "");
  ~HistSvc();

  void BookFillHist(const string& name, int nbinsx, float xlow, float xup, float value, float weight=1);
  TH1* BookHist(const string& name, int nbinsx, float xlow, float xup);
 
  TH1* BookHist(const string& name, int nbinsx, float* xbins);
  TH1* BookHist(const string& name, int nbinsx, float xlow, float xup, int nbinsy, float ylow, float yup);
  TH1* BookHist(const string& name, int nbinsx, float* xbins, int nbinsy, float* ybins);

  void BookFillHist(const string& name, int nbinsx, float* xbins, float value, float weight=1);
  void BookFillHist(const string& name, int nbinsx, float xlow, float xup, int nbinsy, float ylow, float yup, float xvalue, float yvalue, float weight=1);
  void BookFillHist(const string& name, int nbinsx, float* xbins, int nbinsy, float* ybins, float xvalue,  float yvalue, float weight=1);

  
  

  TTree* BookTree(const string& name, const string& branchName, void* address, const std::string& leaves);
  void BookFillTree(const string& name, const string& branchName, void* address, const std::string& leaves);
  TTree* BookTree(const string& name, const string& branchName, double* address);
  void BookFillTree(const string& name, const string& branchName, double* address);

  TH1* BookCutHist(const string& name, int nbinsx, string cuts[]);
  void BookFillCutHist(const string& name, int nbinsx, string cuts[], string label, float weight);

  TH1* BookCutHist(const string& name, int nbinsx, string cutsx[], int nbinsy, string cutsy[]);
  void BookFillCutHist(const string& name, int nbinsx, string cutsx[], int nbinsy, string cutsy[], const string& labelx, const string& labely, float weight);

  TH1* BookRunHist(const string& name, int nbinsx, int runs[]);
  void BookFillRunHist(const string& name, int nbinsx, int runs[], int run, float weight);

  void Write(TFile* file);

  void SetPrefix(string prefix);
  void SetDir(string dir);
  void SetSysName(string sysname);
  string Prefix(){return m_prefix;}
  string SysName(){return m_sysname;}
  string Dir(){return m_dir;}


  void CutFlow(bool cutflow) {m_cutflow = cutflow;}

  void Reset();
  TH1* FindHistInMap(const string& name);
  void SetWeightSystsNames(vector<string>& weightsystname);
  void SetWeightSysts(vector<float>& weightsyst);


  // simply add a hist directly
  void AddHist(const string& name, TH1* hist) {
    if (!hist) return;
    hist->SetName(name.c_str());
    string fullname = m_dir+m_sysname+m_prefix+name+m_sysnamesuf;
    m_hists[fullname]=hist;
  }

private:

  //typedef map<string, TH1*> store_t;  
  //store_t* m_hists;

  //  typedef unordered_map<string, unordered_map <string, array<TH1*,10>>> store_t;  
  //  unordered_map<string, unordered_map <string, unordered_map<int, TH1*>>> m_hists ;  
  unordered_map<string, TH1*> m_hists ;  
  //  store_t m_hists;

  typedef unordered_map<string, TTree*> treestore_t;  
  treestore_t* m_trees;
  
  int m_nweightsyst;
  vector<std::string>  m_weightsystname;
  vector <float>   m_weightsyst;

  string m_prefix; 
  string m_dir; 
  string m_sysname; 
  string m_sysnamesuf;
  bool m_cutflow;

  string histName(const string name);
  string dirName(const string name);
  string addSlash(const string& path);


  
  void split(const string& s, char delim, vector<string>& elems);
  vector<string> split(const string &s, char delim);
};

#endif
