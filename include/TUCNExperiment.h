// @(#)root/geom:$Id: $
// Author: Matthew Raso-Barnett  February 2009

#ifndef TUCNExperiment_H
#define TUCNExperiment_H

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TUCNExperiment -         										 					  //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
class TUCNGeoManager;

class TUCNExperiment
{
	private: 
		Int_t fIndexBlackhole;
		Int_t fIndexTracking;
		Int_t fIndexDetector;
		Int_t fIndexWall;
		
		TUCNGeoManager* fUCNGeometry;
	
	protected:
		
	public: 
		// constructor 
		TUCNExperiment();		
		TUCNExperiment(TUCNGeoManager* geom);
		
		// destructor
		virtual ~TUCNExperiment();
			
		// methods	
		TUCNGeoManager* 	Geometry() const {return fUCNGeometry;}
			
		void 					MakeTestVolume();
		void 					MakeCuboid(Double_t dx, Double_t dy, Double_t dz);

		void 					MakeBox(Double_t dx, Double_t dy, Double_t dz);
		void 					MakeTube(Double_t length, Double_t innerRadius, Double_t outerRadius);
		void 					MakeVerticalTube(Double_t length, Double_t innerRadius, Double_t outerRadius);
		void 					MakeTorus(Double_t R,Double_t Rmin,Double_t Rmax,Double_t Phi1,Double_t Dphi);
		
		void 					CloseGeometry();
		void 					DrawGeometry();		
						
	ClassDef(TUCNExperiment,1)
		
};

#endif
