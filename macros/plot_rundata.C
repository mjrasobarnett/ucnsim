#include <iostream>

#include "TGLViewer.h"
#include "TGLCamera.h"
#include "TGLPerspectiveCamera.h"

#include "include/Constants.h"
#include "include/Units.h"
#include "include/FitSuite.h"


Bool_t PlotInitialAndFinalPositions(const TString& dataFileName, const TString& runName);
Bool_t PlotInitialAndFinalAngularDistribution(const TString& dataFileName, const TString& runName);
Bool_t PlotInitialAndFinalPhaseSpace(const TString& dataFileName, const TString& runName); 

// -------------------------------------------------------------------------------------- 
Int_t plot_rundata(const char* dataFileName, const char* runName) {


   PlotInitialAndFinalPositions(dataFileName, runName);
	PlotInitialAndFinalAngularDistribution(dataFileName, runName);
	PlotInitialAndFinalPhaseSpace(dataFileName, runName); 

	return 0;
}


// -------------------------------------------------------------------------------------- 
Bool_t PlotInitialAndFinalPositions(const TString& dataFileName, const TString& runName) 
{
// -- Create a TPolyMarker3D object to store the final positions of the neutrons and write this to file. 
	cout << "-------------------------------------------" << endl;
	cout << "PlotInitialAndFinalPositions" <<  endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(dataFileName, "update");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << dataFileName << endl;
	   return 0;
	}
	// -- Extract Run Object
	cout << "Fetching run: " << runName << endl;
	TUCNRun* run = new TUCNRun();
   file->GetObject(static_cast<const char*>(runName), run);
	if (run == NULL) {
		cerr << "Could not find run: " << runName << endl;
		return kFALSE;
	}
	cout << "Successfully Loaded Run: " << runName << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Create the initial points
	TPolyMarker3D* initialPoints = new TPolyMarker3D(run->Neutrons(), 1); // 1 is marker style
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		TUCNParticle* particle = run->GetInitialParticle(i);
		if (particle == NULL) continue;
		initialPoints->SetPoint(i, particle->X(), particle->Y(), particle->Z());
	}
	cout << "Created Initial Points" << endl;
	initialPoints->SetMarkerColor(2);
	initialPoints->SetMarkerStyle(6);
	// -- Write the points to the File
	initialPoints->SetName("NeutronInitialPositions");
	initialPoints->Write();
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Create the final points
	TPolyMarker3D* finalPoints = new TPolyMarker3D(run->Neutrons(), 1); // 1 is marker style
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		TUCNParticle* particle = run->GetParticle(i);
		if (particle == NULL) continue;
		finalPoints->SetPoint(i, particle->X(), particle->Y(), particle->Z());
	}
	cout << "Created Final Points" << endl;
	finalPoints->SetMarkerColor(2);
	finalPoints->SetMarkerStyle(6);
	// -- Write the points to the File
	finalPoints->SetName("NeutronFinalPositions");
	finalPoints->Write();
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Clean Up
	delete run;
	delete initialPoints; 
	delete finalPoints;
	delete file;
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotInitialAndFinalAngularDistribution(const TString& dataFileName, const TString& runName) 
{
// -- Create a Histogram object to store the angular distribution (as in, their initial and final directions about the origin). 
	cout << "-------------------------------------------" << endl;
	cout << "PlotInitialAndFinalAngularDistribution" <<  endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(dataFileName, "update");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << dataFileName << endl;
	   return 0;
	}
	// -- Extract Run Object
	TUCNRun* run = new TUCNRun();
   file->GetObject(static_cast<const char*>(runName), run);
	if (run == NULL) {
		cerr << "Could not find run: " << runName << endl;
		return kFALSE;
	}
	// -- Plot the Initial and Final Directions
	Int_t nbins = 50;
	TH1F* initialThetaHist = new TH1F("InitialThetaHist","Initial Direction: Theta component, Units of Pi", nbins, 0.0, 1.0);
	TH1F* initialPhiHist = new TH1F("InitialPhiHist","Initial Direction: Phi component, Units of Pi", nbins, 0.0, 2.0);
	TH1F* finalThetaHist = new TH1F("FinalThetaHist","Final Direction: Theta component, Units of Pi", nbins, 0.0, 1.0);
	TH1F* finalPhiHist = new TH1F("FinalPhiHist","Final Direction: Phi component, Units of Pi", nbins, 0.0, 2.0);
	// Axis Titles
	initialThetaHist->SetXTitle("Height from bottom of Tube (m)");
	initialThetaHist->SetYTitle("Number of Neutrons");
	
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		TUCNParticle* initialParticle = run->GetInitialParticle(i);
		if (initialParticle == NULL) continue;
		TUCNParticle* finalParticle = run->GetParticle(i);
		if (finalParticle == NULL) continue;
		initialThetaHist->Fill(initialParticle->Theta()/TMath::Pi());
		initialPhiHist->Fill(initialParticle->Phi()/TMath::Pi());
		finalThetaHist->Fill(finalParticle->Theta()/TMath::Pi());
		finalPhiHist->Fill(finalParticle->Phi()/TMath::Pi());
	}
	// -- Write the points to the File
	initialThetaHist->Write();
	initialPhiHist->Write();
	finalThetaHist->Write();
	finalPhiHist->Write();
	
	delete initialThetaHist;
	delete initialPhiHist;
	delete finalThetaHist;
	delete finalPhiHist;
	delete run;
	delete file;
	
	return kTRUE;
}

// -------------------------------------------------------------------------------------- 
Bool_t PlotInitialAndFinalPhaseSpace(const TString& dataFileName, const TString& runName) 
{
// -- Create a Histogram object to store the angular distribution (as in, their initial and final directions about the origin). 
	cout << "-------------------------------------------" << endl;
	cout << "PlotInitialAndFinalPhaseSpace" <<  endl;
	cout << "-------------------------------------------" << endl;
	///////////////////////////////////////////////////////////////////////////////////////
	// -- Open File
	TFile *file = 0;
	file = TFile::Open(dataFileName, "update");
	if (!file || file->IsZombie()) {
	   cerr << "Cannot open file: " << dataFileName << endl;
	   return 0;
	}
	// -- Extract Run Object
	TUCNRun* run = new TUCNRun();
   file->GetObject(static_cast<const char*>(runName), run);
	if (run == NULL) {
		cerr << "Could not find run: " << runName << endl;
		return kFALSE;
	}
	
   Double_t maximumEnergy = 300*Units::neV;
   Double_t maximumMomentum = 20*Units::eV;
   Double_t runTime = 100*Units::s;
   Double_t maximumDistance = 1000*Units::m;
	
	// -- Plot the Initial Phase Space
	Int_t nbins = 50;
	TH1F* initialEnergyHist = new TH1F("InitialEnergyHist","Initial Energy: Units of neV", nbins, 0.0, maximumEnergy);
	initialEnergyHist->SetXTitle("Energy (neV)");
	initialEnergyHist->SetYTitle("Neutrons");
	TH1F* initialPxHist = new TH1F("InitialPxHist","Initial Px: Units of eV", nbins, -maximumMomentum, maximumMomentum);
	initialPxHist->SetXTitle("Px (eV)");
	initialPxHist->SetYTitle("Neutrons");
	TH1F* initialPyHist = new TH1F("InitialPyHist","Initial Py: Units of eV", nbins, -maximumMomentum, maximumMomentum);
	initialPyHist->SetXTitle("Py (eV)");
	initialPyHist->SetYTitle("Neutrons");
   TH1F* initialPzHist = new TH1F("InitialPzHist","Initial Pz: Units of eV", nbins, -maximumMomentum, maximumMomentum);
	initialPzHist->SetXTitle("Pz (eV)");
	initialPzHist->SetYTitle("Neutrons");
   TH1F* initialTimeHist = new TH1F("InitialTimeHist","Initial Time: Units of s", nbins, 0.0, runTime);
	initialTimeHist->SetXTitle("Time (s)");
	initialTimeHist->SetYTitle("Neutrons");
   TH1F* initialDistHist = new TH1F("InitialDistHist","Initial Distance Travelled: Units of m", nbins, 0.0, maximumDistance);
	initialDistHist->SetXTitle("Distance (m)");
	initialDistHist->SetYTitle("Neutrons");

	// -- Plot the Final Phase Space
	TH1F* finalEnergyHist = new TH1F("FinalEnergyHist","Final Energy: Units of neV", nbins, 0.0, maximumEnergy);
	finalEnergyHist->SetXTitle("Energy (neV)");
	finalEnergyHist->SetYTitle("Neutrons");
	TH1F* finalPxHist = new TH1F("FinalPxHist","Final Px: Units of eV", nbins, -maximumMomentum, maximumMomentum);
	finalPxHist->SetXTitle("Px (eV)");
	finalPxHist->SetYTitle("Neutrons");
	TH1F* finalPyHist = new TH1F("FinalPyHist","Final Py: Units of eV", nbins, -maximumMomentum, maximumMomentum);
	finalPyHist->SetXTitle("Py (eV)");
	finalPyHist->SetYTitle("Neutrons");
   TH1F* finalPzHist = new TH1F("FinalPzHist","Final Pz: Units of eV", nbins, -maximumMomentum, maximumMomentum);
	finalPzHist->SetXTitle("Pz (eV)");
	finalPzHist->SetYTitle("Neutrons");
   TH1F* finalTimeHist = new TH1F("FinalTimeHist","Final Time: Units of s", nbins, 0.0, runTime);
	finalTimeHist->SetXTitle("Time (s)");
	finalTimeHist->SetYTitle("Neutrons");
   TH1F* finalDistHist = new TH1F("FinalDistHist","Final Distance Travelled: Units of m", nbins, 0.0, maximumDistance);
	finalDistHist->SetXTitle("Distance (m)");
	finalDistHist->SetYTitle("Neutrons");
	
	for (Int_t i = 0; i < run->Neutrons(); i++) {
		TUCNParticle* initialParticle = run->GetInitialParticle(i);
		if (initialParticle == NULL) continue;
		TUCNParticle* finalParticle = run->GetParticle(i);
		if (finalParticle == NULL) continue;
		initialEnergyHist->Fill(initialParticle->Energy());
	   initialPxHist->Fill(initialParticle->Px()/Units::eV);
		initialPyHist->Fill(initialParticle->Py()/Units::eV);
	   initialPzHist->Fill(initialParticle->Pz()/Units::eV);
	   initialTimeHist->Fill(initialParticle->T()/Units::s);
	   initialDistHist->Fill(initialParticle->Distance()/Units::m);
	   
		finalEnergyHist->Fill(finalParticle->Energy());
	   finalPxHist->Fill(finalParticle->Px()/Units::eV);
		finalPyHist->Fill(finalParticle->Py()/Units::eV);
	   finalPzHist->Fill(finalParticle->Pz()/Units::eV);
	   finalTimeHist->Fill(finalParticle->T()/Units::s);
	   finalDistHist->Fill(finalParticle->Distance()/Units::m);
	}
   
   // Fit Final Time to Exponential - determine emptying time
   TF1 *expo = new TF1("Expo", "expo", 5., runTime);
   finalTimeHist->Fit("Expo","R");
   Double_t emptyingTime = expo->GetParameter(0);
	Double_t emptyingTimeError = expo->GetParError(0);
	cout << "EmptyingTime: " << emptyingTime << "\t" << "Error: " << emptyingTimeError << endl;
   expo->Write();
   
	// -- Write the histograms to the File
   initialEnergyHist->Write();
   initialPxHist->Write();
   initialPyHist->Write();
   initialPzHist->Write();
   initialTimeHist->Write();
   initialDistHist->Write();
   finalEnergyHist->Write();
   finalPxHist->Write();
   finalPyHist->Write();
   finalPzHist->Write();
   finalTimeHist->Write();
   finalDistHist->Write();
	
	delete initialEnergyHist;
   delete initialPxHist;
   delete initialPyHist;
   delete initialPzHist;
   delete initialTimeHist;
   delete initialDistHist;
   delete finalEnergyHist;
   delete finalPxHist;
   delete finalPyHist;
   delete finalPzHist;
   delete finalTimeHist;
   delete finalDistHist;
   delete run;
	delete file;
	
	return kTRUE;
}


