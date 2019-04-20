#include <TFile.h>
#include <iostream>
#include <TH1F.h>
#include <TCanvas.h>

void neutronEfficiencies(){

  TH1F * WindowLength = new TH1F("WindowLength","Fraction of Events" , 100, 0., 400000.);
  TH1F * Threshold = new TH1F("Threshold","Fraction of Events" , 100, 0., 400000.);
  TH2F * ThresholdVsWindowLength = new TH2F("ThresholdVsWindowLength","Fraction of Events" , 100, 0., 700000., 100, 0., 1400);
  TH2F * ThresholdVsWindowLengthZoomed = new TH2F("ThresholdVsWindowLengthZoomed","Fraction of Events" , 100, 0., 400000., 100, 0., 400);
  TH2F * TimeDifferenceVsODPulseArea = new TH2F("TimeDifferenceVsODPulseArea","Events" , 100, 0., 400000., 100, 0., 1400);
  //TH1F * S1Area = new TH1F("S1Area","Pulse Area" , 100, 0., 400000.);
  //TH1F * S1AreaWithVetoes = new TH1F("S1AreaWithVetoes","Pulse Area" , 100, 0., 400000.);
  //TH1F * S1AreaWithSkinVetoes = new TH1F("S1AreaWithSkinVetoes","Pulse Area" , 100, 0., 400000.);

  TFile *f1 = new TFile("outputs/neutrons2AmLi041218.root");
  WindowLength = (TH1F*)f1->Get("WindowLength");
  Threshold = (TH1F*)f1->Get("Threshold");
  ThresholdVsWindowLength = (TH2F*)f1->Get("ThresholdVsWindowLength");
  //ThresholdVsWindowLengthZoomed = (TH2F*)f1->Get("ThresholdVsWindowLengthZoomed");
  TimeDifferenceVsODPulseArea = (TH2F*)f1->Get("Abs(SSTimeDifference)VsSSODPulseArea");
  //S1Area = (TH1F*)f1->Get("TPCHGPulseArea");
  //S1AreaWithVetoes = (TH1F*)f1->Get("TPCHGPulseAreaWithVetoCuts");
  //S1AreaWithSkinVetoes = (TH1F*)f1->Get("TPCHGPulseAreaWithSkinVetoCuts");

  double TotalEvents = TimeDifferenceVsODPulseArea->GetEntries();

  for(int i=0; i<WindowLength->GetNbinsX()+1; i++){
    WindowLength->SetBinContent(i,WindowLength->GetBinContent(i)*100/TotalEvents);
    Threshold->SetBinContent(i, Threshold->GetBinContent(i)*100/TotalEvents);
  }

  for(int i=0; i < ThresholdVsWindowLength->GetNbinsX()+1; i++){
    for(int j=0; j < ThresholdVsWindowLength->GetNbinsY(); j++){
      ThresholdVsWindowLength->SetBinContent(i,j,ThresholdVsWindowLength->GetBinContent(i,j)*100/TotalEvents);
      ThresholdVsWindowLengthZoomed->SetBinContent(i,j,ThresholdVsWindowLengthZoomed->GetBinContent(i,j)*100/TotalEvents);
    }
  }

  WindowLength->GetXaxis()->SetTitle("Window Length [ns]");
  WindowLength->GetYaxis()->SetTitle("Efficiency [%]");
  WindowLength->SetTitle("");
  //WindowLength->Scale(100);

  Threshold->GetXaxis()->SetTitle("Threshold [phd]");
  Threshold->GetYaxis()->SetTitle("Efficiency [%]");
  Threshold->SetTitle("");
  //Threshold->Scale(100);

  /*  ThresholdVsWindowLengthZoomed->GetXaxis()->SetTitle("Window Length [ns]");
  ThresholdVsWindowLengthZoomed->GetYaxis()->SetTitle("Threshold [phd]");
  ThresholdVsWindowLengthZoomed->GetZaxis()->SetTitle("Efficiency [%]");
  ThresholdVsWindowLengthZoomed->SetTitle("");
  */
  ThresholdVsWindowLength->GetXaxis()->SetTitle("Window Length [us]");
  ThresholdVsWindowLength->GetYaxis()->SetTitle("Threshold [phd]");
  ThresholdVsWindowLength->GetZaxis()->SetTitle("Efficiency [%]");
  ThresholdVsWindowLength->SetTitle("");
  //S1Area->GetXaxis()->SetTitle("PulseArea [phd]");
  //S1AreaWithVetoes->SetLineColor(kRed);

  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
  //c1->SetLogy();
  c1->cd();
  WindowLength->Draw("HIST");

  TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
  //c2->SetLogy();
  c2->cd();
  Threshold->Draw("HIST");

  TCanvas *c3 = new TCanvas("c3", "c3", 800, 600);
  //c3->SetLogy();
  c3->cd();
  ThresholdVsWindowLength->Draw("COLZ");
  //S1AreaWithVetoes->Draw("HIST""SAME");
  //  TCanvas *c4 = new TCanvas("c4", "c4", 800, 600);
  //c3->SetLogy();
  //  c4->cd();
  //  ThresholdVsWindowLengthZoomed->Draw("COLZ");
}
