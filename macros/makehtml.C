#include "THtml.h"

void makehtml() {
	gSystem->Load("libGeom");
	gSystem->Load("libEG");
	gSystem->Load("libPhysics");
	
	gSystem->Load("libUCN");

  	THtml html;
	html.SetProductName("UCNSIM");
	html.MakeAll();
}