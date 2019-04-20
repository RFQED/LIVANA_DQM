#include <TFile.h>
#include <iostream>
#include <TH1F.h>
#include <TCanvas.h>

void SimpPlots(){

  TH1F * ODEventRate = new TH1F("ODEventRate","ODEventRate" , 100, 1526500000., 1526800000.);

  TFile *f1 = new TFile("../SimpPlots.root");
  ODEventRate = (TH1F*)f1->Get("ODEventRate");
  ODPulseArea = (TH1F*)f1->Get("ODHGPulseArea");

  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
  c1->SetLogy();
  c1->cd();

  ODPulseArea->Draw("HIST");

  TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
  c2->cd();
  ODEventRate->Draw("HIST");

}
