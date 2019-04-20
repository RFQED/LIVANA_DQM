#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TTree.h"
#include <iostream>
#include "TProfile.h"
#include "TGraphErrors.h"
#include <cmath>
#include <cstdio>
#include "TROOT.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "TPaveLabel.h"
#include "TPad.h"
#include "TFile.h"
#include "TString.h"
#include "TSystem.h"
#include <vector>
#include "TXMLEngine.h"
#include <algorithm>    // std::max
#include <ctime>
 
void dqplots(char* filename, int fnum);

// for 20, 25, 30, 35, 40, 45 ns trigger widths
void dqplots(char* filename, int fnum)
{

  // current date/time based on current system
  time_t now = time(0);
  
  // convert now to tm struct for UTC
  tm *gmtm = gmtime(&now);
  
  char strday[20];
  char strtime[20];
  sprintf(strday,"%04d%02d%02d",gmtm->tm_year+1900,gmtm->tm_mon+1,gmtm->tm_mday);
  sprintf(strtime,"%02d:%02d:%02d",gmtm->tm_hour,gmtm->tm_min,gmtm->tm_sec);
  
  gROOT->SetBatch(kTRUE);

  // XML file with metadata

  // First create engine
  TXMLEngine* xml = new TXMLEngine;
  // Create main node of document tree
  XMLNodePointer_t dqid = xml->NewChild(0, 0, "DQID");

  TFile *_file0 = TFile::Open(filename);

  char cdir[100];
  sprintf(cdir,"LZDQ_Miniproduction_%d",fnum);

  xml->NewAttr(dqid, 0, "id",cdir);
  XMLNodePointer_t meta = xml->NewChild(dqid, 0, "META");   // node with metadata
  xml->NewChild(meta, 0, "SIMDATA", "SIMULATION");
  xml->NewChild(meta, 0, "STARTDAY", strday);
  xml->NewChild(meta, 0, "STARTTIME", strtime);

  char mdir[100];
  sprintf(mdir,"mkdir LZDQ_Miniproduction_%d",fnum);

  gSystem->Exec(mdir);    // create a directory to save the results

  char fullroot[200];
  sprintf(fullroot,"LZDQ_Miniproduction_%d/LZDQ_Miniproduction_%d.root",fnum,fnum);

  char fullxml[200];
  sprintf(fullxml,"LZDQ_Miniproduction_%d/dqsetmeta.xml",fnum);    

  TFile *fileout = TFile::Open(fullroot,"recreate");    // root file with histograms

  TTree* tree = (TTree*)_file0->Get("Events");        // LZap tree
  TCanvas *c1 = new TCanvas("c1", "c1",600,500);  

  // colors 
  vector<TString> flags;
  flags.push_back("Green");
  flags.push_back("Yellow");
  flags.push_back("Red");
  flags.push_back("Undefined");


  // bulk histograms

  vector<TString> param;
  param.push_back("pulseArea_phd");
  param.push_back("peakTime_ns");
  param.push_back("coincidence");

  vector<TString> paramobj;
  paramobj.push_back("h1");
  paramobj.push_back("h1");
  paramobj.push_back("h1");

  vector<TString> paramover;   // Can be overlapped or not with the reference dqset
  paramover.push_back("yes");
  paramover.push_back("yes");
  paramover.push_back("yes");

  // hist range
  vector<double> hlow;
  hlow.push_back(-20.0);
  hlow.push_back(0.);
  hlow.push_back(0.);
  vector<double> hhigh;
  hhigh.push_back(500.0);
  hhigh.push_back(4000.0);
  hhigh.push_back(500.0);


  // alg range
  vector<double> algmean;
  algmean.push_back(86.76);
  algmean.push_back(460.6);
  algmean.push_back(191.);
  vector<double> algstd;
  algstd.push_back(106.8);
  algstd.push_back(551.8);
  algstd.push_back(151.);


  // list of subsystems
  vector<TString> subdet;
  subdet.push_back("TPCHG");
  subdet.push_back("TPCLG");
  subdet.push_back("Skin");
  subdet.push_back("ODHG");
  subdet.push_back("ODLG");

  // loop over subsystems
  for (vector<TString>::iterator its = subdet.begin(); its != subdet.end(); its++){

    // subsystem node 
    XMLNodePointer_t subdetXML = xml->NewChild(dqid, 0, "SUBDET");
    xml->NewAttr(subdetXML, 0, "id",its->Data());
    //    xml->NewAttr(subdetXML, 0, "flag","Yellow");
    const char* subd = its->Data();

    int subflag = 0;

    size_t vit=0;
    // loop over parameters
    for (vector<TString>::iterator itp = param.begin(); itp != param.end(); itp++){

      // parameter node
      XMLNodePointer_t par = xml->NewChild(subdetXML, 0, "PAR");
      xml->NewAttr(par, 0, "id",itp->Data());                     // parameter name
      xml->NewAttr(par, 0, "obj",paramobj[vit].Data());           // object (histogram, etc)
      xml->NewAttr(par, 0, "overlap",paramover[vit].Data());      // overlap with reference object or not
      xml->NewAttr(par, 0, "drawoption","default");               // can change the drawing option

      // draw histogram
      const char* para =  itp->Data();
      char htitle[100];
      sprintf(htitle,"%s_%s",subd,para);         // histogram name (should be subsystem_parameter)
      char drawcommand[200];
      sprintf(drawcommand,"pulses%s.%s>>%s",subd,para,htitle);               
      TH1F* hist = new TH1F(htitle,htitle,100,hlow[vit],hhigh[vit]);
      tree->Draw(drawcommand,"");
      hist->SetLineColor(4);
      hist->Sumw2();
      hist->Scale(1./hist->Integral());
      hist->Draw("e");
      char printname[200];
      sprintf(printname,"%s/%s.%s.png",cdir,subd,para);        // image file should be subsystem.parameter.png
      c1->Print(printname);
      hist->Write();         


      // algorithm node
      XMLNodePointer_t alg = xml->NewChild(par, 0, "ALG");
      xml->NewAttr(alg, 0, "name","MEAN");
      double mn = hist->GetMean();
      char xmlcont[30];
      sprintf(xmlcont,"%5.2f",mn);
      xml->NewChild(alg, 0, "VALUE", xmlcont);
      sprintf(xmlcont,"%5.2f",algmean[vit]-20.);
      xml->NewChild(alg, 0, "LOW2", xmlcont);
      sprintf(xmlcont,"%5.2f",algmean[vit]-10.);
      xml->NewChild(alg, 0, "LOW", xmlcont);
      sprintf(xmlcont,"%5.2f",algmean[vit]+10.);
      xml->NewChild(alg, 0, "HIGH", xmlcont);
      sprintf(xmlcont,"%5.2f",algmean[vit]+20.);
      xml->NewChild(alg, 0, "HIGH2", xmlcont);
      int iflagmn = 0; // green

      // DQ checks  (mean outside of range)
      if (mn < algmean[vit]-20 || mn > algmean[vit]+20) { 
	iflagmn = 2; // red
      } else if (mn < algmean[vit]-10 || mn > algmean[vit]+10) {
	iflagmn = 1; // yellow;
      }

      xml->NewChild(alg, 0, "FLAG", flags[iflagmn].Data());

      // another algorithm
      alg = xml->NewChild(par, 0, "ALG");
      xml->NewAttr(alg, 0, "name","StD");
      double sig = hist->GetRMS();
      sprintf(xmlcont,"%5.2f",sig);
      xml->NewChild(alg, 0, "VALUE", xmlcont);
      sprintf(xmlcont,"%5.2f",algstd[vit]-20.);
      xml->NewChild(alg, 0, "LOW2", xmlcont);
      sprintf(xmlcont,"%5.2f",algstd[vit]-10.);
      xml->NewChild(alg, 0, "LOW", xmlcont);
      sprintf(xmlcont,"%5.2f",algstd[vit]+10.);
      xml->NewChild(alg, 0, "HIGH", xmlcont);
      sprintf(xmlcont,"%5.2f",algstd[vit]+20.);
      xml->NewChild(alg, 0, "HIGH2", xmlcont);
      int iflagsig = 0; // green
      
      if (sig < algstd[vit]-20 || sig > algstd[vit]+20) {
	iflagsig = 2; // red
      } else if (sig < algstd[vit]-10 || sig > algstd[vit]+10) {
	iflagsig = 1; // yellow;
      }

      xml->NewChild(alg, 0, "FLAG", flags[iflagsig].Data());

      int totalflag = std::max(iflagmn,iflagsig);   // total flag for the parameter is the worst one
      xml->NewAttr(par, 0, "flag",flags[totalflag].Data());

      subflag = std::max(subflag,totalflag);
  
      vit++;
    }
    xml->NewAttr(subdetXML, 0, "flag",flags[subflag].Data());
    
    // additional histograms/plots
    // 2d plot for TPCHG and TPCLG
    if (its->CompareTo("TPCHG") == 0 || its->CompareTo("TPCLG") == 0 ){
      
      XMLNodePointer_t par = xml->NewChild(subdetXML, 0, "PAR");
      xml->NewAttr(par, 0, "id","s2yVSs2x");
      xml->NewAttr(par, 0, "obj","h2");
      xml->NewAttr(par, 0, "overlap","no");
      xml->NewAttr(par, 0, "drawoption","colz");
      xml->NewAttr(par, 0, "flag","Undefined");
      
      char drawcommand[200];
      sprintf(drawcommand,"pulses%s.s2Xposition_cm:pulses%s.s2Yposition_cm>>hnew2",subd,subd);
      char selectcommand[200];
      sprintf(selectcommand,"pulses%s.s2Xposition_cm<200",subd);
      tree->Draw(drawcommand,selectcommand,"colz");
      TH1F *hnew2 = (TH1F*)gDirectory->Get("hnew2");
      char htitle[100];
      sprintf(htitle,"%s_s2yVSs2x",subd);
      hnew2->SetTitle(htitle);
      hnew2->SetName(htitle);
      char printname[200];
      sprintf(printname,"%s/%s.s2yVSs2x.png",cdir,subd);
      c1->Print(printname);
      hnew2->Write();
      
      XMLNodePointer_t alg = xml->NewChild(par, 0, "ALG");
      xml->NewAttr(alg, 0, "name","N/A");
      xml->NewChild(alg, 0, "VALUE", "N/A");
      xml->NewChild(alg, 0, "LOW2", "N/A");
      xml->NewChild(alg, 0, "LOW", "N/A");
      xml->NewChild(alg, 0, "HIGH", "N/A");
      xml->NewChild(alg, 0, "HIGH2", "N/A");
      xml->NewChild(alg, 0, "FLAG", "Undefined");
    }
    // 2d plots for ODHG and ODLG
    if (its->CompareTo("ODHG") == 0 || its->CompareTo("ODLG") == 0 ){
      
      XMLNodePointer_t par = xml->NewChild(subdetXML, 0, "PAR");
      xml->NewAttr(par, 0, "id","ZvsPhi");
      xml->NewAttr(par, 0, "obj","h2");
      xml->NewAttr(par, 0, "overlap","no");
      xml->NewAttr(par, 0, "drawoption","colz");
      xml->NewAttr(par, 0, "flag","Undefined");

      char htitle[100];
      sprintf(htitle,"%s_ZvsPhi",subd);
      TH2F* ZvsPhi = new TH2F(htitle,htitle,20,-0.5,20.5,6,0.5,6.5);

      char drawcommand[200];
      sprintf(drawcommand,"pulses%s.pulseZPosition:pulses%s.pulsePhiPosition>>%s",subd,subd,htitle);
      tree->Draw(drawcommand,"","colz");
      char printname[200];
      sprintf(printname,"%s/%s.ZvsPhi.png",cdir,subd);
      c1->Print(printname);
      ZvsPhi->Write();
      
      XMLNodePointer_t alg = xml->NewChild(par, 0, "ALG");
      xml->NewAttr(alg, 0, "name","N/A");
      xml->NewChild(alg, 0, "VALUE", "N/A");
      xml->NewChild(alg, 0, "LOW2", "N/A");
      xml->NewChild(alg, 0, "LOW", "N/A");
      xml->NewChild(alg, 0, "HIGH", "N/A");
      xml->NewChild(alg, 0, "HIGH2", "N/A");
      xml->NewChild(alg, 0, "FLAG", "Undefined");
    }
    
  }

  // additional histograms/plots
  // Events

  // Global subsystem
  XMLNodePointer_t subdetXML = xml->NewChild(dqid, 0, "SUBDET");
  xml->NewAttr(subdetXML, 0, "id","Global");
  xml->NewAttr(subdetXML, 0, "flag","Green");
  
  XMLNodePointer_t par = xml->NewChild(subdetXML, 0, "PAR");
  xml->NewAttr(par, 0, "id","triggerRate");
  xml->NewAttr(par, 0, "obj","h1");
  xml->NewAttr(par, 0, "overlap","no");
  xml->NewAttr(par, 0, "drawoption","default");
  xml->NewAttr(par, 0, "flag","Undefined");
  
  tree->Draw("eventHeader.triggerTimeStamp_s-1491177602>>hnew");
  
  TH1F *hnew = (TH1F*)gDirectory->Get("hnew");
  hnew->SetTitle("Global_triggerRate");
  hnew->SetName("Global_triggerRate");
  char printname[200];
  sprintf(printname,"%s/Global.triggerRate.png",cdir);
  c1->Print(printname);
  hnew->Write();
  
  XMLNodePointer_t alg = xml->NewChild(par, 0, "ALG");
  xml->NewAttr(alg, 0, "name","SpikeFinder");
  xml->NewChild(alg, 0, "VALUE", "N/A");
  xml->NewChild(alg, 0, "LOW2", "N/A");
  xml->NewChild(alg, 0, "LOW", "N/A");
  xml->NewChild(alg, 0, "HIGH", "N/A");
  xml->NewChild(alg, 0, "HIGH2", "N/A");
  xml->NewChild(alg, 0, "FLAG", "Undefined");
    

  // current date/time based on current system
  now = time(0);
  
  // convert now to tm struct for UTC
  tm *gmtm2 = gmtime(&now);
  
  sprintf(strday,"%04d%02d%02d",gmtm2->tm_year+1900,gmtm2->tm_mon+1,gmtm2->tm_mday);
  sprintf(strtime,"%02d:%02d:%02d",gmtm2->tm_hour,gmtm2->tm_min,gmtm2->tm_sec);

  xml->NewChild(meta, 0, "ENDDAY", strday);
  xml->NewChild(meta, 0, "ENDTIME", strtime);
  xml->NewChild(meta, 0, "TAG", "BKG");

  // same xml file
  XMLDocPointer_t xmldoc = xml->NewDoc();   
  xml->DocSetRootElement(xmldoc, dqid);
  // Save document to file
  xml->SaveDoc(xmldoc, fullxml);
  // Release memory before exit
  xml->FreeDoc(xmldoc);
  delete xml;

  fileout->Close();
}
