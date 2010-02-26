// TUCNGeoManager
// Author: Matthew Raso-Barnett  22/05/2009
#include <iostream>
#include <fstream>

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TVirtualGeoTrack.h"
#include "TParticle.h"

#include "TUCNGeoBuilder.h"
#include "TUCNGeoNavigator.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "TUCNParticleCloud.h"
#include "TUCNParabola.h"
#include "Units.h"

#include "TUCNGeoManager.h"

using std::cout;
using std::endl;

//#define VERBOSE_MODE


ClassImp(TUCNGeoManager)

//_____________________________________________________________________________
	TUCNGeoManager::TUCNGeoManager() 
	:TGeoManager()
{
// Default Constructor
	Info("TUCNGeoManager", "Default Constructor");
	fUCNNavigator = NULL;		
	fGravField = NULL;
	fParticleCloud = NULL;
	fNavigatorIndex = 0; 				
}

//_____________________________________________________________________________
TUCNGeoManager::TUCNGeoManager(const char *name, const char *title) 
	:TGeoManager(name, title)
{
// Constructor
	Info("TUCNGeoManager","UCNGeometry %s, %s created", this->GetName(), this->GetTitle());
	fGravField = NULL;
	fParticleCloud = NULL;
	fNavigatorIndex = 0;
	fUCNNavigator = NULL;
	// Create GeoNavigator -- Navigator is created here so that we couple it to the UCNManager straight away. 
	TUCNGeoNavigator* navigator = new TUCNGeoNavigator(this);
	this->AddUCNNavigator(navigator);		
}

//_____________________________________________________________________________
TUCNGeoManager::~TUCNGeoManager()
{
// Destructor
	Info("TUCNGeoManager", "Destructor");	
	if (fGravField) delete TUCNGravField::Instance();
	if (fParticleCloud) delete fParticleCloud;
	delete TUCNParabola::Instance();
}


//_____________________________________________________________________________
TGeoVolume* TUCNGeoManager::MakeBox(const char *name, TGeoMedium *medium, Double_t dx, Double_t dy, Double_t dz)
{
// Make in one step a volume pointing to a box shape with given medium.
	return TUCNGeoBuilder::UCNInstance(this)->MakeUCNBox(name, medium, dx, dy, dz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeTube(const char *name, TGeoMedium *medium,
	Double_t rmin, Double_t rmax, Double_t dz)
{
// Make in one step a volume pointing to a tube shape with given medium.
	return TUCNGeoBuilder::UCNInstance(this)->MakeUCNTube(name, medium, rmin, rmax, dz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeArb8(const char *name, TGeoMedium *medium,
	Double_t dz, Double_t *vertices)
{
// Make an TGeoArb8 volume.
	Error("MakeArb8","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeArb8(name, medium, dz, vertices);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakePara(const char *name, TGeoMedium *medium,
	Double_t dx, Double_t dy, Double_t dz,
	Double_t alpha, Double_t theta, Double_t phi)
{
// Make in one step a volume pointing to a paralelipiped shape with given medium.
	Error("MakePara","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakePara(name, medium, dx, dy, dz, alpha, theta, phi);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeSphere(const char *name, TGeoMedium *medium,
	Double_t rmin, Double_t rmax, Double_t themin, Double_t themax,
	Double_t phimin, Double_t phimax)
{
// Make in one step a volume pointing to a sphere shape with given medium
	Error("MakeSphere","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeSphere(name, medium, rmin, rmax, themin, themax, phimin, phimax);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeTorus(const char *name, TGeoMedium *medium, Double_t r,
	Double_t rmin, Double_t rmax, Double_t phi1, Double_t dphi)
{
// Make in one step a volume pointing to a torus shape with given medium.
	Error("MakeTorus","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeTorus(name, medium, r, rmin, rmax, phi1, dphi);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeTubs(const char *name, TGeoMedium *medium,
	Double_t rmin, Double_t rmax, Double_t dz,
	Double_t phi1, Double_t phi2)
{
// Make in one step a volume pointing to a tube segment shape with given medium.
	Error("MakeTubs","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeTubs(name, medium, rmin, rmax, dz, phi1, phi2);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeEltu(const char *name, TGeoMedium *medium,
	Double_t a, Double_t b, Double_t dz)
{
// Make in one step a volume pointing to a tube shape with given medium
	Error("MakeEltu","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeEltu(name, medium, a, b, dz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeHype(const char *name, TGeoMedium *medium,
	Double_t rin, Double_t stin, Double_t rout, Double_t stout, Double_t dz)
{
// Make in one step a volume pointing to a tube shape with given medium
	Error("MakeHype","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeHype(name, medium, rin, stin, rout, stout, dz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeParaboloid(const char *name, TGeoMedium *medium,
	Double_t rlo, Double_t rhi, Double_t dz)
{
// Make in one step a volume pointing to a tube shape with given medium
	Error("MakeParaboloid","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeParaboloid(name, medium, rlo, rhi, dz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeCtub(const char *name, TGeoMedium *medium,
	Double_t rmin, Double_t rmax, Double_t dz, Double_t phi1, Double_t phi2,
	Double_t lx, Double_t ly, Double_t lz, Double_t tx, Double_t ty, Double_t tz)
{
// Make in one step a volume pointing to a tube segment shape with given medium
	Error("MakeCtub","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeCtub(name, medium, rmin, rmax, dz, phi1, phi2, lx, ly, lz, tx, ty, tz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeCone(const char *name, TGeoMedium *medium,
	Double_t dz, Double_t rmin1, Double_t rmax1,
	Double_t rmin2, Double_t rmax2)
{
// Make in one step a volume pointing to a cone shape with given medium.
	Error("MakeCone","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeCone(name, medium, dz, rmin1, rmax1, rmin2, rmax2);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeCons(const char *name, TGeoMedium *medium,
	Double_t dz, Double_t rmin1, Double_t rmax1,
	Double_t rmin2, Double_t rmax2,
	Double_t phi1, Double_t phi2)
{
// Make in one step a volume pointing to a cone segment shape with given medium
	Error("MakeCons","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeCons(name, medium, dz, rmin1, rmax1, rmin2, rmax2, phi1, phi2);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakePcon(const char *name, TGeoMedium *medium,
	Double_t phi, Double_t dphi, Int_t nz)
{
// Make in one step a volume pointing to a polycone shape with given medium.
	Error("MakePcon","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakePcon(name, medium, phi, dphi, nz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakePgon(const char *name, TGeoMedium *medium,
	Double_t phi, Double_t dphi, Int_t nedges, Int_t nz)
{
// Make in one step a volume pointing to a polygone shape with given medium.
	Error("MakePgon","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakePgon(name, medium, phi, dphi, nedges, nz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeTrd1(const char *name, TGeoMedium *medium,
	Double_t dx1, Double_t dx2, Double_t dy, Double_t dz)
{
// Make in one step a volume pointing to a TGeoTrd1 shape with given medium.
	Error("MakeTrd1","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeTrd1(name, medium, dx1, dx2, dy, dz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeTrd2(const char *name, TGeoMedium *medium,
	Double_t dx1, Double_t dx2, Double_t dy1, Double_t dy2,
	Double_t dz)
{
// Make in one step a volume pointing to a TGeoTrd2 shape with given medium.
	Error("MakeTrd2","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeTrd2(name, medium, dx1, dx2, dy1, dy2, dz);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeTrap(const char *name, TGeoMedium *medium,
	Double_t dz, Double_t theta, Double_t phi, Double_t h1,
	Double_t bl1, Double_t tl1, Double_t alpha1, Double_t h2, Double_t bl2,
	Double_t tl2, Double_t alpha2)
{
// Make in one step a volume pointing to a trapezoid shape with given medium.
	Error("MakeTrap","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeTrap(name, medium, dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeGtra(const char *name, TGeoMedium *medium,
	Double_t dz, Double_t theta, Double_t phi, Double_t twist, Double_t h1,
	Double_t bl1, Double_t tl1, Double_t alpha1, Double_t h2, Double_t bl2,
	Double_t tl2, Double_t alpha2)
{
// Make in one step a volume pointing to a twisted trapezoid shape with given medium.
	Error("MakeGtra","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeGtra(name, medium, dz, theta, phi, twist, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);
}

//_____________________________________________________________________________
TGeoVolume *TUCNGeoManager::MakeXtru(const char *name, TGeoMedium *medium, Int_t nz)
{
// Make a TGeoXtru-shaped volume with nz planes
	Error("MakeXtru","This volume is not yet supported. Cannot build a UCN version. Do Not Use.");
	return TGeoBuilder::Instance(this)->MakeXtru(name, medium, nz);
}


//_____________________________________________________________________________
TGeoNode* TUCNGeoManager::FindNextBoundaryAndStepAlongParabola(TVirtualGeoTrack* track, TUCNGravField* field, Double_t stepTime, Bool_t compsafe)
{
// Compute distance to next boundary within STEPTIME. If no boundary is found,
// propagate current point along current direction with fStepTime=STEPTIME. Otherwise
// propagate with fStepTime=TNEXT (time to reach boundary) and locate/return the next
// node.	
	return this->GetUCNNavigator()->FindNextBoundaryAndStepAlongParabola(track, field, stepTime, compsafe);
} 

//_____________________________________________________________________________
Int_t TUCNGeoManager::AddUCNNavigator(TUCNGeoNavigator *navigator)
{
// Add a navigator in the list of navigators in TGeoManager. Make it the current navigator in both classes
// and store the index so that we have some way to retrieve it from the TObjArray in TGeoManager. 

// This method will only allow a UCNGeoNavigator to be added to the GeoManager ONCE! This is because of perhaps unfounded fears that once we add
// a navigator to the list of navigators in TGeoManager, if we lose our pointer to that UCNGeoNavigator, then when we retrieve this object from the array
// we no longer have a way to know that that navigator in the list is of type TUCNGeoNavigator or of type TGeoNavigator, unless we store the index of
// every UCNGeoNavigator that we add to the list. My current experience of this class is that we only need one anyway, and rather than make it a singleton
// I will currently just throw a warning if we try to add another UCNNavigator. 
	if (fUCNNavigator == NULL) { 
		fNavigatorIndex = this->AddNavigator(navigator);
		fUCNNavigator = navigator;
		this->SetCurrentNavigator(fNavigatorIndex);
	} else {
		Warning("AddUCNNavigator", "UCNNavigator already exists. Another cannot be added to GeoManager");
	}
	return fNavigatorIndex;
}

//_____________________________________________________________________________
TUCNGravField*	TUCNGeoManager::AddGravField(Double_t nx, Double_t ny, Double_t nz)
{
// Create an instance of the Gravitational Field and set its direction vector
	if (fGravField == NULL) {
		fGravField = TUCNGravField::Instance();
	} else {
		Warning("AddGravField", "Field has already been created. Returning original field.");
		return fGravField;
	}
	fGravField->Direction(nx, ny, nz);
	return fGravField;
}

//_____________________________________________________________________________
TUCNParticleCloud* TUCNGeoManager::AddParticleCloud()
{
// Create an instance of the Gravitational Field and set its direction vector
	if (fParticleCloud == NULL) {
		fParticleCloud = new TUCNParticleCloud(this);
	}
	return fParticleCloud;
}

//_____________________________________________________________________________
void TUCNGeoManager::MakeTracks(TUCNParticleCloud* cloud)
{
// Take a ParticleCloud and make a track for each particle and add it to the list of tracks
	Int_t particles = cloud->GetListOfParticles()->GetEntriesFast();
	for (Int_t id = 0; id < particles; id++) {
		// Make track and add to list
		TUCNParticle* particle = cloud->GetParticle(id);
		TVirtualGeoTrack* track = this->MakeTrack(id, particle->GetPdgCode(), particle);
		this->AddTrack(track);
		this->SetCurrentTrack(track);
		track->AddPoint(particle->Vx(), particle->Vy(), particle->Vz(), particle->T());	
	}
}

//_____________________________________________________________________________
void TUCNGeoManager::PropagateTracks(Double_t runTime, Double_t maxStepTime, Bool_t storeData, Bool_t storeTrack)
{
// Take list of tracks and propagate for time runTime
	// -- Write out initial distribution data
	if (storeData == kTRUE) {
		ofstream initialout("initialdata.txt");
		ofstream finalout("finaldata.txt");
		// --------------------------------------------------------------------------------------
		initialout << "Number" << "\t" << "Neutron Time" << "\t" << "Neutron Energy" << "\t";
		initialout << "X Position" << "\t" << "Y Position" << "\t" << "Z Position" << "\t" << "PX" << "\t" << "PY" << "\t" << "PZ" << "\t";
		initialout << "Detected" << "\t" << "Decayed" << "\t" << "Lost" << "\t" << endl;
		// --------------------------------------------------------------------------------------
		finalout << "Number" << "\t" << "Neutron Time" << "\t" << "Neutron Energy" << "\t";
		finalout << "X Position" << "\t" << "Y Position" << "\t" << "Z Position" << "\t" << "PX" << "\t" << "PY" << "\t" << "PZ" << "\t";
		finalout << "Detected" << "\t" << "Decayed" << "\t" << "Lost" << "\t" << endl;

		Int_t tracks = this->GetNtracks();

		for (Int_t id = 0; id < tracks; id++) {
			// Get each Track
			TVirtualGeoTrack* track = this->GetTrack(id);
			TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
			// Write out the initial particle data
			initialout << id << "\t" << particle->T() /Units::s << "\t" << particle->Energy() /Units::neV << "\t";
			initialout << particle->Vx() /Units::m << "\t" << particle->Vy() /Units::m << "\t" << particle->Vz() /Units::m << "\t";
			initialout << particle->Px() /Units::eV << "\t" << particle->Py() /Units::eV << "\t" << particle->Pz() /Units::eV << "\t";
			initialout << particle->Detected() << "\t" << particle->Decayed() << "\t" << particle->Lost() << "\t" << endl;
			// Initialise track - Sets navigator's current point/direction/node to that of the particle
			this->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
				particle->Px()/particle->P(),	particle->Py()/particle->P(), particle->Pz()/particle->P());
			// Propagate track
			this->GetUCNNavigator()->PropagateTrack(track, runTime, maxStepTime);
			// Write out the final position
			finalout << id << "\t" << particle->T() /Units::s << "\t" << particle->Energy() /Units::neV << "\t";
			finalout << particle->Vx() /Units::m << "\t" << particle->Vy() /Units::m << "\t" << particle->Vz() /Units::m << "\t";
			finalout << particle->Px() /Units::eV << "\t" << particle->Py() /Units::eV << "\t" << particle->Pz() /Units::eV << "\t";
			finalout << particle->Detected() << "\t" << particle->Decayed() << "\t" << particle->Lost() << "\t" << endl;

			if (storeTrack == kFALSE) {
				// Delete the track to save memory
				track->ResetTrack();
			}
		}
		initialout.close();
		finalout.close();
	} else {
		Int_t tracks = this->GetNtracks();
		for (Int_t id = 0; id < tracks; id++) {
			// Get each Track
			TVirtualGeoTrack* track = this->GetTrack(id);
			TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
			// Initialise track - Sets navigator's current point/direction/node to that of the particle
			this->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
				particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
			// Propagate track
			this->GetUCNNavigator()->PropagateTrack(track, runTime, maxStepTime);
			if (storeTrack == kFALSE) {
				// Delete the track to save memory
				track->ResetTrack();
			}
		}
	}
}

//_____________________________________________________________________________
void TUCNGeoManager::PropagateTracks(Int_t steps, Double_t maxStepTime, Bool_t storeTrack)
{
// Take list of tracks and propagate for certain number of steps
	Int_t tracks = this->GetNtracks();
	for (Int_t id = 0; id < tracks; id++) {
		#ifdef VERBOSE_MODE
			Info("PropagateTracks","Track: %i", id);
		#endif
		// Get each Track
		TVirtualGeoTrack* track = this->GetTrack(id);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
		// Initialise track - Sets navigator's current point/direction/node to that of the particle
		this->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), \
			particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
		// Propagate track
		this->GetUCNNavigator()->PropagateTrack(track, steps, maxStepTime);
		if (storeTrack == kFALSE) {
			// Delete the track to save memory
			#ifdef VERBOSE_MODE
				Info("PropagateTracks","Delete Track: %i", id);
			#endif
			track->ResetTrack();
		}
	}
}