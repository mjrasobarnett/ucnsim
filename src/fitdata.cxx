#include <iostream>
#include <fstream>
#include <cassert>

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

#include "Constants.h"
#include "Units.h"

using namespace std;

Int_t main(Int_t argc,Char_t **argv)
{
	TString filename;
	if (argc == 2) {
		filename = argv[1];
	} else {
		cerr << "Usage:" << endl;
		cerr << "fitdata <rundata-filename.root> " << endl;
		return -1;
	}
	
	TRint *theApp = new TRint("FittingApp", &argc, argv);
	
	TFile myfile(filename);
	myfile.ls();

	TIter next(myfile.GetListOfKeys()); 
	TKey* key;
	while ((key=(TKey*)next())) {
		printf("key: %s points to an object of class: %s at %i , with cycle number: %i \n", key->GetName(), key->GetClassName(),key->GetSeekKey(),key->GetCycle());
//		myfile.GetObject()
	}
	
	theApp->Run();
	return 0;
}	
