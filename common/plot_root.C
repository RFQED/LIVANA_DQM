/**
 * # Processing macro 
 * Example of processing macro. 
 * This macro reads all the files listed in the file `RQfile.list` and generate an output rootfile `macro.root`.
 * ## Usage: 
 * Run the plot macro:
 * ```bash
 * # In compilated mode:
 * root plot_root.C+ 
 * # or in interpreted mode:
 * root plot_root.C
 * ```
 *
 *
 * The macro will creat and display the figure. In this macro, you can do all the treatment that you want on the style of the figure, scaling, comparison, simple calculation, ...
 *
 *---
 *# Description of the macro
 * 
 * In the following section, each part of the macro will be described. The major blocks are: 
 * 1. [Headers] (#Headers)
 * 2. [Main executable] (#main)
 *
 */

/**
 * ## Headers <a name="Headers"></a>
 * Include all the headers
 *
 */
// Standard C++ headers
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

// ROOT Headers
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"
#include "THStack.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TMinuit.h"
#include "TColor.h"
#include "TLine.h"
#include "TLatex.h"
#include "TSystem.h"

// STD namespace
using namespace std;



/**
 * ## Main executable <a name="main"></a>
 *
 * Main executable
 *
 * 
 */
void plot_root(){


  /**
   * ROOT file opening
   */
  TFile* f = TFile::Open("macro.root");
  
  /**
   * Get the 1D and 2D histograms form the ROOTFIle.
   * When you use the method f->Get(char* name), ROOT return a TObject. In order to manipulate it, you have to cas it as a TH1F.
   */  
  TH1F* h_nHGs1 = (TH1F*)f->Get("h_nHGs1");
  TH1F* h_nHGs2 = (TH1F*)f->Get("h_nHGs2");

  TH1F* h_HGs1 = (TH1F*)f->Get("h_HGs1");
  TH1F* h_HGs2 = (TH1F*)f->Get("h_HGs2");

  TH2F* h2_areawidth_pulses = (TH2F*)f->Get("h2_areawidth_pulses");
  TH2F* h2_HGs1s2 = (TH2F*)f->Get("h2_HGs1s2");


  
  /**
   * Remove the stat box
   */
  h_nHGs1->SetStats(0);
  h_nHGs2->SetStats(0);
  h_HGs1->SetStats(0);
  h_HGs2->SetStats(0);
  h2_areawidth_pulses->SetStats(0);
  h2_HGs1s2->SetStats(0);

  
  
  /**
   * Modify the style of the histogram
   */

  h_nHGs1->SetLineColor(10);
  h_nHGs1->SetLineWidth(2);
  h_nHGs1->SetXTitle("n S1 pulses");
  h_nHGs1->SetYTitle("count");

  h_nHGs2->SetLineColor(11);
  h_nHGs2->SetLineWidth(2);
  h_nHGs2->SetXTitle("n S2 pulses");
  h_nHGs2->SetYTitle("count");

  h_HGs1->SetLineColor(10);
  h_HGs1->SetLineWidth(2);
  h_HGs1->SetXTitle("S1 pulses [phd]");
  h_HGs1->SetYTitle("count");

  h_HGs2->SetLineColor(11);
  h_HGs2->SetLineWidth(2);
  h_HGs2->SetXTitle("S2 pulses [phd]");
  h_HGs2->SetYTitle("count");

  h2_areawidth_pulses->SetXTitle("pulse area [phd]");
  h2_areawidth_pulses->SetYTitle("pulse width [ns]");

  h2_HGs1s2->SetXTitle("reconstructed S1 [phd]");
  h2_HGs1s2->SetYTitle("reconstructed S2 [phd]");


  /**
   *  Graphical and text object
   */
  // Object to draw a line
  TLine* line = new TLine;
  line->SetLineStyle(7);
  
  // Object to draw text
  TLatex* latex = new TLatex;
  
  /**
   * ## Draw the figures
   */  
  TCanvas * c;

  // Create a new canvas with all the arguments custom
  c = new TCanvas("c","Fill Area colors",0,0,1000,400);
  // Draw the color table in the canvas
  c->DrawColorTable();
  // Save the plot in a png picture. Available output format [.png,.pdf,.eps,...]
  c->SaveAs("doc/color_table.png");

  /** 
   * The code block above draw the color table used for this tutorial. This is not the LZ official color table. This color table is used to set the color of the lines and the markers.
   *
   * How to use it: 
   * 1. What is you favorite color ? Blue ? Red ? 
   * 2. Set the color of the histogram line: `hitogram->SetLineColor(10);`
   * 
   * ![logo](../color_table.png)
   */
  
  // Create a new canvas with all the default arguments  
  c = new TCanvas;
  h2_areawidth_pulses->Draw("colz");

  // Add the threshold for s1 and s2 pulse identification
  line->DrawLine(h2_areawidth_pulses->GetXaxis()->GetXmin(), 250, h2_areawidth_pulses->GetXaxis()->GetXmax(), 250);
  line->DrawLine(200, h2_areawidth_pulses->GetYaxis()->GetXmin(), 200, h2_areawidth_pulses->GetYaxis()->GetXmax());

  // Add some text on the plot
  latex->DrawLatex(2000, 255, "Pulse width cut");
  latex->DrawLatex(220, 1600, "Area threshold");

  // Set the z axis in log scale
  c->SetLogz();

  // Save the plot in a png picture. Available output format [.png,.pdf,.eps,...]
  c->SaveAs("doc/areawidth_pulses.png");

  /** 
   *
   *  There are no pulse classification for now.
   *  Rough pulse identification:
   * - S1 = area_phd > 200 && width < 250
   * - S2 = area_phd > 200 && width > 250
   *
   * ![logo](../areawidth_pulses.png)
   */

  // THStack is a plot class for histogram that adjust automatically the Y scale of the figure
  THStack * hs_npulses = new THStack;
  // Add the histogram on the THStack
  hs_npulses->Add(h_nHGs1);
  hs_npulses->Add(h_nHGs2);

  // TLegend is the class that draw the legend for some graph
  TLegend* leg_npulses = new TLegend(0.5,0.7,0.7,0.8);
  // Add some legend for some histograms
  leg_npulses->AddEntry(h_nHGs1, "S1 pulses", "l");
  leg_npulses->AddEntry(h_nHGs2, "S2 pulses", "l");

  
  // Create a new canvas with all the default arguments  
  c = new TCanvas;
  // Draw the THStack, the option nostack is neeeded to avoid to sum all the histogram in the THStack
  hs_npulses->Draw("nostack");
  
  // Draw the legend on the top of that with the option "same"
  leg_npulses->Draw("same");

  // The the x and y title. In the case of the THStack, you have to do it after the Draw
  hs_npulses->GetXaxis()->SetTitle("Number of pulses");
  hs_npulses->GetYaxis()->SetTitle("Count");

  // Set the y axis in log scale
  c->SetLogy();
  
  // Save the plot in a png picture. Available output format [.png,.pdf,.eps,...]
  c->SaveAs("doc/npulses.png");


  /** 
   * ![logo](../npulses.png)
   */

  // THStack is a plot class for histogram that adjust automatically the Y scale of the figure
  THStack * hs_s1s2 = new THStack;
  // Add the histogram on the THStack
  hs_s1s2->Add(h_HGs1);
  hs_s1s2->Add(h_HGs2);
  
  // TLegend is the class that draw the legend for some graph
  TLegend* leg_s1s2 = new TLegend(0.15,0.6,0.45,0.8);
  // Add some legend for some histograms
  leg_s1s2->AddEntry(h_HGs1, "S1 pulses");
  leg_s1s2->AddEntry(h_HGs2, "S2 pulses");
  
  // Create a new canvas with all the default arguments  
  c = new TCanvas;

  hs_s1s2->Draw("nostack");

  // Draw the legend on the top of that with the option "same"
  leg_s1s2->Draw("same");

  // The the x and y title. In the case of the THStack, you have to do it after the Draw
  hs_s1s2->GetXaxis()->SetTitle("S area [phd]");
  hs_s1s2->GetYaxis()->SetTitle("Count");

  // Set the x axis in log scale
  c->SetLogx();
  
  // Save the plot in a png picture. Available output format [.png,.pdf,.eps,...]
  c->SaveAs("doc/spectra_s1s2.png");


  /** 
   * ![logo](../spectra_s1s2.png)
   */

  c = new TCanvas;
  // Create a new canvas with all the default arguments  
  h2_HGs1s2->Draw("colz");

  // Save the plot in a png picture. Available output format [.png,.pdf,.eps,...]
  c->SaveAs("doc/s1s2.png");

  /** 
   * ![logo](../s1s2.png)
   * ![logo](https://lz-git.ua.edu/qriffard/lz_mdc2_tutorial/blob/master/mdc2_like/doc/areawidth_pulses.png)
   */
  
  
  /**
   * ### End of the macro
   */  

} 
