#ifndef __CINT__

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cassert>

#include "TCanvas.h"
#include "TBenchmark.h"
#include "TSystem.h"
#include "TRint.h"
#include "TRandom.h"
#include "TObjArray.h"

#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoTrack.h"
#include "TParticle.h"
#include "TParticlePDG.h"
#include "TGeoMatrix.h"
#include "TPolyMarker3D.h"

#include "TUCNGeoManager.h"
#include "TUCNGeoNavigator.h"
#include "TUCNGeoBuilder.h"
#include "TUCNGeoBBox.h"
#include "TUCNGeoMaterial.h"
#include "TUCNGravField.h"
#include "TUCNParticleCloud.h"
#include "TUCNParticle.h"
#include "TUCNExperiment.h"
#include "TUCNPolynomial.h"
#include "Constants.h"
#include "Units.h"

using std::cout;
using std::endl;

Int_t main(Int_t argc,Char_t **argv)
{
	TRint *theApp = new TRint("UCN App", &argc, argv);
#else 
Int_t ucnstandalone() {
	gSystem->Load("libPhysics");
	gSystem->Load("libGeom");
	gSystem->Load("libEG");
	gSystem->Load("libUCN"); // libUCN needs libGeom and libEG to be loaded first or it will complain. 
#endif

/*	Double_t quadraticRoots[2] = {0., 0.};
	Double_t quadraticParams[3] = {1., 3., -10.};
	
	Int_t quadraticSolutions = TUCNPolynomial::Instance()->QuadraticRootFinder(quadraticParams, quadraticRoots);
	
	cout << "Quadratic Solutions: " << quadraticSolutions << endl;
	cout << "Quadratic Roots: " << quadraticRoots[0] << "\t" << quadraticRoots[1] << endl << endl;
*/	
	
/*	// -- Solve Cubic equation
	Double_t cubicRoots[3] = {0., 0., 0.};
	Double_t cubicParams[4] = {1, -21.25, 20.25, 0}; 
	//{3., -10., 14., 27.}
	//{1.,-4.,1.,0.06}
	//{1., -4., 1., 6.88}
	//{1., 6., 12., 8.}
	//{1., -2., -11., 12.}
		
	Int_t cubicSolutions = TUCNPolynomial::Instance()->CubicRootFinder(cubicParams, cubicRoots);
	cout << "TUCNPolynomial Cubic Solutions: " << cubicSolutions << endl;
	for(Int_t i = 0; i < cubicSolutions; i++) {
		cout << "Root Number " << i + 1 << ": " << cubicRoots[i] << endl;
	}
*/
	
	Double_t quarticRoots[4] = {0., 0., 0., 0.};
	Double_t quarticParams[5] = {1,0,2,6*TMath::Sqrt(10),1};
	
	// {1., 2., -41., -42., 360.};
	
	Int_t quarticSolutions = TUCNPolynomial::Instance()->QuarticRootFinder(quarticParams, quarticRoots);
	
	cout << "Solutions: " << quarticSolutions << endl;
	cout << "Roots: " << quarticRoots[0] << "\t" << quarticRoots[1] << "\t" << quarticRoots[2] << "\t" << quarticRoots[3] <<  endl << endl;
	
					
#ifndef __CINT__
	theApp->Run();
#endif
	return 0;
}