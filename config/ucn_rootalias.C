void set_my_style(void) 
{ 
	cerr <<"------------------------------------------------------------"<< endl;
	cerr <<"- Setting matt's style -" << endl;
	cerr <<"------------------------------------------------------------"<< endl;
	gROOT->SetStyle("Plain");
//	gStyle->SetOptStat(0);
//	gStyle->SetOptTitle(0);
	gStyle->SetOptFit(111);
	gStyle->SetStatW(0.25);
	gStyle->SetStatH(0.25);
	gStyle->SetPadLeftMargin  (0.15);
	gStyle->SetPadRightMargin (0.10);
	gStyle->SetPadBottomMargin(0.15);
	gStyle->SetPadTopMargin   (0.10);
	gStyle->SetPadGridX(1);
	gStyle->SetPadGridY(1);
	gStyle->SetPaperSize(10.,10.);
	gStyle->SetPalette(1,0);
	gStyle->SetTitleSize  (0.03,"XYZ");
	gStyle->SetLabelOffset(0.02,"XYZ");
	gStyle->SetLabelSize  (0.03,"XYZ");
	gStyle->SetTitleOffset(2.00,  "X");
	gStyle->SetTitleOffset(2.00,  "Y");
	gStyle->SetNdivisions(505,"XY");
}

void load_library(TString name)
{
	if ( gSystem->Load(name) == 0 ) {
    cout << "Successfully loaded library:" << name << endl;
  }
	else if ( gSystem->Load(name) > 0 ) {
		cout << "Library " << name << " has already been loaded" << endl;
	}
	else if ( gSystem->Load(name) < 0 ) {
		cout << "Failed to load library:" << name << endl;
	}
}