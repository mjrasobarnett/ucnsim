void set_my_style(void) 
{ 
	cerr <<"------------------------------------------------------------"<< endl;
	cerr <<"- Setting matt's style -" << endl;
	cerr <<"------------------------------------------------------------"<< endl;
	gROOT->SetStyle("Plain");
	gStyle->SetOptStat(1);
	gStyle->SetOptFit(111);
//	gStyle->SetStatW(0.15);
//	gStyle->SetStatH(0.15);
//	gStyle->SetPadLeftMargin  (0.15);
//	gStyle->SetPadRightMargin (0.10);
//	gStyle->SetPadBottomMargin(0.15);
//	gStyle->SetPadTopMargin   (0.10);
	gStyle->SetPadGridX(1);
	gStyle->SetPadGridY(1);	
	gStyle->SetPalette(1,0);
	gStyle->SetLabelOffset(0.01,"XYZ"); // Axis numbers
	gStyle->SetLabelSize  (0.03,"XYZ"); // Axis numbers
	gStyle->SetTitleSize  (0.04,"XYZ"); // Axis Titles
   gStyle->SetTitleOffset(1.0,"XYZ");	
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