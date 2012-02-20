void set_my_style(void) 
{ 
   cout <<"==================================================="<< endl;
   cout <<"           Setting Matt's Default Style            "<< endl;
   cout <<"==================================================="<< endl;
   gROOT->SetStyle("Modern");
   gStyle->SetCanvasBorderMode(0);     // turn off canvas borders
   gStyle->SetPadBorderMode(0);
   
   gStyle->SetPadTopMargin(0.12); // Set Pad Margins
   gStyle->SetPadRightMargin(0.08);
   gStyle->SetPadBottomMargin(0.15);
   gStyle->SetPadLeftMargin(0.14);
   gStyle->SetPadGridX(1); // Set Grid lines on plot
   gStyle->SetPadGridY(1);
   
   gStyle->SetPalette(1,0); // blue to red false color palette. Use 9 for b/w
   
   gStyle->SetStatX(0.94); // Set Stat Box position and size
   gStyle->SetStatY(0.98);
   gStyle->SetStatW(0.15);
   gStyle->SetStatH(0.10);
   gStyle->SetOptStat(11);
   gStyle->SetOptFit(1111);
   
   gStyle->SetTitleSize(0.03,"XYZ"); // Set Axis Sizes 
   gStyle->SetLineWidth(1.5); // Axis Line Width
   gStyle->SetTitleOffset(1.8,"x"); // X Axis Title offset
   gStyle->SetTitleOffset(1.6,"y"); // Y Axis Title offset
   gStyle->SetTitleOffset(1.8,"z"); // Z Axis Title offset
   
   gStyle->SetLabelOffset(0.01,"XYZ"); // Axis Label offset / size
   gStyle->SetLabelSize(0.03,"XYZ");
}

void load_library(TString name)
{
	if ( gSystem->Load(name) == 0 ) {
    cout << "Successfully loaded library: " << name << endl;
  }
	else if ( gSystem->Load(name) > 0 ) {
		cout << "Library " << name << " has already been loaded" << endl;
	}
	else if ( gSystem->Load(name) < 0 ) {
		cout << "Failed to load library:" << name << endl;
	}
}
