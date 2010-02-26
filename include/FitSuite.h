#ifndef FITSUITE_H
#define FITSUITE_H

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

#include "TRint.h"
#include "TFile.h"
#include "TKey.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TGeoManager.h"
#include "TPolyMarker3D.h"

#include "TUCNExperiment.h"
#include "TUCNFieldManager.h"
#include "TUCNParticle.h"
#include "TUCNRun.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

namespace FitSuite
{
//	Bool_t LoadFile(const TString& configFileName);
	
/*	Bool_t PlotFinalPositions(TFile* f);
	
	
	//______________________________________________________________________________
	Bool_t LoadFile(const string& configFileName, TFile* dataFile) {
		// Read in config file
		TUCNConfigFile ConfigFile(configFileName);
		TString outputFile = ConfigFile.GetString("OutputFile","I/O");
		// Open the TFile containing the ROOT
		if (!(outputFile.Contains(".root"))) {
			cerr << "LoadFile::OutputFile specified is not a ROOT file" << endl;
			return kFALSE;
		} else {
			TFile *f = new TFile(outputFile,"update");
			if (!f || f->IsZombie()) {
			   cerr << "LoadFile::Cannot open file" << endl;
			   return kFALSE;
			} else {
				cerr << "f:" << f << endl;
				dataFile = f;
				cerr << "data:" << dataFile << endl;
				return kTRUE;
			}
		}
	}
	
	//______________________________________________________________________________
	Bool_t DisplayFinalPositions(TFile* f) {
		cout << "Display Final Positions" << endl;
		return kTRUE;
	}
*/	
	
	
}

#endif /* FITSUITE_H */
