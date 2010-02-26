// UCN Particle Cloud class 
// Author: Matthew Raso-Barnett  03/06/2009


#ifndef TUCNPARTICLECLOUD_H
#define TUCNPARTICLECLOUD_H


////////////////////////////////////////////////////////////////////////////
//                                                                        //
// 						TUCNParticleCloud 											  //
//                   																	  //
////////////////////////////////////////////////////////////////////////////
class TObjArray;
class TUCNGeoManager;
class TUCNParticle;


class TUCNParticleCloud : public TObject
{
protected:
	TUCNParticleCloud(const TUCNParticleCloud&); 
   TUCNParticleCloud& operator=(const TUCNParticleCloud&);

private:
	TObjArray* 			fParticles;
	TUCNGeoManager* 	fUCNGeometry;
			
public:
	// -- constructors
   TUCNParticleCloud();
	TUCNParticleCloud(TUCNGeoManager* geom);

   // -- destructor
	virtual ~TUCNParticleCloud();

	// -- methods
	TObjArray*				UniformMonoEnergeticDistribution(Int_t particles, Double_t totalEnergy, TGeoVolume* volume, TGeoMatrix* matrix=0);
	TObjArray*				GetListOfParticles() const 		{return fParticles;}
	
	TUCNGeoManager* 		Geometry() const 	 {return fUCNGeometry;}
	TUCNParticle*			GetParticle(Int_t id) const; 
	
	
   ClassDef(TUCNParticleCloud,1)   // 
};

#endif
