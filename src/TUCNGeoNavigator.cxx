// TUCNGeoNavigator
// Author: Matthew Raso-Barnett  27/05/2009
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoPatternFinder.h"
#include "TGeoVoxelFinder.h"
#include "TVirtualGeoTrack.h"
#include "TParticle.h"
#include "TGeoNavigator.h"
#include "TRandom3.h"
#include "TMath.h"

#include "TUCNGeoBBox.h"
#include "TUCNGeoManager.h"
#include "TUCNGeoMaterial.h"
#include "TUCNGravField.h"
#include "TUCNParticle.h"
#include "TUCNParabola.h"
#include "Units.h"

#include "TUCNGeoNavigator.h"

using std::cout;
using std::endl;
using std::runtime_error;

//#define VERBOSE_MODE

ClassImp(TUCNGeoNavigator)

const Double_t 	TUCNGeoNavigator::fgTolerance;
const Int_t 		TUCNGeoNavigator::fgMaxSteps;

//_____________________________________________________________________________
TUCNGeoNavigator::TUCNGeoNavigator()
              	  :TGeoNavigator(),
				      fUCNGeometry(0),
						fUCNNextNode(0),
						fStepTime(1.),
						fUCNIsStepEntering(kFALSE),
	               fUCNIsStepExiting(kFALSE),
						fUCNIsOutside(kFALSE),
						fUCNIsOnBoundary(kFALSE)
{
// dummy constructor
	Info("TUCNGeoNavigator", "Dummy Constructor");
}

//_____________________________________________________________________________
TUCNGeoNavigator::TUCNGeoNavigator(TUCNGeoManager* geom)
              	  :TGeoNavigator(geom),
						fUCNGeometry(geom),
						fUCNNextNode(0),
						fStepTime(1.),
						fUCNIsStepEntering(kFALSE),
	               fUCNIsStepExiting(kFALSE),
						fUCNIsOutside(kFALSE),
						fUCNIsOnBoundary(kFALSE)
{
// Default constructor.
	Info("TUCNGeoNavigator", "Constructor");
	for (Int_t i=0; i<3; i++) {
      fUCNNormal[i] = 0.;
   }
}      

//_____________________________________________________________________________
TUCNGeoNavigator::TUCNGeoNavigator(const TUCNGeoNavigator& gm)
              	  :TGeoNavigator(gm),
						fUCNGeometry(gm.fUCNGeometry),
						fUCNNextNode(gm.fUCNNextNode),
						fStepTime(gm.fStepTime),
						fUCNIsStepEntering(gm.fUCNIsStepEntering),
	               fUCNIsStepExiting(gm.fUCNIsStepExiting),
						fUCNIsOutside(gm.fUCNIsOutside),
						fUCNIsOnBoundary(gm.fUCNIsOnBoundary)
{
// Copy constructor.
	Info("TUCNGeoNavigator", "Copy Constructor");
	for (Int_t i=0; i<3; i++) {
      fUCNNormal[i] = gm.fUCNNormal[i];
   }
}      

/*//_____________________________________________________________________________
TUCNGeoNavigator& TUCNGeoNavigator::operator=(const TUCNGeoNavigator& gm)
{
   //assignment operator
   if(this!=&gm) {
		TGeoNavigator::=(gm);
		// Placeholder for if we add member variables
		//...
   }
   return *this;   
}
*/

//_____________________________________________________________________________
TUCNGeoNavigator::~TUCNGeoNavigator()
{
// Destructor.
   Info("TUCNGeoNavigator", "Destructor");
}

//_____________________________________________________________________________
Double_t* TUCNGeoNavigator::FindUCNNormal()
{
// Computes fast normal to next crossed boundary, assuming that the current point
// is close enough to the boundary. Works only after calling FindNextBoundaryAndStepAlongParabola.
// This method must be used exclusively instead of TGeoNavigator::FindNormal when gravity is present.

	if (fUCNGeometry->GetGravField() == NULL) return this->FindNormal(); // If there is no gravity use method FindNormal
	if (!fUCNNextNode) return 0;
   Double_t local[3];
   Double_t ldir[3];
   Double_t lnorm[3];
   this->GetHMatrix()->MasterToLocal(this->GetCurrentPoint(), local);
   this->GetHMatrix()->MasterToLocalVect(this->GetCurrentDirection(), ldir);
   fUCNNextNode->GetVolume()->GetShape()->ComputeNormal(local, ldir, lnorm);
   this->GetHMatrix()->LocalToMasterVect(lnorm, fUCNNormal);
	return fUCNNormal;
}

//_____________________________________________________________________________
TGeoNode* TUCNGeoNavigator::FindNextDaughterBoundaryAlongParabola(Double_t* point, Double_t* velocity, Double_t* field, Int_t &idaughter, Bool_t compmatrix)
{
// Computes as fStep the distance to next daughter of the current volume. 
// The point and direction must be converted in the coordinate system of the current volume.
// The proposed step limit is fStep.
	
	// -- First Get the current local and global fields
	Double_t motherField[3] = {field[0], field[1], field[2]}; 
	Double_t motherPoint[3] = {point[0], point[1], point[2]};
	Double_t motherVelocity[3] = {velocity[0], velocity[1], velocity[2]};
	
	#ifdef VERBOSE_MODE		
		Info("FindNextDaughterBoundaryAlongParabola", "Mother Local Field: X: %f, Y: %f, Z: %f", motherField[0], motherField[1], motherField[2]);
	#endif
   
	// -- Initialising some important parameters
	Double_t tnext = TGeoShape::Big();
	Double_t snext = TGeoShape::Big();
   idaughter = -1; // nothing crossed
   TGeoNode *nodefound = 0;
	TGeoHMatrix* globalMatrix = this->GetCurrentMatrix(); // This has been added because we do not have access to fGlobalMatrix in TGeoNavigator
	
   // Get number of daughters. If no daughters we are done.
   TGeoVolume *vol = this->GetCurrentNode()->GetVolume();
   Int_t nd = vol->GetNdaughters();
   
	#ifdef VERBOSE_MODE		
		Info("FindNextDaughterBoundaryAlongParabola","Volume: %s. Checking number of Daughters: %i", vol->GetName(), nd);
	#endif
	
	if (!nd) return 0;  // No daughter 
   if (fUCNGeometry->IsActivityEnabled() && !vol->IsActiveDaughters()) return 0;
   Double_t localPoint[3], localVelocity[3], localfield[3];
   TGeoNode *current = 0;
   Int_t i=0;
   
	// ************************************************************************************
	// -- BRANCH 1
	// -- if current volume is divided, we are in the non-divided region. We
   // -- check first if we are inside a cell in which case compute distance to next cell
   // ************************************************************************************
	
   TGeoPatternFinder *finder = vol->GetFinder();
   if (finder) {
      Int_t ifirst = finder->GetDivIndex();
      Int_t ilast = ifirst+finder->GetNdiv()-1;
      current = finder->FindNode(motherPoint);
      if (current) {
         // Point inside a cell: find distance to next cell
         Int_t index = current->GetIndex();
         if ((index-1) >= ifirst) ifirst = index-1;
         else                     ifirst = -1;
         if ((index+1) <= ilast)  ilast  = index+1;
         else                     ilast  = -1;
      }
      if (ifirst>=0) {   
         current = vol->GetNode(ifirst);
         current->cd();
         current->MasterToLocal(&motherPoint[0], localPoint);
         current->MasterToLocalVect(&motherVelocity[0], localVelocity);
			current->MasterToLocalVect(&motherField[0], &localfield[0]);
         // ***************************************************************************************************
			#ifdef VERBOSE_MODE		
				Info("FindNextDaughterBoundaryAlongParabola","Divided Volume. Calculate distance from outside to first cell.");
				Info("FindNextDaughterBoundaryAlongParabola", "Volume: %s. Local Field: X: %f, Y: %f, Z: %f", current->GetName(), localfield[0], localfield[1], localfield[2]);
			#endif
			tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localfield, this->GetStep(),  3);  
			snext = TUCNParabola::Instance()->ArcLength(localVelocity, localfield, tnext);
         if (snext < (this->GetStep() - fgTolerance)) {
            if (compmatrix) {
               this->GetHMatrix()->CopyFrom(globalMatrix);
               this->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            this->SetStep(snext);
				this->SetStepTime(tnext);
				fUCNNextNode = current;
            nodefound = current;
            idaughter = ifirst;
         }
      }   
      if (ilast==ifirst) return nodefound;
      if (ilast>=0) { 
         current = vol->GetNode(ilast);
         current->cd();
         current->MasterToLocal(&motherPoint[0], localPoint);
         current->MasterToLocalVect(&motherVelocity[0], localVelocity);
			current->MasterToLocalVect(&motherField[0], localfield);
			// ***************************************************************************************************
			#ifdef VERBOSE_MODE		
				Info("FindNextDaughterBoundaryAlongParabola","Divided Volume. Calculate distance from outside to last cell.");
				Info("FindNextDaughterBoundaryAlongParabola", "Volume: %s. Local Field: X: %f, Y: %f, Z: %f", current->GetName(), localfield[0], localfield[1], localfield[2]);
			#endif
			tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localfield, this->GetStep(), 3);
			snext = TUCNParabola::Instance()->ArcLength(localVelocity, localfield, tnext);
         if (snext < (this->GetStep() - fgTolerance)) {
            if (compmatrix) {
               this->GetHMatrix()->CopyFrom(globalMatrix);
               this->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            this->SetStep(snext);
				this->SetStepTime(tnext);
				fUCNNextNode = current;
            nodefound = current;
            idaughter = ilast;
         }
      }   
      return nodefound;
   }
   // if only few daughters, check all and exit
   TGeoVoxelFinder *voxels = vol->GetVoxels();
   Int_t indnext;
   if (nd<5 || !voxels) {
      for (i=0; i<nd; i++) {
         current = vol->GetNode(i);
         if (fUCNGeometry->IsActivityEnabled() && !current->GetVolume()->IsActive()) continue;
         current->cd();
         if (voxels && voxels->IsSafeVoxel(motherPoint, i, this->GetStep())) continue;
         current->MasterToLocal(motherPoint, localPoint);
         current->MasterToLocalVect(motherVelocity, localVelocity);
			current->MasterToLocalVect(motherField, localfield);
         if (current->IsOverlapping() && current->GetVolume()->Contains(localPoint)) continue;
			// ***************************************************************************************************
         #ifdef VERBOSE_MODE		
				Info("FindNextDaughterBoundaryAlongParabola","Only Few daughters. Calculate distance from outside to all.");
				Info("FindNextDaughterBoundaryAlongParabola", "Volume: %s. Local Field: X: %f, Y: %f, Z: %f", current->GetName(), localfield[0], localfield[1], localfield[2]);
			#endif
			tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localfield, this->GetStep(),  3);
			snext = TUCNParabola::Instance()->ArcLength(localVelocity, localfield, tnext);
         if (snext < (this->GetStep() - fgTolerance)) {
				#ifdef VERBOSE_MODE		
					Info("FindNextDaughterBoundaryAlongParabola","Distance to daughter is less than distance to exit current volume.");
            #endif
				indnext = current->GetVolume()->GetNextNodeIndex();
				if (compmatrix) {
               this->GetHMatrix()->CopyFrom(globalMatrix);
               this->GetHMatrix()->Multiply(current->GetMatrix());
            }    
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            this->SetStep(snext);
				this->SetStepTime(tnext);
				fUCNNextNode = current;
            nodefound = fUCNNextNode;   
            idaughter = i;   
            while (indnext>=0) {
               current = current->GetDaughter(indnext);
               if (compmatrix) this->GetHMatrix()->Multiply(current->GetMatrix());
               fUCNNextNode = current;
               nodefound = current;
               indnext = current->GetVolume()->GetNextNodeIndex();
            }
         }
      }
		#ifdef VERBOSE_MODE		
			Info("FindNextDaughterBoundaryAlongParabola","Return nearest Node found if any.");
      #endif
		return nodefound;
   }
   // if current volume is voxelized, first get current voxel
   Int_t ncheck = 0;
   Int_t sumchecked = 0;
   Int_t *vlist = 0;
   voxels->SortCrossedVoxels(motherPoint, motherVelocity);
   while ((sumchecked<nd) && (vlist=voxels->GetNextVoxel(motherPoint, motherVelocity, ncheck))) {
      for (i=0; i<ncheck; i++) {
         current = vol->GetNode(vlist[i]);
         if (fUCNGeometry->IsActivityEnabled() && !current->GetVolume()->IsActive()) continue;
         current->cd();
         current->MasterToLocal(motherPoint, localPoint);
         current->MasterToLocalVect(motherVelocity, localVelocity);
			current->MasterToLocalVect(&motherField[0], &localfield[0]);
         if (current->IsOverlapping() && current->GetVolume()->Contains(localPoint)) continue;
			#ifdef VERBOSE_MODE		
				Info("FindNextDaughterBoundaryAlongParabola","Volume is voxelized. Calculate distance from outside to voxel.");
				Info("FindNextDaughterBoundaryAlongParabola", "Volume: %s. Local Field: X: %f, Y: %f, Z: %f", current->GetName(), localfield[0], localfield[1], localfield[2]);
			#endif
			tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(localPoint, localVelocity, localfield, this->GetStep(), 3);
			snext = TUCNParabola::Instance()->ArcLength(localVelocity, localfield, tnext);
         sumchecked++;
//         printf("checked %d from %d : snext=%g\n", sumchecked, nd, snext);
         if (snext < (this->GetStep() - fgTolerance)) {
            indnext = current->GetVolume()->GetNextNodeIndex();
				if (compmatrix) {
               this->GetHMatrix()->CopyFrom(globalMatrix);
               this->GetHMatrix()->Multiply(current->GetMatrix());
            }
            fUCNIsStepExiting  = kFALSE;
            fUCNIsStepEntering = kTRUE;
            this->SetStep(snext);
				this->SetStepTime(tnext);
				fUCNNextNode = current;
            nodefound = fUCNNextNode;
            idaughter = vlist[i];
            while (indnext>=0) {
               current = current->GetDaughter(indnext);
               if (compmatrix) this->GetHMatrix()->Multiply(current->GetMatrix());
               fUCNNextNode = current;
               nodefound = current;
               indnext = current->GetVolume()->GetNextNodeIndex();
            }
         }
      }
   }
   return nodefound;
}

//_____________________________________________________________________________
TGeoNode* TUCNGeoNavigator::FindNextBoundaryAndStepAlongParabola(TVirtualGeoTrack* track, TUCNGravField* field, Double_t stepTime, Bool_t compsafe)
{
// Compute distance to next boundary within STEPMAX. If no boundary is found,
// propagate current point along current direction with fStep=STEPMAX. Otherwise
// propagate with fStep=SNEXT (distance to boundary) and locate/return the next 
// node.
	
	// -- Get the track's particle
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	
	// -- Get the global field
	if (fUCNGeometry->GetGravField() == NULL) {
		throw runtime_error("In TUCNGeoNavigator::FindNextBoundaryAndStepAlongParabola - Grav Field has not been set");
	}
	
	Double_t globalField[3] 	 = {field->Gx(), field->Gy(), field->Gz()}; 
	Double_t globalPoint[3] 	 = {particle->Vx(), particle->Vy(), particle->Vz()};
	Double_t globalDir[3]	 	 = {particle->DirX(), particle->DirY(), particle->DirZ()};
	Double_t globalVelocity[3]  = {particle->VelocityX(), particle->VelocityY(), particle->VelocityZ()};
	
	Double_t currentField[3]	 = {globalField[0], globalField[1], globalField[2]};
	Double_t currentPoint[3]    = {globalPoint[0], globalPoint[1], globalPoint[2]};
	Double_t currentDir[3]      = {globalDir[0], globalDir[1], globalDir[2]};
	Double_t currentVelocity[3] = {globalVelocity[0], globalVelocity[1], globalVelocity[2]};
	
	#ifdef VERBOSE_MODE
		Info("FindNextBoundaryAndStepAlongParabola", "Starting StepTime: %f", stepTime);
		Info("FindNextBoundaryAndStepAlongParabola", "Velocity: %f", particle->Velocity());
		Info("FindNextBoundaryAndStepAlongParabola", "Global Field: X: %f, Y: %f, Z: %f", globalField[0], globalField[1], globalField[2]);	
		Info("FindNextBoundaryAndStepAlongParabola", "Global Point: X: %f, Y: %f, Z: %f", globalPoint[0], globalPoint[1], globalPoint[2]);	
		Info("FindNextBoundaryAndStepAlongParabola", "Global Dir: X: %f, Y: %f, Z: %f", globalDir[0], globalDir[1], globalDir[2]);	
		Info("FindNextBoundaryAndStepAlongParabola", "Global Velocity: X: %f, Y: %f, Z: %f", globalVelocity[0], globalVelocity[1], globalVelocity[2]);	
	#endif	
	
	// Compute maximum stepsize
	const Double_t stepMax = TUCNParabola::Instance()->ArcLength(globalVelocity, globalField, stepTime);
//	const Double_t stepMax = static_cast<TUCNGeoBBox*>(fUCNGeometry->GetTopVolume()->GetShape())->DistanceAlongParabola(globalVelocity, globalField, stepTime);	
	
	// -- Some initialisations
	static Int_t icount = 0;
   icount++;
   Int_t iact = 3;
   Int_t nextindex;
   Bool_t is_assembly;
   fUCNIsStepExiting  = kFALSE;
   fUCNIsStepEntering = kFALSE;
	fUCNIsOutside = this->IsOutside();
	fUCNIsOnBoundary = this->IsOnBoundary();
   TGeoNode *skip;
	
	// -- Store time step and step distance -- these two should always be convertible
	this->SetStepTime(stepTime);
	this->SetStep(stepMax);
   Double_t snext = TGeoShape::Big();
	Double_t tnext = TGeoShape::Big();
		
	// *********************************************************************
	// -- BRANCH 0
	// -- Compute safety
   // *********************************************************************
	if (compsafe) {
      #ifdef VERBOSE_MODE		
			Warning("FindNextBoundaryAndStepAlongParabola","Branch 0. Step will not be computed if Safety > stepMax.");
		#endif
		fUCNIsOnBoundary = kFALSE;
      Safety();
      // -- If proposed step less than safety, nothing to check
      if (this->GetSafeDistance() > this->GetStep() + fgTolerance) return this->GetCurrentNode();
   }   
   
	// -- If we are on the boundary step off it by tolerance
	Double_t extra = (fUCNIsOnBoundary)?fgTolerance:0.0;
   fUCNIsOnBoundary = kFALSE;
   // -- Update fPoint
	currentPoint[0] += extra*currentDir[0];
   currentPoint[1] += extra*currentDir[1];
   currentPoint[2] += extra*currentDir[2];
	this->SetCurrentPoint(currentPoint);
	
	this->GetHMatrix()->CopyFrom(this->GetCurrentMatrix());
   	
	// *********************************************************************
	// -- BRANCH 1
	// -- fIsOutside means outside TOP volume - ie: outside geometry
   // *********************************************************************	
	if (fUCNIsOutside) {
      // -- Find Distance And Time to reach TOP volume -- Time is used to make the step. 
      // ***************************************************************************************************
		#ifdef VERBOSE_MODE		
			Info("FindNextBoundaryAndStepAlongParabola","Branch 1. Outside TOP volume (geometry). Calculate distance to TOP.");
			Info("FindNextBoundaryAndStepAlongParabola", "Global Field: X: %f, Y: %f, Z: %f", globalField[0],globalField[1],globalField[2]);	
		#endif
		tnext = static_cast<TUCNGeoBBox*>(fUCNGeometry->GetTopVolume()->GetShape())->TimeFromOutsideAlongParabola(currentPoint, currentVelocity, currentField, this->GetStep(), iact); 
		snext = TUCNParabola::Instance()->ArcLength(currentVelocity, currentField, tnext);;
		// -- Check if distance to Top is less than the current Stepsize
		if (snext < this->GetStep() - fgTolerance) {
			if (snext<=0) {
            // -- Distance to top <= 0. Set = 0, then make a step by extra
				snext = 0.0;
				tnext = 0.0;
				this->SetStep(snext);
				this->SetStepTime(tnext);
            // -- Update fPoint
				currentPoint[0] -= extra*currentDir[0];
			   currentPoint[1] -= extra*currentDir[1];
			   currentPoint[2] -= extra*currentDir[2];
				this->SetCurrentPoint(currentPoint);
         } else {
				// -- Distance to top is greater than 0. Set stored stepsize fStep to be this distance + extra. 
            this->SetStep(snext+extra);
				this->SetStepTime(tnext+extra);
         }
   		// -- Top is reachable - and distance to it is less than proposed step - hence we are Entering
         fUCNIsStepEntering = kTRUE;
         fUCNNextNode = fUCNGeometry->GetTopNode();
         nextindex = fUCNNextNode->GetVolume()->GetNextNodeIndex();
			while (nextindex>=0) {
            CdDown(nextindex);
            fUCNNextNode = this->GetCurrentNode();
            nextindex = fUCNNextNode->GetVolume()->GetNextNodeIndex();
            if (nextindex < 0) this->GetHMatrix()->CopyFrom(this->GetCurrentMatrix());
         }
   
         // -- Move global point by snext onto boundary
         // ***************************************************************************************************
			#ifdef VERBOSE_MODE		
				Info("FindNextBoundaryAndStepAlongParabola","Branch 1. Entering TOP. Updating Global Point with grav correction.");
				Info("FindNextBoundaryAndStepAlongParabola", "StepTime: %f", tnext);
			#endif
			const Double_t timestep = tnext;
			
			currentPoint[0] += currentVelocity[0]*timestep + 0.5*currentField[0]*timestep*timestep; 
		   currentPoint[1] += currentVelocity[1]*timestep + 0.5*currentField[1]*timestep*timestep; 
		   currentPoint[2] += currentVelocity[2]*timestep + 0.5*currentField[2]*timestep*timestep;	
			this->SetCurrentPoint(currentPoint);
			
			// -- Update Direction to reflect new position
			currentVelocity[0] = currentVelocity[0] + currentField[0]*timestep;
			currentVelocity[1] = currentVelocity[1] + currentField[1]*timestep;
			currentVelocity[2] = currentVelocity[2] + currentField[2]*timestep;
			Double_t velocityMag = TMath::Sqrt((currentVelocity[0]*currentVelocity[0]) + (currentVelocity[1]*currentVelocity[1]) + (currentVelocity[2]*currentVelocity[2])); 
			if (velocityMag == 0.) throw runtime_error("Velocity is zero!");
			
			currentDir[0] = currentVelocity[0]/velocityMag;
			currentDir[1] = currentVelocity[1]/velocityMag;
			currentDir[2] = currentVelocity[2]/velocityMag;
			this->SetCurrentDirection(currentDir);
			
			// -- Update track/particle properties
			this->UpdateTrack(track, timestep);
						
			fUCNIsOnBoundary = kTRUE;
         fUCNIsOutside = kFALSE;
         return this->CrossBoundaryAndLocate(kTRUE, this->GetCurrentNode());
      }
      // -- New point still outside, and the top node is not within snext, but is reachable.
		if (snext<TGeoShape::Big()) {
        	// -- Move global point by proposed stepsize fStep
			// ***************************************************************************************************
         #ifdef VERBOSE_MODE		
				Info("FindNextBoundaryAndStepAlongParabola","Branch 1. New Point outside but TOP is reachable. Updating Global Point.");
				Info("FindNextBoundaryAndStepAlongParabola", "StepTime: %f", tnext);
			#endif
			fUCNNextNode = fUCNGeometry->GetTopNode();
			const Double_t timestep = (this->GetStepTime())-extra;
			
			currentPoint[0] += currentVelocity[0]*timestep + 0.5*currentField[0]*timestep*timestep; 
		   currentPoint[1] += currentVelocity[1]*timestep + 0.5*currentField[1]*timestep*timestep; 
		   currentPoint[2] += currentVelocity[2]*timestep + 0.5*currentField[2]*timestep*timestep;	
			this->SetCurrentPoint(currentPoint);	
			
			// -- Update Direction to reflect new position
			currentVelocity[0] = currentVelocity[0] + currentField[0]*timestep;
			currentVelocity[1] = currentVelocity[1] + currentField[1]*timestep;
			currentVelocity[2] = currentVelocity[2] + currentField[2]*timestep;
			Double_t velocityMag = TMath::Sqrt((currentVelocity[0]*currentVelocity[0]) + (currentVelocity[1]*currentVelocity[1]) + (currentVelocity[2]*currentVelocity[2])); 
			if (velocityMag == 0.) throw runtime_error("Velocity is zero!");
			
			currentDir[0] = currentVelocity[0]/velocityMag;
			currentDir[1] = currentVelocity[1]/velocityMag;
			currentDir[2] = currentVelocity[2]/velocityMag;
			this->SetCurrentDirection(currentDir);
			
			// -- Update particle properties
			this->UpdateTrack(track, timestep);
		
			return fUCNNextNode;
      }      
      // top node not reachable from current point/direction
      #ifdef VERBOSE_MODE		
			Warning("FindNextBoundaryAndStepAlongParabola","Branch 1. TOP is NOT reachable.");
		#endif
		fUCNNextNode = 0;
      fUCNIsOnBoundary = kFALSE;
      return 0;
   }
   
	// *********************************************************************
	// -- BRANCH 2
	// -- So we are not outside the geometry. Get point and direction in local coordinate frame and calculate distance to boundary of current node. 
   // *********************************************************************
	Double_t localPoint[3], localVelocity[3], localField[3]; // Containers for the local point and dir
   Int_t icrossed = -2;
   this->GetCurrentMatrix()->MasterToLocal(currentPoint, &localPoint[0]);
   this->GetCurrentMatrix()->MasterToLocalVect(currentVelocity, &localVelocity[0]);
	this->GetCurrentMatrix()->MasterToLocalVect(currentField, &localField[0]);
   TGeoVolume *vol = this->GetCurrentNode()->GetVolume();
   
	// -- Find distance to exiting current node
   // ***************************************************************************************************
	#ifdef VERBOSE_MODE		
		Info("FindNextBoundaryAndStepAlongParabola","Branch 2. Find distance to exit current Volume: %s.", vol->GetName());
		Info("FindNextBoundaryAndStepAlongParabola", "Local Field: X: %f, Y: %f, Z: %f", localField[0], localField[1], localField[2]);
	#endif
	tnext = static_cast<TUCNGeoBBox*>(vol->GetShape())->TimeFromInsideAlongParabola(localPoint, localVelocity, localField, this->GetStep(), iact); 
	snext = TUCNParabola::Instance()->ArcLength(localVelocity, localField, tnext);
	fUCNNextNode = this->GetCurrentNode();
	// -- If distance to exiting current node is <= Tolerance value (1e-10) make a small step by this tolerance value
	if (snext <= fgTolerance) {
      snext = fgTolerance;
		tnext = fgTolerance;
		this->SetStep(snext);
		this->SetStepTime(tnext);
      fUCNIsOnBoundary = kTRUE;
      fUCNIsStepEntering = kFALSE;
      fUCNIsStepExiting = kTRUE;
      skip = this->GetCurrentNode();
		
		// -- Move global point by Tolerance value
		// ***************************************************************************************************
	   currentPoint[0] += this->GetStep()*currentDir[0]; 
	   currentPoint[1] += this->GetStep()*currentDir[1]; 
	   currentPoint[2] += this->GetStep()*currentDir[2];	
		this->SetCurrentPoint(currentPoint);
		
		// Update Particle
		this->UpdateTrack(track, tnext);
		
      is_assembly = this->GetCurrentNode()->GetVolume()->IsAssembly();
      if (!(this->GetLevel()) && !is_assembly) {
         fUCNIsOutside = kTRUE;
         return 0;
      }
      if (this->GetCurrentNode()->IsOffset()) {
			#ifdef VERBOSE_MODE		
				Warning("FindNextBoundaryAndStepAlongParabola","Branch 2. fCurrentNode->IsOffset(). Entering CrossDivisionCell().");
			#endif
			return CrossDivisionCell();   
      }
		// -- Cross Boundary and return new volume
		if (this->GetLevel()) CdUp(); 
      else        skip = 0;
      return CrossBoundaryAndLocate(kFALSE, skip);
   }   
	
	// -- If distance to exiting current node is smaller than proposed Stepsize - then set our stepsize fStep to this distance (snext)
   if (snext < this->GetStep() - fgTolerance) {
      icrossed = -1;
      this->SetStep(snext);
		this->SetStepTime(tnext);
      fUCNIsStepEntering = kFALSE;
      fUCNIsStepExiting = kTRUE;
   }
   
	// Find next daughter boundary for the current volume
   // ***************************************************************************************************
	Int_t idaughter = -1;
	TGeoNode *crossed = this->FindNextDaughterBoundaryAlongParabola(localPoint, localVelocity, localField, idaughter, kTRUE);
   if (crossed) {
      fUCNIsStepExiting = kFALSE;
      icrossed = idaughter;
      fUCNIsStepEntering = kTRUE;
   }   
   TGeoNode *current = 0;
   TGeoNode *dnode = 0;
   TGeoVolume *mother = 0;
   
	// *********************************************************************
	// -- BRANCH 3
	// -- if we are in an overlapping node, check also the mother(s) - fNmany is number of overlapping nodes on current branch
   // *********************************************************************
   if (this->GetNmany()) {
      Double_t motherPoint[3], motherVelocity[3], motherField[3];
		Double_t daughterPoint[3], daughterVelocity[3], daughterField[3];
      Int_t novlps;
      Int_t safelevel = GetSafeLevel();
      PushPath(safelevel+1);
      while (this->IsCurrentOverlapping()) {
         Int_t *ovlps = this->GetCurrentNode()->GetOverlaps(novlps);
         CdUp();
         mother = this->GetCurrentNode()->GetVolume();
         this->GetCurrentMatrix()->MasterToLocal(currentPoint, &motherPoint[0]);
         this->GetCurrentMatrix()->MasterToLocalVect(currentVelocity, &motherVelocity[0]);
			this->GetCurrentMatrix()->MasterToLocalVect(currentField, &motherField[0]);
         // -- Check distance to out
         snext = TGeoShape::Big();
			tnext = TGeoShape::Big();
         if (!mother->IsAssembly()) {
         	// ***************************************************************************************************
				#ifdef VERBOSE_MODE		
					Info("FindNextBoundaryAndStepAlongParabola","Branch 3. In overlapping Node - check distance to mother");
					Info("FindNextBoundaryAndStepAlongParabola", "Moth Local Field: X: %f, Y: %f, Z: %f", motherField[0], motherField[1], motherField[2]);
				#endif
				tnext = static_cast<TUCNGeoBBox*>(mother->GetShape())->TimeFromInsideAlongParabola(motherPoint, motherVelocity, motherField, this->GetStep(), iact); 
				snext = TUCNParabola::Instance()->ArcLength(motherVelocity, motherField, tnext);
         }
			if (snext < this->GetStep() - fgTolerance) {
            // exiting mother first (extrusion)
            icrossed = -1;
            PopDummy();
            PushPath(safelevel+1);
            fUCNIsStepEntering = kFALSE;
            fUCNIsStepExiting = kTRUE;
            this->SetStep(snext);
				this->SetStepTime(tnext);
            this->GetCurrentMatrix()->CopyFrom(this->GetHMatrix());
            fUCNNextNode = this->GetCurrentNode();
         }
         // check overlapping nodes
         for (Int_t i=0; i<novlps; i++) {
            current = mother->GetNode(ovlps[i]);
            if (!current->IsOverlapping()) {
               current->cd();
               current->MasterToLocal(&motherPoint[0], &daughterPoint[0]);
               current->MasterToLocalVect(&motherVelocity[0], &daughterVelocity[0]);
               current->MasterToLocalVect(&motherField[0], &daughterField[0]);
					// Current point may be inside the other node - geometry error that we ignore
               snext = TGeoShape::Big();
					tnext = TGeoShape::Big();
					if (!current->GetVolume()->Contains(daughterPoint))
						// ***************************************************************************************************
			         #ifdef VERBOSE_MODE		
							Info("FindNextBoundaryAndStepAlongParabola","Branch 3. In overlapping Node - check distance to enter other nodes");
							Info("FindNextBoundaryAndStepAlongParabola","Local Field: X: %f, Y: %f, Z: %f", daughterField[0], daughterField[1], daughterField[2]);
						#endif
						tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(daughterPoint, daughterVelocity, daughterField, this->GetStep(), iact); 
						snext = TUCNParabola::Instance()->ArcLength(daughterVelocity, daughterField, tnext);
               if (snext < this->GetStep() - fgTolerance) {
                  PopDummy();
                  PushPath(safelevel+1);
                  this->GetCurrentMatrix()->CopyFrom(this->GetHMatrix());
                  this->GetCurrentMatrix()->Multiply(current->GetMatrix());
                  fUCNIsStepEntering = kFALSE;
                  fUCNIsStepExiting = kTRUE;
                  icrossed = ovlps[i];
                  this->SetStep(snext);
                  this->SetStepTime(tnext);
						fUCNNextNode = current;
               }
            } else {
               // another many - check if point is in or out
               current->cd();
               current->MasterToLocal(&motherPoint[0], &daughterPoint[0]);
               current->MasterToLocalVect(&motherVelocity[0], &daughterVelocity[0]);
               current->MasterToLocalVect(&motherField[0], &daughterField[0]);
					if (current->GetVolume()->Contains(daughterPoint)) {
                  if (current->GetVolume()->GetNdaughters()) {
                     CdDown(ovlps[i]);
                     // ***************************************************************************************************
							dnode = FindNextDaughterBoundaryAlongParabola(daughterPoint, daughterVelocity, daughterField, idaughter, kFALSE);
                     if (dnode) {
                        this->GetCurrentMatrix()->CopyFrom(this->GetHMatrix());
                        this->GetCurrentMatrix()->Multiply(dnode->GetMatrix());
                        icrossed = idaughter;
                        PopDummy();
                        PushPath(safelevel+1);
                        fUCNIsStepEntering = kFALSE;
                        fUCNIsStepExiting = kTRUE;
                        fUCNNextNode = dnode;
                     }   
                     CdUp();
                  }   
               } else {
						// ***************************************************************************************************
						#ifdef VERBOSE_MODE		
							Info("FindNextBoundaryAndStepAlongParabola","Branch 3. In overlapping Node - check distance to exit");
							Info("FindNextBoundaryAndStepAlongParabola", "Local Field: X: %f, Y: %f, Z: %f",daughterField[0],daughterField[1],daughterField[2]);
						#endif
						tnext = static_cast<TUCNGeoBBox*>(current->GetVolume()->GetShape())->TimeFromOutsideAlongParabola(daughterPoint, daughterVelocity, daughterField, this->GetStep(), iact); 
						snext = TUCNParabola::Instance()->ArcLength(daughterVelocity, daughterField, tnext);
                  if (snext < this->GetStep() - fgTolerance) {
                     this->GetCurrentMatrix()->CopyFrom(this->GetHMatrix());
                     this->GetCurrentMatrix()->Multiply(current->GetMatrix());
                     fUCNIsStepEntering = kFALSE;
                     fUCNIsStepExiting = kTRUE;
                     this->SetStep(snext);
                     this->SetStepTime(tnext);
							fUCNNextNode = current;
                     icrossed = ovlps[i];
                     PopDummy();
                     PushPath(safelevel+1);
                  }               
               }  
            }
         }
      }
      // Now we are in a non-overlapping node
      if (this->GetNmany()) {
      // We have overlaps up in the branch, check distance to exit
         Int_t up = 1;
         Int_t imother;
         Int_t nmany = this->GetNmany();
         Bool_t ovlp = kFALSE;
         Bool_t nextovlp = kFALSE;
         Bool_t offset = kFALSE;
         TGeoNode *currentnode = this->GetCurrentNode();
         TGeoNode *mothernode, *mup;
         TGeoHMatrix *matrix;
         while (nmany) {
            mothernode = GetMother(up);
            mup = mothernode;
            imother = up+1;
            offset = kFALSE;
            while (mup->IsOffset()) {
               mup = GetMother(imother++);
               offset = kTRUE;
            }   
            nextovlp = mup->IsOverlapping();
            if (offset) {
               mothernode = mup;
               if (nextovlp) nmany -= imother-up;
               up = imother-1;
            } else {    
               if (ovlp) nmany--;
            }
            if (ovlp || nextovlp) {
               matrix = GetMotherMatrix(up);
               matrix->MasterToLocal(currentPoint, daughterPoint);
               matrix->MasterToLocalVect(currentVelocity, daughterVelocity);
               matrix->MasterToLocalVect(currentField, daughterField);
					snext = TGeoShape::Big();
					tnext = TGeoShape::Big();
					if (!mothernode->GetVolume()->IsAssembly()) {
						// ***************************************************************************************************
						#ifdef VERBOSE_MODE		
							Info("FindNextBoundaryAndStepAlongParabola","Branch 3. Non-overlapping Node - checking distance to exit");	
							Info("FindNextBoundaryAndStepAlongParabola", "Local Field: X: %f, Y: %f, Z: %f",daughterField[0],daughterField[1],daughterField[2]);
						#endif
						tnext = static_cast<TUCNGeoBBox*>(mothernode->GetVolume()->GetShape())->TimeFromInsideAlongParabola(daughterPoint, daughterVelocity, daughterField, this->GetStep(), iact); 
						snext = TUCNParabola::Instance()->ArcLength(daughterVelocity, daughterField, tnext);
					}
               fUCNIsStepEntering = kFALSE;
               fUCNIsStepExiting  = kTRUE;
               if (snext < this->GetStep() - fgTolerance) {
                  fUCNNextNode = mothernode;
                  this->GetCurrentMatrix()->CopyFrom(matrix);
                  this->SetStep(snext);
                  this->SetStepTime(tnext);
						while (up--) CdUp();
                  PopDummy();
                  PushPath();
                  icrossed = -1;
                  up = 1;
                  currentnode = this->GetCurrentNode();
                  ovlp = currentnode->IsOverlapping();
                  continue;
               }   
            }   
            currentnode = mothernode;
            ovlp = nextovlp;
            up++;            
         }
      }      
      PopPath();
   }
   #ifdef VERBOSE_MODE		
	 	Info("FindNextBoundaryAndStepAlongParabola","Branch 3. Updating Global Point. fTimeStep: %f, fStep: %f", this->GetStepTime(), this->GetStep());
	#endif
	const Double_t timestep = this->GetStepTime();
	currentPoint[0] += currentVelocity[0]*timestep + 0.5*currentField[0]*timestep*timestep; 
   currentPoint[1] += currentVelocity[1]*timestep + 0.5*currentField[1]*timestep*timestep; 
   currentPoint[2] += currentVelocity[2]*timestep + 0.5*currentField[2]*timestep*timestep;	
	this->SetCurrentPoint(currentPoint);
	
	// -- Update Direction to reflect new position
	currentVelocity[0] = currentVelocity[0] + currentField[0]*timestep;
	currentVelocity[1] = currentVelocity[1] + currentField[1]*timestep;
	currentVelocity[2] = currentVelocity[2] + currentField[2]*timestep;
	Double_t velocityMag = TMath::Sqrt((currentVelocity[0]*currentVelocity[0]) + (currentVelocity[1]*currentVelocity[1]) + (currentVelocity[2]*currentVelocity[2])); 
	if (velocityMag == 0.) throw runtime_error("Velocity is zero!");
	
	currentDir[0] = currentVelocity[0]/velocityMag;
	currentDir[1] = currentVelocity[1]/velocityMag;
	currentDir[2] = currentVelocity[2]/velocityMag;
	this->SetCurrentDirection(currentDir);
	
	// -- Update particle properties
	this->UpdateTrack(track, timestep);
	
	this->SetStep(this->GetStep() + extra);
	this->SetStepTime(this->GetStepTime() + extra);
	
	// *********************************************************************
	// -- BRANCH 4
	// -- Final check on results of above
   // *********************************************************************
	if (icrossed == -2) {
      // Nothing crossed within stepMax -> propagate and return same location   
      #ifdef VERBOSE_MODE		
			Info("FindNextBoundaryAndStepAlongParabola","Branch 4. Nothing crossed within stepMax. Propagating point and returning same location.");
		#endif
		fUCNIsOnBoundary = kFALSE;
      return this->GetCurrentNode();
   }
   fUCNIsOnBoundary = kTRUE;
   if (icrossed == -1) {
      #ifdef VERBOSE_MODE		
			Info("FindNextBoundaryAndStepAlongParabola","Branch 4. On Boundary. Crossing boundary and locating.");
		#endif
		skip = this->GetCurrentNode();
      is_assembly = this->GetCurrentNode()->GetVolume()->IsAssembly();
      if (!(this->GetLevel()) && !is_assembly) {
         fUCNIsOutside = kTRUE;
         return 0;
      }   
      if (this->GetCurrentNode()->IsOffset()) return CrossDivisionCell();   
      if (this->GetLevel()) CdUp();
      else        skip = 0;
//      is_assembly = fCurrentNode->GetVolume()->IsAssembly();
//      while (fLevel && is_assembly) {
//         CdUp();
//         is_assembly = fCurrentNode->GetVolume()->IsAssembly();
//         skip = fCurrentNode;
//      }   
      return CrossBoundaryAndLocate(kFALSE, skip);
   }   
   current = this->GetCurrentNode();   
   CdDown(icrossed);
   nextindex = this->GetCurrentNode()->GetVolume()->GetNextNodeIndex();
	while (nextindex>=0) {
      current = this->GetCurrentNode();
      CdDown(nextindex);
      nextindex = this->GetCurrentNode()->GetVolume()->GetNextNodeIndex();
   }   
	#ifdef VERBOSE_MODE		
		Info("FindNextBoundaryAndStepAlongParabola","Branch 4. Crossing boundary and locating.");
   #endif
	return CrossBoundaryAndLocate(kTRUE, current);	
}

//_____________________________________________________________________________
Double_t TUCNGeoNavigator::DetermineNextStepTime(TUCNParticle* particle, const Double_t runTime)			 
{
// Placeholder for method to calculate the next step time depending on electric/magnetic field environment
	if (this->GetStepTime() == 0.0) {
		this->SetStepTime(0.01*Units::s);
	}
	
	// -- Check if we will reach the maximum runtime of the track. If so propagate only until this time.
	if (particle->T() > (runTime - this->GetStepTime())) {
		this->SetStepTime((runTime - particle->T()) + fgTolerance);
	}
	return this->GetStepTime();
}

//_____________________________________________________________________________
void TUCNGeoNavigator::SetStepTime(Double_t stepTime) 
{
// 	
	if (stepTime <= 0.0) {
		Error("SetStepTime", "StepTime %f , cannot be set to zero or a negative value. StepTime will be set to the default value of 0.01s", stepTime);
		fStepTime = 0.01*Units::s;
	} else{
		fStepTime = stepTime;
	}
}

//_____________________________________________________________________________
Bool_t TUCNGeoNavigator::PropagateTrack(TVirtualGeoTrack* track, const Double_t runTime, const Double_t maxStepTime)
{
// Propagate track through geometry until it is either stopped or the runTime has been reached
// Track passed MUST REFERENCE A TUCNPARTICLE as its particle type. 
// In future I will attempt to guarentee this. (4/06/09) 

// UNITS:: runTime, stepTime in Seconds

	Int_t stepNumber;
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());

#ifdef VERBOSE_MODE				
	Info("PropagateForSetTime","Starting Run - Max time (seconds): %f", runTime);
#endif
	
	// -- Check that Particle has not been initialised inside a boundary or detector		
	if (static_cast<TUCNGeoMaterial*>(this->GetCurrentNode()->GetMedium()->GetMaterial())->IsTrackingMaterial() == kFALSE) {
		cout << this->GetCurrentNode()->GetMedium()->GetName() << endl;
		throw runtime_error("In TUCNGeoNavigator::PropagateTrack - particle initialised inside a boundary");
	}
	
	// -- Get the Grav Field
	TUCNGravField* field = fUCNGeometry->GetGravField();
	
	///////////////////////////////////	
	// -- Propagation Loop
	///////////////////////////////////
	for (stepNumber = 1 ; stepNumber < fgMaxSteps ; stepNumber++) {
		
		#ifdef VERBOSE_MODE		
			cout << "STEP " << stepNumber << "\t" << particle->T() << " s" << "\t" << this->GetCurrentNode()->GetName() << endl;	
		#endif
		
		// -- Calculate the Next StepTime
		this->SetStepTime(maxStepTime); // Start with the default value
		this->DetermineNextStepTime(particle, runTime); // Check if there are any factors that will reduce this time
		
		// -- Find time to reach next boundary and step along parabola
		#ifdef VERBOSE_MODE	
			cout << "------------------- START OF STEP ----------------------" << endl;
			cout << "Initial Steptime (s): " << this->GetStepTime() << endl;
			cout << "Initial Vertex (m): " << "X:" << particle->Vx() << "\t" << "Y:" << particle->Vy()  << "\t" << "Z:" << particle->Vz() << "\t" << "T:" << particle->T() << endl;
			cout << "Initial Dir: " << "X:" << particle->DirX()  << "\t" << "Y:" << particle->DirY()  << "\t" << "Z:" << particle->DirZ() << "\t" << "Mag:" << particle->Dir() << endl;
			cout << "Initial Vel (m/s): " << "X:" << particle->VelocityX()  << "\t" << "Y:" << particle->VelocityY()  << "\t" << "Z:" << particle->VelocityZ() << "\t" << "V:" << particle->Velocity() << endl;
			cout << "Initial Mom (eV): " << "X:" << particle->Px()  << "\t" << "Y:" << particle->Py()  << "\t" << "Z:" << particle->Pz() << "\t" << "P:" << particle->P() << endl;
			cout << "Initial Volume: "    << this->GetCurrentVolume()->GetName() << endl;
			cout << "Initial Energy (neV): "    << particle->Energy() /Units::neV << endl;
			cout << "-----------------------------" << endl;
			cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
			cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
			cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
			cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
			cout << "-----------------------------" << endl << endl;
		#endif
		
		
		if (field == NULL) {
			this->FindNextBoundaryAndStep(this->GetStepTime());
			this->UpdateTrack(track, this->GetStepTime());
			// TODO: UPDATE CLASS DATA MEMBERS NOW LIKE fUCNNextNode
			
		} else {
			// -- Update Particle is called by FindNext...AlongParabola so no need to repeat that here
			this->FindNextBoundaryAndStepAlongParabola(track, field, this->GetStepTime());
		}
		
			
		#ifdef VERBOSE_MODE	
			cout << "------------------- END OF STEP ----------------------" << endl;
			cout << "Final Steptime (s): " << this->GetStepTime() << endl;
			cout << "Final Vertex (m): " << "X:" << particle->Vx() << "\t" << "Y:" << particle->Vy()  << "\t" << "Z:" << particle->Vz() << "\t" << "T:" << particle->T() << endl;
			cout << "Final Dir: " << "X:" << particle->DirX()  << "\t" << "Y:" << particle->DirY()  << "\t" << "Z:" << particle->DirZ() << "\t" << "Mag:" << particle->Dir() << endl;
			cout << "Final Vel (m/s): " << "X:" << particle->VelocityX()  << "\t" << "Y:" << particle->VelocityY()  << "\t" << "Z:" << particle->VelocityZ() << "\t" << "V:" << particle->Velocity() << endl;
			cout << "Final Mom (eV): " << "X:" << particle->Px()  << "\t" << "Y:" << particle->Py()  << "\t" << "Z:" << particle->Pz() << "\t" << "P:" << particle->P() << endl;
			cout << "Final Volume: "    << this->GetCurrentVolume()->GetName() << endl;
			cout << "Final Energy (neV): "    << particle->Energy() /Units::neV << endl;
			cout << "-----------------------------" << endl;
			cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
			cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
			cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
			cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
			cout << "-----------------------------" << endl << endl;
		#endif	
		
		TUCNGeoMaterial* currentMaterial = static_cast<TUCNGeoMaterial*>(this->GetCurrentNode()->GetMedium()->GetMaterial());
		
		// -- Determine what to do if we are on a boundary
		// -- Is Track on the surface of a boundary?
		if (currentMaterial->IsTrackingMaterial() == kFALSE) {
			// -- Are we on the surface of a detector?
			if (currentMaterial->IsDetectorMaterial() == kTRUE) {
				// -- Was particle detected?
				Double_t prob = gRandom->Uniform(0.0,1.0);
				if (prob < currentMaterial->DetectionEfficiency()) {	
					particle->Detected(kTRUE);  // Set detected flag
				} else {	
				#ifdef VERBOSE_MODE	
					cout << "------------------- BOUNCE ----------------------" << endl;
					cout << "Current medium: " << this->GetCurrentNode()->GetMedium()->GetName() << endl;
				#endif
					// -- Make a Bounce
					this->Bounce(track);	
					// -- Cross Boundary and Locate
					this->CrossBoundaryAndLocate(kFALSE, this->GetCurrentNode()); 	
					this->UpdateTrack(track);
				#ifdef VERBOSE_MODE	
					cout << "-------------------------------------------------" << endl;
					cout << "Final medium: " << this->GetCurrentNode()->GetMedium()->GetName() << endl;
					cout << "-------------------------------------------------" << endl << endl;
				#endif
				}
			// -- Was particle lost to boundary (absorbed/upscattered) ?
			} else if (kFALSE) {	
				particle->Lost(kTRUE); // Set lost flag
			// -- Are we outside the geometry heirarchy we have built - ie: in TOP
			} else if (currentMaterial->IsBlackhole() == kTRUE) {
				particle->Lost(kTRUE);
			//	throw runtime_error("In TUCNGeoNavigator::PropagateTrack - track has entered TOP/Blackhole!");
			} else {
				#ifdef VERBOSE_MODE	
					cout << "------------------- BOUNCE ----------------------" << endl;
					cout << "Current Node: " << this->GetCurrentNode()->GetName() << endl;
				#endif	
				// -- Make a Bounce
				this->Bounce(track);
				// -- Cross Boundary and Locate
				if (this->IsUCNStepEntering() == kTRUE) {
					#ifdef VERBOSE_MODE	
						cout << "Search Upwards - Step entering a volume so we must step back out" << endl;
						cout << this->GetCurrentNode()->GetVolume()->GetNextNodeIndex() << endl;
					#endif
					this->CrossBoundaryAndLocate(kFALSE, this->GetCurrentNode()); 	
						
				} else {
					#ifdef VERBOSE_MODE
						cout << "Search Downwards - Step exited a volume so we must re-enter" << endl;
						cout << this->GetCurrentNode()->GetVolume()->GetNextNodeIndex() << endl;
					#endif
					this->CrossBoundaryAndLocate(kTRUE, this->GetCurrentNode());
				}
				this->UpdateTrack(track);
				#ifdef VERBOSE_MODE	
					cout << "-------------------------------------------------" << endl;
					cout << "Final Node: " << this->GetCurrentNode()->GetName() << endl;
					cout << "-------------------------------------------------" << endl << endl;
				#endif
			}
		}
		
		// -- Determine destination
		// -- Has lost flag been set?
		if (particle->Lost() == kTRUE) {
			break; // -- End Propagtion Loop
		// -- Has detected flag been set?
		} else if (particle->Detected() == kTRUE) {
			break; // -- End Propagation Loop
		// -- Has particle decayed within steptime?
		} else if (particle->WillDecay(this->GetStepTime()) == kTRUE) {
			particle->Decayed(kTRUE);// Set Decay Flag
			break; // -- End Propagation Loop
		// -- Have we reached the maximum runtime?
		} else if (particle->T() >= runTime) {
			break; // -- End Propagation Loop
		}
	// -- END OF PROPAGATION LOOP
	}	
							
	cout << "FINAL STATUS: " << "Track: " << track->GetId() << "\t" << "Steps taken: " << stepNumber << "\t";
	cout << "Time: " << particle->T() << "s" << "\t" << "Final Medium: " << this->GetCurrentNode()->GetMedium()->GetName() << "\t";
	cout << "Bounces: " << particle->Bounces() << "\t" << "Diffuse: " << particle->DiffuseBounces() << "\t" << "Specular: " << particle->SpecularBounces() << endl;
	
	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNGeoNavigator::PropagateTrack(TVirtualGeoTrack* track, const Int_t steps, const Double_t maxStepTime)
{
// Propagate track through geometry until it is either stopped or the max steps has been reached
// Track passed MUST REFERENCE A TUCNPARTICLE as its particle type. 
// In future I will attempt to guarentee this. (4/06/09) 

	Int_t stepNumber;
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());

#ifdef VERBOSE_MODE				
	Info("PropagateTrack","Starting Run - Max steps: %i", steps);
#endif
	
	// -- Check that Particle has not been initialised inside a boundary or detector		
	if (static_cast<TUCNGeoMaterial*>(this->GetCurrentNode()->GetMedium()->GetMaterial())->IsTrackingMaterial() == kFALSE) {
		cout << this->GetCurrentNode()->GetMedium()->GetName() << endl;
		throw runtime_error("In TUCNGeoNavigator::PropagateTrack - particle initialised inside a boundary");
	}
	
	// -- Get the Grav Field
	TUCNGravField* field = fUCNGeometry->GetGravField();
	
	if (steps <= 0) {
		Error("PropagateTrack", "No. of Steps must be greater than or equal to 1");
		return kFALSE;
	}
	
	///////////////////////////////////	
	// -- Propagation Loop
	///////////////////////////////////
	for (stepNumber = 1 ; stepNumber <= steps ; stepNumber++) {
		
		#ifdef VERBOSE_MODE		
			cout << "STEP " << stepNumber << "\t" << particle->T() << " s" << "\t" << this->GetCurrentNode()->GetName() << endl;	
		#endif
		
		// -- Define the next steptime
		this->SetStepTime(maxStepTime);
		
		// -- Find time to reach next boundary and step along parabola
		#ifdef VERBOSE_MODE	
			cout << "------------------- START OF STEP ----------------------" << endl;
			cout << "Initial Steptime (s): " << this->GetStepTime() << endl;
			cout << "Initial Vertex (m): " << "X:" << particle->Vx() << "\t" << "Y:" << particle->Vy()  << "\t" << "Z:" << particle->Vz() << "\t" << "T:" << particle->T() << endl;
			cout << "Initial Dir: " << "X:" << particle->DirX()  << "\t" << "Y:" << particle->DirY()  << "\t" << "Z:" << particle->DirZ() << "\t" << "Mag:" << particle->Dir() << endl;
			cout << "Initial Vel (m/s): " << "X:" << particle->VelocityX()  << "\t" << "Y:" << particle->VelocityY()  << "\t" << "Z:" << particle->VelocityZ() << "\t" << "V:" << particle->Velocity() << endl;
			cout << "Initial Mom (eV): " << "X:" << particle->Px()  << "\t" << "Y:" << particle->Py()  << "\t" << "Z:" << particle->Pz() << "\t" << "P:" << particle->P() << endl;
			cout << "Initial Volume: "    << this->GetCurrentVolume()->GetName() << endl;
			cout << "Initial Energy (neV): "    << particle->Energy() /Units::neV << endl;
			cout << "-----------------------------" << endl;
			cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
			cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
			cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
			cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
			cout << "-----------------------------" << endl << endl;
		#endif
		
		if (field == NULL) {
			this->FindNextBoundaryAndStep(this->GetStepTime());
			this->UpdateTrack(track, this->GetStepTime());
			// TODO: UPDATE CLASS DATA MEMBERS NOW LIKE fUCNNextNode
			
		} else {
			// -- Update Particle is called by FindNext...AlongParabola so no need to repeat that here
			this->FindNextBoundaryAndStepAlongParabola(track, field, this->GetStepTime());
		}
			
		#ifdef VERBOSE_MODE	
			cout << "------------------- END OF STEP ----------------------" << endl;
			cout << "Final Steptime (s): " << this->GetStepTime() << endl;
			cout << "Final Vertex (m): " << "X:" << particle->Vx() << "\t" << "Y:" << particle->Vy()  << "\t" << "Z:" << particle->Vz() << "\t" << "T:" << particle->T() << endl;
			cout << "Final Dir: " << "X:" << particle->DirX()  << "\t" << "Y:" << particle->DirY()  << "\t" << "Z:" << particle->DirZ() << "\t" << "Mag:" << particle->Dir() << endl;
			cout << "Final Vel (m/s): " << "X:" << particle->VelocityX()  << "\t" << "Y:" << particle->VelocityY()  << "\t" << "Z:" << particle->VelocityZ() << "\t" << "V:" << particle->Velocity() << endl;
			cout << "Final Mom (eV): " << "X:" << particle->Px()  << "\t" << "Y:" << particle->Py()  << "\t" << "Z:" << particle->Pz() << "\t" << "P:" << particle->P() << endl;
			cout << "Final Volume: "    << this->GetCurrentVolume()->GetName() << endl;
			cout << "Final Energy (neV): "    << particle->Energy() /Units::neV << endl;
			cout << "-----------------------------" << endl;
			cout << "Is On Boundary?  " << this->IsUCNOnBoundary() << endl;
			cout << "Is Outside?  "     << this->IsUCNOutside() << endl;
			cout << "Is Step Entering?  " << this->IsUCNStepEntering()  << endl;
			cout << "Is Step Exiting?  " << this->IsUCNStepExiting() << endl;
			cout << "-----------------------------" << endl << endl;
		#endif	
		
		TUCNGeoMaterial* currentMaterial = static_cast<TUCNGeoMaterial*>(this->GetCurrentNode()->GetMedium()->GetMaterial());
		
		// -- Determine what to do if we are on a boundary
		// -- Is Track on the surface of a boundary?
		if (currentMaterial->IsTrackingMaterial() == kFALSE) {
			// -- Are we on the surface of a detector?
			if (currentMaterial->IsDetectorMaterial() == kTRUE) {
				// -- Was particle detected?
				Double_t prob = gRandom->Uniform(0.0,1.0);
				if (prob < currentMaterial->DetectionEfficiency()) {	
					particle->Detected(kTRUE);  // Set detected flag
				} else {	
				#ifdef VERBOSE_MODE	
					cout << "------------------- BOUNCE ----------------------" << endl;
					cout << "Current medium: " << this->GetCurrentNode()->GetMedium()->GetName() << endl;
				#endif
					// -- Make a Bounce
					this->Bounce(track);	
					// -- Cross Boundary and Locate
					this->CrossBoundaryAndLocate(kFALSE, this->GetCurrentNode()); 	
					this->UpdateTrack(track);
				#ifdef VERBOSE_MODE	
					cout << "-------------------------------------------------" << endl;
					cout << "Final medium: " << this->GetCurrentNode()->GetMedium()->GetName() << endl;
					cout << "-------------------------------------------------" << endl << endl;
				#endif
				}
			// -- Was particle lost to boundary (absorbed/upscattered) ?
			} else if (kFALSE) {	
				particle->Lost(kTRUE); // Set lost flag
			// -- Are we outside the geometry heirarchy we have built - ie: in TOP
			} else if (currentMaterial->IsBlackhole() == kTRUE) {
				particle->Lost(kTRUE);
			//	throw runtime_error("In TUCNGeoNavigator::PropagateTrack - track has entered TOP/Blackhole!");
			} else {
				#ifdef VERBOSE_MODE	
					cout << "------------------- BOUNCE ----------------------" << endl;
					cout << "Current Node: " << this->GetCurrentNode()->GetName() << endl;
				#endif	
				// -- Make a Bounce
				this->Bounce(track);
				// -- Cross Boundary and Locate
				if (this->IsUCNStepEntering() == kTRUE) {
					#ifdef VERBOSE_MODE	
						cout << "Search Upwards - Step entering a volume so we must step back out" << endl;
						cout << this->GetCurrentNode()->GetVolume()->GetNextNodeIndex() << endl;
					#endif
					this->CrossBoundaryAndLocate(kFALSE, this->GetCurrentNode()); 	
						
				} else {
					#ifdef VERBOSE_MODE
						cout << "Search Downwards - Step exited a volume so we must re-enter" << endl;
						cout << this->GetCurrentNode()->GetVolume()->GetNextNodeIndex() << endl;
					#endif
					this->CrossBoundaryAndLocate(kTRUE, this->GetCurrentNode());
				}
				this->UpdateTrack(track);
				#ifdef VERBOSE_MODE	
					cout << "-------------------------------------------------" << endl;
					cout << "Final Node: " << this->GetCurrentNode()->GetName() << endl;
					cout << "-------------------------------------------------" << endl << endl;
				#endif
			}
		}
		
		// -- Determine destination
		// -- Has lost flag been set?
		if (particle->Lost() == kTRUE) {
			break; // -- End Propagtion Loop
		// -- Has detected flag been set?
		} else if (particle->Detected() == kTRUE) {
			break; // -- End Propagation Loop
		// -- Has particle decayed within steptime?
		} else if (particle->WillDecay(this->GetStepTime()) == kTRUE) {
			particle->Decayed(kTRUE);// Set Decay Flag
			break; // -- End Propagation Loop
		}
	// -- END OF PROPAGATION LOOP
	}	
							
	cout << "FINAL STATUS: " << "Track: " << track->GetId() << "\t" << "Steps taken: " << stepNumber << "\t";
	cout << "Time: " << particle->T() << "s" << "\t" << "Final Medium: " << this->GetCurrentNode()->GetMedium()->GetName() << "\t";
	cout << "Bounces: " << particle->Bounces() << "\t" << "Diffuse: " << particle->DiffuseBounces() << "\t" << "Specular: " << particle->SpecularBounces() << endl;
	
	return kTRUE;
}

//_____________________________________________________________________________
void TUCNGeoNavigator::UpdateTrack(TVirtualGeoTrack* track, Double_t timeInterval)
{
// Take the particle and update its position, momentum, time and energy with the current properties stored in the Navigator
	TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
	
	#ifdef VERBOSE_MODE
		Info("UpdateTrack", "Initial X: %f, Y: %f, Z: %f, T: %f", particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
		Info("UpdateTrack", "Initial Px: %f, Py: %f, Pz: %f, E: %f", particle->Px(), particle->Py(), particle->Pz(), particle->Energy()/Units::neV);
	#endif
	
	const Double_t* pos = this->GetCurrentPoint();
	const Double_t* dir = this->GetCurrentDirection(); 
	const TUCNGravField* field = fUCNGeometry->GetGravField();
	
	Double_t heightClimbed = 0.0;
	Double_t gravPotentialEnergy = 0.0;
	
	if (field != NULL) {
		// Determine the height of our particle in the global coordinate system of TOP.
		// Take the dot product of the point vector with the field direction unit vector to get the height of this point in the gravitational field.
		// This assumes a convention that 'height' is measured along an axis that INCREASES in the opposite direction to the field direction vector
		// (which is usually 'downwards')
		heightClimbed = -1.0*((pos[0] - particle->Vx())*field->Nx() + (pos[1] - particle->Vy())*field->Ny() + (pos[2] - particle->Vz())*field->Nz());
		gravPotentialEnergy = particle->Mass_GeV_c2()*field->GravAcceleration()*heightClimbed;
	}
	
	// Determine current Kinetic energy of particle given the height climbed in graviational field
	Double_t kineticEnergy = particle->Energy() - gravPotentialEnergy;
	
	// Detemine current momentum
	Double_t momentum = TMath::Sqrt(2.0*particle->Mass_GeV()*kineticEnergy);
	Double_t mom[3] = {momentum*dir[0], momentum*dir[1], momentum*dir[2]};
	
	// Update particle
	particle->SetProductionVertex(pos[0], pos[1], pos[2], particle->T() + timeInterval);
	particle->SetMomentum(mom[0], mom[1], mom[2], kineticEnergy);

	// Update track
	track->AddPoint(particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
	
	#ifdef VERBOSE_MODE
		Info("UpdateTrack", "Height climbed: %f, Kinetic Energy Gained(Lost): %f", heightClimbed, -gravPotentialEnergy/Units::neV);
		Info("UpdateTrack", "Final X: %f, Y: %f, Z: %f, T: %f", particle->Vx(), particle->Vy(), particle->Vz(), particle->T());
		Info("UpdateTrack", "Final Px: %f, Py: %f, Pz: %f, E: %f", particle->Px(), particle->Py(), particle->Pz(), particle->Energy()/Units::neV);
	#endif
}


//_____________________________________________________________________________
Bool_t TUCNGeoNavigator::Bounce(TVirtualGeoTrack* track)
{
	// -- Get particle
	TUCNParticle* ucnparticle = static_cast<TUCNParticle*>(track->GetParticle());
	
	// -- Direction Vector
	Double_t dir[3] = {(this->GetCurrentDirection())[0], (this->GetCurrentDirection())[1], (this->GetCurrentDirection())[2]};
	
	// -- Normal Vector
	const Double_t *normal = this->FindUCNNormal();
	Double_t norm[3] = {normal[0], normal[1], normal[2]};
	
	// Check if the normal vector is actually pointing in the wrong direction 
	// (wrong means pointing along the direction of the track, rather than in the opposite direction)
	Double_t dotProduct = dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2];
	if (dotProduct > 0.) {
		// If so, reflect the normal to get the correct direction
		norm[0] = -norm[0];
		norm[1] = -norm[1];
		norm[2] = -norm[2];
	}
		
	// -- Calculate Probability of diffuse reflection
	Double_t diffuseProbability = ucnparticle->DiffuseProbability(); // Need to add some kind of law to determine probability
	
	// Determine Reflection Type 
	Double_t prob = gRandom->Uniform(0.0,1.0);
	if (prob <= diffuseProbability) {
		// -- Diffuse Bounce
		this->DiffuseBounce(dir, norm);
		ucnparticle->MadeDiffuseBounce(); // Update counter
	} else {
		// -- Specular Bounce
		this->SpecularBounce(dir, norm);
		ucnparticle->MadeSpecularBounce(); // Update counter
	}

	// -- Set New Direction
	this->SetCurrentDirection(dir);
	
	// -- Update Track
	this->UpdateTrack(track);	

	// -- Update Bounce Counter
	ucnparticle->MadeBounce();
	
	return kTRUE;
}	

//_____________________________________________________________________________
Bool_t TUCNGeoNavigator::SpecularBounce(Double_t* dir, const Double_t* norm)
{
	#ifdef VERBOSE_MODE	
		cout << "----------------------------" << endl;
		cout << "Specular Bounce" << endl;
		cout << "BEFORE - nx: " << dir[0] << "\t" << "ny: " << dir[1] << "\t" << "ny: " << dir[2] << endl;
		cout << "normx: " << norm[0] << "\t" << "normy: " << norm[1] << "\t" << "normz: " << norm[2] << endl;
	#endif
	
	Double_t dotProduct = dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2];	
	// Reflection Law for Specular Reflection
	dir[0] = dir[0] - 2.0*dotProduct*norm[0];
	dir[1] = dir[1] - 2.0*dotProduct*norm[1];  
	dir[2] = dir[2] - 2.0*dotProduct*norm[2];  
	
	#ifdef VERBOSE_MODE	
		cout << "AFTER - nx: " << dir[0] << "\t" << "ny: " << dir[1] << "\t" << "nz: " << dir[2] << endl;
	#endif	  

	return kTRUE;
}

//_____________________________________________________________________________
Bool_t TUCNGeoNavigator::DiffuseBounce(Double_t* dir, const Double_t* norm)
{
	#ifdef VERBOSE_MODE	
		cout << "----------------------------" << endl;
		cout << "Diffuse Bounce" << endl;
		cout << "BEFORE - nx: " << dir[0] << "\t" << "nx: " << dir[1] << "\t" << "nx: " << dir[2] << endl;
		cout << "normx: " << norm[0] << "\t" << "normx: " << norm[1] << "\t" << "normx: " << norm[2] << endl;
	#endif	
	
	// First we need to pick random angles to choose the orientation of our diffuse direction vector. 
	// Correct method for UCN physics though is to weight these angles towards the poles by adding an extra cos(theta) - derivation of how to pick these angles is in notes
	Double_t phi = gRandom->Uniform(0.0, 1.0)*2*TMath::Pi();
	Double_t u = gRandom->Uniform(0.0, 0.5);		
	Double_t theta = TMath::ACos(TMath::Sqrt(1.0 - 2*u)); // We ignore the negative sqrt term, since we are only interested in theta between 0 and pi/2 
	
	// Calculate local normal vector	
	Double_t lnorm[3] = {0.,0.,0.};
	this->MasterToLocalVect(norm, lnorm);
	TVector3 localNorm(lnorm[0], lnorm[1], lnorm[2]);
	assert(TMath::Abs(localNorm.Mag() - 1.0) < fgTolerance);
		
	// Define a preferred direction in our coordinate system - usually the z-direction - that we want to be perpendicular to the normal vector of our surface
	TVector3 upAxis(0.,0.,1.);
		
	// Here we check to make sure that the upAxis we chose is not parallel to the normal vector. If it is, we try another one, x. 
	if (TMath::Abs(upAxis.Dot(localNorm)) > fgTolerance) {
		upAxis.SetXYZ(1.,0.,0.);
		if (TMath::Abs(upAxis.Dot(localNorm)) > fgTolerance) {
			throw runtime_error("In TUCNGeoNavigator::DiffuseBounce - Could not find an axis perpendicular to normal. Normal is parallel to z and x axes!!!");
		}
	}
	
	// Calculate the cross product of the 'up' vector with our normal vector to get a vector guaranteed to be perpendicular to the normal. 
	// This is the vector we will want to rotate around initially. 
	TVector3 perpAxis = localNorm.Cross(upAxis); 
	assert(TMath::Abs(perpAxis.Mag() - 1.0) < fgTolerance);
	
	
	// Rotate normal vector about perpendicular axis by angle theta. Using Rodrigues' formula derived in notes. 
	TVector3 rotatedNorm(0.,0.,0.);
	{
		TVector3 term2 = localNorm - (localNorm.Dot(perpAxis))*perpAxis; // name term2 and term3 just refer to the terms as they appear in the Rodrigues' formula
		TVector3 term3 = perpAxis.Cross(localNorm);
		rotatedNorm = localNorm + (TMath::Cos(theta) - 1.0)*term2 + TMath::Sin(theta)*term3;
	}
	assert(TMath::Abs(rotatedNorm.Mag() - 1.0) < fgTolerance);
	
	// Rotate the newly rotated Normal vector, rotatedNorm, by angle phi, this time rotating about the original normal vector, norm.
	TVector3 direction(0.,0.,0.);
	{
		TVector3 term3 = localNorm.Cross(rotatedNorm);
		TVector3 term2 = rotatedNorm - (rotatedNorm.Dot(localNorm))*localNorm;
		direction = rotatedNorm + (TMath::Cos(phi) - 1.0)*term2 + TMath::Sin(phi)*term3;
	}
	assert(TMath::Abs(direction.Mag() - 1.0) < fgTolerance); 
	
	// Convert the direction vector back to the global frame
	Double_t ldir[3] = {direction.X(), direction.Y(), direction.Z()};
	this->LocalToMasterVect(ldir, dir);
	
	assert(TMath::Abs(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2] - 1.0) < fgTolerance);
	// Assert that our final direction is not perpendicular to the normal. This could result in escaping the boundary
	assert(TMath::Abs(dir[0]*norm[0] + dir[1]*norm[1] + dir[2]*norm[2]) > fgTolerance); 
	
	#ifdef VERBOSE_MODE	
		cout << "AFTER - nx: " << dir[0] << "\t" << "nx: " << dir[1] << "\t" << "nx: " << dir[2] << endl;
	#endif
	
	return kTRUE;
}
