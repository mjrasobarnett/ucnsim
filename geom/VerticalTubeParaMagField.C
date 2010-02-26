#include "../include/Units.h"
#include "../include/Constants.h"

Int_t VerticalTubeParaMagField()
{
	// -- Simple Tube example for Mike P, that includes Wall Losses and finding the average field in a parabolic mag field
	
	// Loading useful libraries (if they aren't already loaded)
	gSystem->Load("libPhysics");
	gSystem->Load("libGeom");
	gSystem->Load("libEG");
	gSystem->Load("libUCN");
	
	// Create the geoManager
	TUCNGeoManager* geoManager = new TUCNGeoManager("GeoManager", "Geometry Manager");
	// Create a file which we will store the geometry in
	TFile* file = new TFile("geom.root","RECREATE");
	
	// -------------------------------------
	// BUILDING GEOMETRY
	cerr << "Building Geometry..." << endl;
	// Input parameters to calculate f  -- All numbers come from Mike P (presumable from data), for a specific energy group, from which we can calculate f. 
	Double_t observedLifetime = 150.*Units::s;
	Double_t fermiPotential = (0.91*Units::m)*Constants::height_equivalent_conversion;
	Double_t totalEnergy = (0.52*Units::m)*Constants::height_equivalent_conversion;
	Double_t initialVelocity = TMath::Sqrt(2.*totalEnergy/Constants::neutron_mass);
	Double_t meanFreePath = 0.16*Units::m;
	
	// Calculate f = W/V and hence W
	Double_t X = totalEnergy/fermiPotential;
	Double_t L = 2.0*((1./X)*TMath::ASin(TMath::Sqrt(X)) - TMath::Sqrt((1./X) - 1.));
	Double_t f = meanFreePath/(initialVelocity*observedLifetime*L);
	Double_t W = f*fermiPotential;
	cerr << "Input Parameters: " << endl;
	cerr << "E: " << totalEnergy/Units::neV << "\t" << "V: " << fermiPotential/Units::neV << "\t" << "E/V: " << X << "\t"<< "L: " << L << endl;
	cerr << "f: " << f << "\t" << "W: " << W/Units::neV << endl;
	
	// Materials
	TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", fermiPotential, W);
	
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackHole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = geoManager->MakeUCNBox("TOP",blackHole,20,20,20);
	geoManager->SetTopVolume(chamber);
				
	// -- Make a GeoTube object via the UCNGeoManager
	Double_t rMin = 0.0, rMax = 0.236, length = 0.121; 
	TGeoVolume* tube   = geoManager->MakeUCNTube("tube",boundary, rMin, rMax, length/2.);
	TGeoVolume* innerTube  = geoManager->MakeUCNTube("innerTube",vacuum, rMin, rMax-0.001, (length-0.001)/2.);
	
	
	// -- Define the transformation of the volume
	TGeoRotation r1,r2; 
	r1.SetAngles(0,0,0);          //rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,length/2.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	TGeoVolume* volume = tube;
	TGeoVolume* innerVolume = innerTube;
	
	// -- Create the nodes	
	volume->AddNode(innerVolume,1);
	chamber->AddNode(volume,1, matrix);
	
	// -- Define the Source in our geometry where we will create the particles
	geoManager->SetSourceVolume(innerVolume);
	geoManager->SetSourceMatrix(matrix);
	
	// -- Arrange and close geometry
   geoManager->CloseGeometry();
	
	// -- Write out geometry to file
	cerr << "Geometry Built... Writing to file: " << file->GetName() << endl;
	geoManager->Write();
	file->ls();
	file->Close();
	
	geoManager->GetTopVolume()->Draw();
	
	return 0;
}
