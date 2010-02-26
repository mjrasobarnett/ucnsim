// UCN Geometry class
// Author: Matthew Raso-Barnett  19/01/2009

#include <iostream>
#include <fstream>
#include <cassert>
#include <stdexcept>

#include <TVector3.h>
#include <TRandom.h>
#include <TVirtualGeoTrack.h>

#include <TUCNGeoTrack.h>
#include <PhysicalConstants.h>

#define STORE_TRACKS
//#define VERBOSE_MODE
//#define STORE_TRACK_DATA
//#define STORE_ANGLE_DATA
	


using namespace std;

ClassImp(TUCNGeoTrack) 

//______________________________________________________________________________
TUCNGeoTrack::TUCNGeoTrack(void)
{
//	-- Default constructor
	Info("TUCNGeoTrack", "Default Constructor");
	fTrackID = 0;
	fDetected = kFALSE;
	fTolerance = 1.e-10;
	fThrownAway = kFALSE;
	fStepTime = 1.;
	
	fDiffuseProb = 0.;
	fBounceCounter = 0;
	fSpecBounceCounter = 0;
	fDiffBounceCounter = 0;
}

//______________________________________________________________________________
TUCNGeoTrack::TUCNGeoTrack(Int_t trackid, Double_t steptime)
{
// -- Constructor
	Info("TUCNGeoTrack", "Constructor");
	fTrackID = trackid;
	fDetected = kFALSE;
	fTolerance = 1.e-10;
	fThrownAway = kFALSE;
	fStepTime = steptime;
	
	fDiffuseProb = 0.;
	fBounceCounter = 0;
	fSpecBounceCounter = 0;
	fDiffBounceCounter = 0;
}

//______________________________________________________________________________
TUCNGeoTrack::~TUCNGeoTrack()
{ 
// -- Default Destructor
	Info("TUCNGeoTrack", "Destructor");
}


//   *-*-*-*-*-*-*-*-*-*-*-*-*-*-METHODS-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//								  			=======

//______________________________________________________________________________
void TUCNGeoTrack::InitialiseTrack(TParticle *neutron)
{
// -- Method creates a track that is initialise with the position and direction of the TUCNParticle
	TVirtualGeoTrack* track = gGeoManager->MakeTrack(fTrackID, neutron->GetPdgCode(), neutron);
	gGeoManager->AddTrack(track);
	gGeoManager->SetCurrentTrack(track);
	gGeoManager->InitTrack(neutron->Vx(), neutron->Vy(), neutron->Vz(), neutron->Px()/neutron->P(), neutron->Py()/neutron->P(), neutron->Pz()/neutron->P());
//	track->AddPoint(neutron->Vx(), neutron->Vy(), neutron->Vz(), neutron->T());
#ifdef VERBOSE_MODE		
	Info("InitialiseTrack", "Track initialised at - X: %f, Y: %f, Z: %f, T: %f.", neutron->Vx(), neutron->Vy(), neutron->Vz(), neutron->T());
#endif
}

//______________________________________________________________________________
void TUCNGeoTrack::Bounce(TParticle* neutron)
{			
#ifdef VERBOSE_MODE		
	cout << "------------------ BOUNCE ------------------- " << endl;
	cout << "Medium: " << gGeoManager->GetCurrentNode()->GetMedium()->GetName() <<  endl;	
#endif

	// -- Direction Vector
	const Double_t *currentDir = gGeoManager->GetCurrentDirection();
	TVector3 InitialDir(currentDir[0], currentDir[1], currentDir[2]);
	TVector3 FinalDir(0.,0.,0.);
	
#ifdef VERBOSE_MODE		
	cout << "Initial Dir - " << "dx: " << InitialDir.X() << "\t" << "dy: " << InitialDir.Y() << "\t" << "dz: " << InitialDir.Z() << endl;
#endif
	
	// -- Normal Vector
	const Double_t *currentNorm = gGeoManager->FindNormal();
	const TVector3 Norm(-currentNorm[0], -currentNorm[1], -currentNorm[2]);
	
#ifdef VERBOSE_MODE		
	cout << "Norm - " << "dx: " << Norm.X() << "\t" << "dy: " << Norm.Y() << "\t" << "dz: " << Norm.Z() << endl;
#endif
	
	// Determine Reflection Type 
	Double_t prob = gRandom->Uniform(0.0,1.0);
	if (fDiffuseProb >= prob) {
		// -- Diffuse Bounce
		this->DiffuseBounce(FinalDir, Norm);
	} else {
		// -- Specular Bounce
		this->SpecularBounce(InitialDir, FinalDir, Norm);
	}
		
#ifdef VERBOSE_MODE		
	cout << "Final Dir - " << "dx: " << FinalDir.X() << "\t" << "dy: " << FinalDir.Y() << "\t" << "dz: " << FinalDir.Z() << endl;
	cout << "----------------------------------------------- " << endl;
#endif
	
	// -- Set New Direction in GeoManager
	gGeoManager->SetCurrentDirection(FinalDir.X(), FinalDir.Y(), FinalDir.Z());

	// -- Update Particle Momentum
	neutron->SetMomentum(currentDir[0]*(neutron->P()), currentDir[1]*(neutron->P()), currentDir[2]*(neutron->P()), neutron->Energy());	
	
	// -- Update Bounce Counter
	fBounceCounter++;
	
	// Checks
	if (TMath::Abs(FinalDir.X()-currentDir[0]) > 1.e-10 || TMath::Abs(FinalDir.Y()-currentDir[1]) > 1.e-10 || TMath::Abs(FinalDir.Z()-currentDir[2]) > 1.e-10) {
		throw runtime_error("Bounce: Dir mismatch");
	}
	if (TMath::Abs(FinalDir.Mag() - 1.0) > 1.e-10) {
		cout << FinalDir.Mag() << endl;
		throw runtime_error("Bounce: Final Dir not unit vector");
	}
	if (TMath::Abs(InitialDir.Mag() - 1.0) > 1.e-10) {
		throw runtime_error("Bounce: Initial Dir not unit vector");
	}
	if (TMath::Abs(Norm.Mag() - 1.0) > 1.e-10) {
		throw runtime_error("Bounce: Norm not unit vector");
	}
	if (fBounceCounter != (fDiffBounceCounter + fSpecBounceCounter)) {
		throw runtime_error("Bounce: Bounce counters are not matching");
	}
}

//______________________________________________________________________________
void TUCNGeoTrack::SpecularBounce(TVector3& InitialDir, TVector3& FinalDir, const TVector3& Norm)
{
	// Reflection Law for Specular Reflection
	FinalDir = InitialDir - 2.0 * (( InitialDir.Dot(Norm) ) * Norm);  
	fSpecBounceCounter++;
}

//______________________________________________________________________________
void TUCNGeoTrack::DiffuseBounce(TVector3& finalDir, const TVector3& norm)
{
	// First we need to pick random angles to choose the orientation of our diffuse direction vector. 
	// One could pick angles so that every direction on the unit sphere has the same probability
	// Correct method for UCN physics though is to weight these angles towards the poles by adding an extra cos(theta) - derivation of how to pick these angles is in notes
//	Double_t phi = TMath::Pi();
//	Double_t theta = TMath::Pi()/4.0;
	Double_t phi = gRandom->Uniform(0.0, 1.0)*2*TMath::Pi();
	Double_t u = gRandom->Uniform(0.0, 0.5);		
	Double_t theta = TMath::ACos(TMath::Sqrt(1.0 - 2*u)); // We ignore the negative sqrt term, since we are only interested in theta between 0 and pi/2, and the negative term gives pi/2 to pi. 
	
	// Calculate local normal vector	
	Double_t normal[3] = {norm.X(), norm.Y(), norm.Z()};
	Double_t localnormal[3];
	gGeoManager->MasterToLocalVect(normal, localnormal);
	TVector3 localNorm(localnormal[0], localnormal[1], localnormal[2]);
	assert(TMath::Abs(localNorm.Mag() - 1.0) < 1.e-10);
	
	// Define a preferred direction in our coordinate system - usually the z-direction - that we want to be perpendicular to the normal vector of our surface
	TVector3 upAxis(0.,0.,1.);
	
	// Calculate the cross product of the 'up' vector with our normal vector to get a vector guaranteed to be perpendicular to the normal. This is the vector we will want to rotate around initially. 
	TVector3 perpAxis = localNorm.Cross(upAxis);
	//assert(TMath::Abs(perpAxis.Mag() - 1.0) < 1.e-10);
	
	// Here we check to make sure that the upAxis we chose is not parallel to the normal vector. If it is, we try another one, x. 
	if (TMath::Abs(perpAxis.Dot(perpAxis) - 1.0) > 1.e-10) {
		upAxis.SetXYZ(1.,0.,0.);
		perpAxis = localNorm.Cross(upAxis);
		assert(TMath::Abs(perpAxis.Mag() - 1.0) < 1.e-10);
		
		if (TMath::Abs(perpAxis.Dot(perpAxis) - 1.0) > 1.e-10) {
			throw runtime_error("Error: in Diffuse bounce routine. Could not find an axis perpendicular to normal. Normal is apparanetly parallel to z and x axes!!!");
		}
	} 
	// Rotate normal vector about perpendicular axis by angle theta. Using Rodrigues' formula derived in notes. 
	TVector3 rotatedNorm;
	{
		TVector3 term3 = perpAxis.Cross(localNorm);
		TVector3 term2 = localNorm - (localNorm.Dot(perpAxis))*perpAxis;
		rotatedNorm = localNorm + (TMath::Cos(theta) - 1.0)*term2 + TMath::Sin(theta)*term3;
	}
	assert(TMath::Abs(rotatedNorm.Mag() - 1.0) < 1.e-10);
	
	// Rotate the newly rotated Normal vector, rotatedNorm, by angle phi, this time rotating about the original normal vector, norm.
	TVector3 direction;
	{
		TVector3 term3 = localNorm.Cross(rotatedNorm);
		TVector3 term2 = rotatedNorm - (rotatedNorm.Dot(localNorm))*localNorm;
		direction = rotatedNorm + (TMath::Cos(phi) - 1.0)*term2 + TMath::Sin(phi)*term3;
	}
	assert(TMath::Abs(direction.Mag() - 1.0) < 1.e-10);
	
	// Convert the direction vector back to the global frame
	Double_t dir[3] = {direction.X(), direction.Y(), direction.Z()};
	Double_t globalDir[3];
	gGeoManager->LocalToMasterVect(dir, globalDir);
	finalDir.SetXYZ(globalDir[0], globalDir[1], globalDir[2]);
	assert(TMath::Abs(finalDir.Mag() - 1.0) < 1.e-10);
	assert(TMath::Abs(finalDir.Dot(norm)) > 1.e-10); // Assert that our final direction is not perpendicular to the normal. This could result in escaping the boundary

	#ifdef VERBOSE_MODE	
		cout << "----------------------------" << endl;
		cout << "Diffuse Bounce" << endl;
		cout << "Current Volume: " << gGeoManager->GetCurrentVolume()->GetName() << endl;
		cout << "Normal Vector: " << norm.X() << "\t" << norm.Y() << "\t" << norm.Z() << endl;
		cout << "Local Normal: " << localNorm.X() << "\t" << localNorm.Y() << "\t" << localNorm.Z() << endl;
		cout << "'Up' Axis: " << upAxis.X() << "\t" << upAxis.Y() << "\t" << upAxis.Z() << endl;
		cout << "Perpendicular vector to normal: " << perpAxis.X() << "\t" << perpAxis.Y() << "\t" << perpAxis.Z() << endl;
		cout << "Normal vector after rotating by theta: " << rotatedNorm.X() << "\t" << rotatedNorm.Y() << "\t" << rotatedNorm.Z() << endl;
		cout << "Final direction vector after rotating by phi: " << direction.X() << "\t" << direction.Y() << "\t" << direction.Z() << endl;	
		cout << "Global Final Direction: " << finalDir.X() << "\t" << finalDir.Y() << "\t" << finalDir.Z() << endl;
	#endif
		
	fDiffBounceCounter++;
}

//______________________________________________________________________________
Bool_t TUCNGeoTrack::Step(Double_t steptime, TUCNParticle* neutron) 
{	
	const Double_t* currentpos = gGeoManager->GetCurrentPoint();	
	const Double_t* currentdir = gGeoManager->GetCurrentDirection();
	
	// -- mass_GeV_c is the neutron mass in units of GeV divided by c. This ensures we get the right dimension in the velocity below. 			
	Double_t mass_GeV_c = (neutron->GetMass())*GeV/c_light;
	Double_t mass_GeV_c2 = 	mass_GeV_c/c_light;		
	Double_t initialVelocity = neutron->Velocity();
		
	TVector3 InitialPosition(currentpos[0], currentpos[1], currentpos[2]);
	TVector3 InitialDirection(currentdir[0], currentdir[1], currentdir[2]);
	TVector3 InitialVelocity(InitialDirection.X()*initialVelocity, InitialDirection.Y()*initialVelocity, InitialDirection.Z()*initialVelocity);
	TVector3 InitialMomentum(InitialVelocity.X()*mass_GeV_c, InitialDirection.Y()*mass_GeV_c, InitialDirection.Z()*mass_GeV_c);
	
	TVector3 GlobalFieldVector = *(gGeoManager->GetFieldVector());
	
	Double_t initialKE = neutron->GetTotalEnergy() + mass_GeV_c2*grav_acceleration*InitialPosition.Dot(GlobalFieldVector);
	
	#ifdef VERBOSE_MODE	
		cout << "------------------- START OF STEP ----------------------" << endl;
		cout << "Steptime (s): " 		 << steptime << endl;
		cout << "Initial Point (m): " << InitialPosition.X()  << "\t" << InitialPosition.Y()  << "\t" << InitialPosition.Z()  << endl;
		cout << "Initial Dir: "       << InitialDirection.X() << "\t" << InitialDirection.Y() << "\t" << InitialDirection.Z() << endl;
		cout << "Initial Velocity (m/s): " << InitialVelocity.X()  << "\t" << InitialVelocity.Y()  << "\t" << InitialVelocity.Z()  << endl;
		cout << "Initial Momentum (eV): "   << InitialMomentum.X() << "\t" << InitialMomentum.Y() << "\t" << InitialMomentum.Z() << endl;
		cout << "Initial Volume: "    << gGeoManager->GetCurrentVolume()->GetName() << endl;
		cout << "Initial Velocity Mag. (m/s): " << initialVelocity << "m/s" << endl;
		cout << "Energy (neV): "    << neutron->Energy() /neV << endl;
		cout << "-----------------------------" << endl;
		cout << "Is On Boundary?  " << gGeoManager->IsOnBoundary() << endl;
		cout << "Is Outside?  "     << gGeoManager->IsOutside() << endl;
		cout << "Is Entering?  " 	 << gGeoManager->IsEntering()  << endl;
		cout << "Is Exiting?  "   	 << gGeoManager->IsExiting() << endl;
		cout << "-----------------------------" << endl << endl;
	#endif

	// -----------  Checks ----------------
	// -- Check Energy
	if (TMath::Abs(initialKE - neutron->Energy()) > 1.e-12) {
		cout << "Current KE calculated from Height: " << initialKE /neV << " neV" << "\t" << "Neutron KE: " << neutron->Energy() /neV << " neV" << endl;
		throw runtime_error("Initial Energy Mismatch");
	}
	
	// -- Check Velocity
	if (TMath::Abs(initialVelocity - InitialVelocity.Mag()) > 1.e-12) {
		cout << "Current V calculated from Height: " << initialVelocity << " m/s" << "\t" << "Neutron V: " << InitialVelocity.Mag() << " m/s" << endl;
		throw runtime_error("Initial Velocity Mismatch");
	}
	
	// --------------------------------------------------------------------------
	// -- STEP --
	// --------------------------------------------------------------------------
	#ifdef VERBOSE_MODE	
		cout << "Step!" << endl;
	#endif
	gGeoManager->FindNextBoundaryAndStepAlongParabola(steptime, kFALSE);
	
	currentpos = gGeoManager->GetCurrentPoint();	
	currentdir = gGeoManager->GetCurrentDirection();
	
	Double_t timetravelled = gGeoManager->GetCurrentNavigator()->GetTimeStep();
	Double_t distancetravelled = gGeoManager->GetCurrentNavigator()->GetStep();
	
	// -- Store Final Position & Velocity
	TVector3 FinalPosition(currentpos[0], currentpos[1], currentpos[2]);
	TVector3 FinalDirection(currentdir[0], currentdir[1], currentdir[2]);
	
	Double_t heightclimbed = (FinalPosition - InitialPosition).Dot(GlobalFieldVector);
	Double_t finalKE = initialKE + mass_GeV_c2*grav_acceleration*heightclimbed;
	Double_t finalVelocity = TMath::Sqrt((2.0*finalKE)/mass_GeV_c2);
	
			
	TVector3 FinalVelocity(FinalDirection.X()*finalVelocity, FinalDirection.Y()*finalVelocity, FinalDirection.Z()*finalVelocity);
	TVector3 FinalMomentum(FinalVelocity.X()*mass_GeV_c, FinalDirection.Y()*mass_GeV_c, FinalDirection.Z()*mass_GeV_c);
	
	neutron->SetProductionVertex(FinalPosition.X(), FinalPosition.Y(), FinalPosition.Z(), (neutron->T() + timetravelled));
	neutron->SetMomentum(FinalVelocity.X()*mass_GeV_c, FinalVelocity.Y()*mass_GeV_c, FinalVelocity.Z()*mass_GeV_c, finalKE);
	neutron->IncreasePathLength(distancetravelled);
	
	
	#ifdef VERBOSE_MODE	
		cout << "-----------------------------" << endl;
		cout << "Time Travelled in step (s): " << timetravelled << endl;
		cout << "Distance Travelled in step (m): " << distancetravelled << endl;
		cout << "Total Time Travelled (s): " << neutron->T() << endl;
		cout << "Total Distance Travelled (m): " << neutron->GetPathLength() << endl;
		cout << "-----------------------------" << endl;
		cout << "Final Point (m): " << FinalPosition.X() << "\t"  << FinalPosition.Y() << "\t"  << FinalPosition.Z()  << endl;
		cout << "Final Dir: "   << FinalDirection.X() << "\t" << FinalDirection.Y() << "\t" << FinalDirection.Z() << endl;
		cout << "Final Velocity (m/s): " << FinalVelocity.X()  << "\t" << FinalVelocity.Y()  << "\t" << FinalVelocity.Z()  << endl;
		cout << "Final Momentum (eV): "   << FinalMomentum.X() << "\t" << FinalMomentum.Y() << "\t" << FinalMomentum.Z() << endl;
		cout << "Final Total Velocity (m/s): " << neutron->Velocity() << endl;
		cout << "Energy (neV): " << neutron->Energy() /neV << endl;
		cout << "Final Velocity (check): " << finalVelocity << endl;
		cout << "FinalKE (check): " << finalKE << endl;
		cout << "Height Climbed: " << heightclimbed << endl;
		cout << "Final Volume: " << gGeoManager->GetCurrentVolume()->GetName() <<  endl;	
		cout << "-----------------------------" << endl;
		cout << "Is On Boundary?  "  << gGeoManager->IsOnBoundary() << endl;
		cout << "Is Outside?  "  << gGeoManager->IsOutside() << endl;
		cout << "Is Entering?  " << gGeoManager->IsEntering()  << endl;
		cout << "Is Exiting?  "  << gGeoManager->IsExiting() << endl;
		cout << "-----------------------------" << endl;
	#endif
	
	// --------------------------------------------------------------------------}
	
#ifdef STORE_TRACKS
	gGeoManager->GetCurrentTrack()->AddPoint(neutron->Vx(), neutron->Vy(), neutron->Vz(), neutron->T()); 
#endif
	
	return kTRUE;
}

//______________________________________________________________________________
Bool_t TUCNGeoTrack::PropagateForSetTime(Double_t seconds)
{
	Int_t stepnumber;
	fThrownAway = kFALSE;
	fDetected = kFALSE;
	Double_t steptime;
	
	
	TUCNParticle* neutron = (TUCNParticle*)gGeoManager->GetCurrentTrack()->GetParticle();

#ifdef VERBOSE_MODE				
	Info("PropagateForSetTime","Starting Run - Max time (seconds): %f", seconds);
#endif
		
	if ( gGeoManager->GetCurrentNode()->GetMedium() != gGeoManager->GetMedium("Vacuum") ) {
		neutron->Print();
		Error("PropagateForSetTime", "Track initialised outside starting volume");
		fThrownAway = kTRUE;
		return kFALSE;
	}
	
#ifdef STORE_TRACK_DATA	
	std::ofstream out1("testdata.txt");
	out1 << "Time T" << "\t" << "Stepnumber" << "\t" << "Position: X" << "\t" << "Position: Y   "  << "\t"  << "Position: Z" << "\t";
	out1 << "Momentum: Px" << "\t"  << "Momentum: Py" << "\t"  << "Momentum: Pz" << endl;
#endif	

#ifdef STORE_ANGLE_DATA	
	std::ofstream out2("bounceangledata.txt");
	out2 << "stepnumber" << "\t" << "Entering Angle" << "\t" << "Exiting Angle" << "\t" << endl;
#endif		

	///////////////////////////////////	
	// -- Propagation Loop
	///////////////////////////////////
	for (stepnumber = 1 ; neutron->T() /s < seconds ; stepnumber++) {
		
		#ifdef VERBOSE_MODE		
			cout << "STEP " << stepnumber << "\t" << neutron->T() << " s" << "\t" << gGeoManager->GetCurrentNode()->GetMedium()->GetName() << endl;	
		#endif
		
		#ifdef STORE_TRACK_DATA	
			out1 << neutron->T() << "\t" << stepnumber << "\t" << neutron->Vx() << "\t" << neutron->Vy() << "\t" << neutron->Vz() << "\t";
			out1 << neutron->Px() << "\t" << neutron->Py() << "\t" << neutron->Pz() << "\t" << endl;
		#endif
		
		// -- Define local steptime
		if (neutron->T() > (seconds - fStepTime)) {
			steptime = (seconds - neutron->T());
		} else {
			steptime = fStepTime;
		}
		
		// -- Step
		if (Step(steptime, neutron) == kFALSE) {
			Error("PropagateForSetTime", "Error returned by Step(): Stopping track");
			fThrownAway = kTRUE;
			fDetected = kFALSE;
			break;
		}
		
		if (gGeoManager->GetCurrentNode()->GetMedium() == gGeoManager->GetMedium("BlackHole")) {
			// -- Check that we aren't outside the Volume
			fThrownAway = kTRUE;
			fDetected = kFALSE;
			Error("PropagateForSetTime", "Warning: Particle Escaped into Blackhole!");
			return kFALSE;
		} else if (gGeoManager->GetCurrentNode()->GetMedium() == gGeoManager->GetMedium("Detector")) {
			// -- Check if we hit a detector 
			fThrownAway = kFALSE;
			fDetected = kTRUE;
			break;
		} else if (gGeoManager->GetCurrentNode()->GetMedium() != gGeoManager->GetMedium("Vacuum")) {
			// -- Check if we are on a boundary - if so bounce then step
			#ifdef STORE_ANGLE_DATA	
				const Double_t* currentdir = gGeoManager->GetCurrentDirection();
				TVector3 dir1(currentdir[0],currentdir[1],currentdir[2]);
				Double_t *currentNorm = gGeoManager->FindNormal();
				TVector3 norm1(currentNorm[0], currentNorm[1], currentNorm[2]);
				out2 << stepnumber << "\t" << TMath::ACos(dir1.Dot(norm1)) << "\t";
			#endif
			Bounce(neutron);
			#ifdef STORE_ANGLE_DATA	
				const Double_t* currentdir2 = gGeoManager->GetCurrentDirection();
				TVector3 dir2(currentdir2[0],currentdir2[1],currentdir2[2]);
				Double_t *currentNorm2 = gGeoManager->FindNormal();
				TVector3 norm2(currentNorm2[0], currentNorm2[1], currentNorm2[2]);
				out2 << TMath::ACos(dir2.Dot(norm2)) << "\t" << endl;
			#endif
			gGeoManager->CrossBoundaryAndLocate(kFALSE, gGeoManager->GetCurrentNode());
		} else {
			//
		}
	}

	cout << "FINAL STATUS: " << "Track: " << gGeoManager->GetCurrentTrack()->GetId() << "\t" << "Steps taken: " << stepnumber << "\t";
	cout << "Time: " << neutron->T() << "s" << "\t" << "Final Medium: " << gGeoManager->GetCurrentNode()->GetMedium()->GetName() << "\t";
	cout << "Bounces: " << fBounceCounter << "\t" << "Specular: " << fSpecBounceCounter << "\t" << "Diffuse: " << fDiffBounceCounter << endl;
	
	if (gGeoManager->GetCurrentNode()->GetMedium() == gGeoManager->GetMedium("Vacuum")) fDetected = kTRUE;
	return kTRUE;
}

//______________________________________________________________________________
Bool_t TUCNGeoTrack::Propagate(Int_t maxsteps)
{
	Int_t stepnumber;
	fThrownAway = kFALSE;
	fDetected = kFALSE;

	TUCNParticle* neutron = (TUCNParticle*)gGeoManager->GetCurrentTrack()->GetParticle();

#ifdef VERBOSE_MODE				
	Info("Propagate","Starting Run - Max Steps: %i", maxsteps);
#endif

	if ( gGeoManager->GetCurrentNode()->GetMedium() != gGeoManager->GetMedium("Vacuum") ) {
		Error("PropagateForSetTime", "Track initialised outside starting volume");
		fThrownAway = kTRUE;
		return kFALSE;
	}

	#ifdef STORE_TRACK_DATA	
		std::ofstream out1("testdata.txt");
		out1 << "Time T" << "\t" << "Stepnumber" << "\t" << "Position: X" << "\t" << "Position: Y   "  << "\t"  << "Position: Z" << "\t";
		out1 << "Momentum: Px" << "\t"  << "Momentum: Py" << "\t"  << "Momentum: Pz" << endl;
	#endif
	
	///////////////////////////////////	
	// -- Propagation Loop
	///////////////////////////////////
	for (stepnumber = 1; stepnumber <= maxsteps; stepnumber++) {

		#ifdef VERBOSE_MODE		
			cout << "STEP " << stepnumber << "\t" << ((TUCNParticle*)gGeoManager->GetCurrentTrack()->GetParticle())->T();
			cout << " s" << "\t" << gGeoManager->GetCurrentNode()->GetMedium()->GetName() << endl;	
		#endif
		
		#ifdef STORE_TRACK_DATA	
				out1 << neutron->T() << "\t" << stepnumber << "\t" << neutron->Vx() << "\t" << neutron->Vy() << "\t" << neutron->Vz() << "\t";
				out1 << neutron->Px() << "\t" << neutron->Py() << "\t" << neutron->Pz() << "\t" << endl;
		#endif 
		
		// -- Step
		if (Step(fStepTime, neutron) == kFALSE) {
			cout << "Error in step: Stopping track" << endl;
			fThrownAway = kTRUE;
			fDetected = kFALSE;
			break;
		}

		if (gGeoManager->GetCurrentNode()->GetMedium() == gGeoManager->GetMedium("BlackHole")) {
			// -- Check that we aren't outside the Volume
			fThrownAway = kTRUE;
			fDetected = kFALSE;
			cout << "Warning: Particle Escaped into Blackhole!" << endl;
			return kFALSE;
		} else if (gGeoManager->GetCurrentNode()->GetMedium() == gGeoManager->GetMedium("Detector")) {
			// -- Check if we hit a detector 
			fThrownAway = kFALSE;
			fDetected = kTRUE;
			break;
		} else if (gGeoManager->GetCurrentNode()->GetMedium() != gGeoManager->GetMedium("Vacuum")) {
			// -- Check if we are on a boundary - if so bounce then step
			Bounce(neutron);
			gGeoManager->CrossBoundaryAndLocate(kFALSE, gGeoManager->GetCurrentNode());
		} else {
			//
		}
	}

	cout << "Final Status: " << "\t" << "Track: " << gGeoManager->GetCurrentTrack()->GetId() << "\t" << "Steps: " << stepnumber << "\t";
	cout << ((TUCNParticle*)gGeoManager->GetCurrentTrack()->GetParticle())->T() << "s" << "\t" << gGeoManager->GetCurrentNode()->GetMedium()->GetName() << endl;
	return kTRUE;
}

