#include <TFile.h>
#include <iostream>
#include <TH1F.h>
#include <TCanvas.h>

void inefficiencies(){

  TH1F * WindowLength = new TH1F("WindowLength","Fraction of Events" , 100, 0., 400000.);
  TH1F * Events = new TH1F("Events","Event Trigger Time" , 100, 0., 400000.);
  TH1F * Threshold = new TH1F("Threshold","Fraction of Events" , 100, 0., 400000.);
  TH1F * S1Area = new TH1F("S1Area","Pulse Area" , 100, 0., 400000.);
  TH1F * S1AreaWithVetoes = new TH1F("S1AreaWithVetoes","Pulse Area" , 100, 0., 400000.);
  TH1F * S1AreaWithSkinVetoes = new TH1F("S1AreaWithSkinVetoes","Pulse Area" , 100, 0., 400000.);

  TFile *f1 = new TFile("Ineff.root");
  WindowLength = (TH1F*)f1->Get("SingleScatterWindowLength");
  Events = (TH1F*)f1->Get("SingleScatterEventTime");
  Threshold = (TH1F*)f1->Get("SingleScatterThreshold");
  S1Area = (TH1F*)f1->Get("TPCHGPulseArea");
  S1AreaWithVetoes = (TH1F*)f1->Get("TPCHGPulseAreaWithVetoCuts");
  S1AreaWithSkinVetoes = (TH1F*)f1->Get("TPCHGPulseAreaWithSkinVetoCuts");

  double TotalEvents = S1Area->GetEntries();

  double nEvents;
  for(int i=0; i<100; i++){
    nEvents = WindowLength->GetBinContent(i);
    WindowLength->SetBinContent(i,(nEvents/TotalEvents)+WindowLength->GetBinContent(i-1));
    Threshold->SetBinContent(i, Threshold->GetBinContent(i)/TotalEvents);
  }

  WindowLength->GetXaxis()->SetTitle("Window Length [ns]");
  WindowLength->GetYaxis()->SetTitle("Efficiency [%]");
  WindowLength->SetTitle("");
  WindowLength->Scale(100);

  Threshold->GetXaxis()->SetTitle("Threshold [phd]");
  Threshold->GetYaxis()->SetTitle("Efficiency [%]");
  Threshold->SetTitle("");
  Threshold->Scale(100);


  S1Area->GetXaxis()->SetTitle("PulseArea [phd]");
  S1AreaWithVetoes->SetLineColor(kRed);

  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
  //c1->SetLogy();
  c1->cd();
  WindowLength->Draw("HIST");

  TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
  //c2->SetLogy();
  c2->cd();
  Threshold->Draw("HIST");

  TCanvas *c3 = new TCanvas("c3", "c3", 800, 600);
  c3->SetLogy();
  c3->cd();
  S1Area->Draw("HIST");
  S1AreaWithVetoes->Draw("HIST""SAME");

}
