	TBenchmark benchmark;
	benchmark.Start("UCN Simulation");
	
	// _________________________________________________________________________________________
	// This program creates a tube nested inside a rotated box, and propagates a particle (or particles)
	// around it for a set period of time. THe aim is to test the methods related to reflecting off of the inside
	// and outside of the tube.  
	// _________________________________________________________________________________________ 
	
	// ----------------------------------------------					
	// -- Create a UCNGeoManager
	TUCNGeoManager* myManager = new TUCNGeoManager("TestUCNGeoManager", "Derived class from TGeoManager");
			
	// -- Making Materials
   TUCNGeoMaterial* matTracking  = new TUCNGeoMaterial("Tracking Material", 0,0,0);
	TUCNGeoMaterial* matBlackHole = new TUCNGeoMaterial("BlackHole", 0,0,0);
	TUCNGeoMaterial* matBoundary  = new TUCNGeoMaterial("Boundary Material", 0,0,0);
   
	matTracking->IsTrackingMaterial(kTRUE);
	matBlackHole->IsBlackhole(kTRUE);
	
	// -- Making Mediums
	TGeoMedium* vacuum = new TGeoMedium("Vacuum",1, matTracking);
	TGeoMedium* blackHole = new TGeoMedium("BlackHole",2, matBlackHole);
	TGeoMedium* boundary = new TGeoMedium("Boundary",3, matBoundary);
	
	// -- Making Top Volume
 	TGeoVolume* chamber = myManager->MakeBox("TOP",blackHole,20,20,20);
	myManager->SetTopVolume(chamber);
			
	// -- Make a UCNBox
	Double_t boxX = 0.5, boxY = 0.5, boxZ = 0.5; 
	TGeoVolume* box   = myManager->MakeBox("box",blackHole, boxX, boxY, boxZ);
	TGeoVolume* innerBox  = myManager->MakeBox("innerbox",blackHole, boxX-0.01, boxY-0.01, boxZ-0.01);
	
	// -- Make a UCNTube 
	Double_t rMin = 0.0, rMax = 0.11, length = 0.4; 
	TGeoVolume* tube   = myManager->MakeTube("tube",boundary, rMin, rMax, length);
	TGeoVolume* innerTube  = myManager->MakeTube("innerTube",vacuum, rMin, rMax-0.01, length-0.01);
	
	tube->SetLineColor(36);
	tube->SetLineWidth(1);
	tube->SetVisibility(kTRUE);
	tube->SetTransparency(50);
	
	innerTube->SetLineColor(8);
	innerTube->SetLineWidth(1);
	innerTube->SetVisibility(kTRUE);
	innerTube->SetTransparency(80);
	
	box->SetVisibility(kFALSE);
	innerBox->SetVisibility(kFALSE);
	
	// -- Define the transformation of the volume
	// Here is a general set-up of rotations and translations to be applied to a single volume. 
	TGeoRotation r1,r2; 
	r1.SetAngles(0,30,0);          	//	rotation defined by Euler angles 
	r2.SetAngles(0,0,0); 	 
	TGeoTranslation t1(0.,0.,0.); 
	TGeoTranslation t2(0.,0.,0.); 
	TGeoCombiTrans c1(t1,r1); 
	TGeoCombiTrans c2(t2,r2); 
	TGeoHMatrix hm = c1 * c2;        // composition is done via TGeoHMatrix class 
	TGeoHMatrix *matrix = new TGeoHMatrix(hm);
	
	// -- Create the nodes	
	tube->AddNode(innerTube,1);
	innerBox->AddNode(tube,1);
	box->AddNode(innerBox,1);
	chamber->AddNode(box,1, matrix);
		
	// -- Arrange and close geometry
   myManager->CloseGeometry();
	
	myManager->GetUCNNavigator()->BuildCache(kTRUE, kTRUE);
	myManager->GetUCNNavigator()->ResetAll();
	
	// -- Make a Grav Field
	Double_t fieldDir[3] = {0., 0., -1.};
	myManager->AddGravField(fieldDir[0], fieldDir[1], fieldDir[2]);
	// ----------------------------------------------					
	
	
	// ----------------------------------------------					
	// -- Create Particle Cloud
	Int_t particles = 2854;
	Double_t totalEnergy = 100*Units::neV;
	TUCNParticleCloud* particleCloud = myManager->AddParticleCloud();
	particleCloud->UniformMonoEnergeticDistribution(particles, totalEnergy, innerTube, matrix);
		
	// -- Create Tracks
	myManager->MakeTracks(myManager->GetParticleCloud());


	Double_t quarticRoots[4] = {0., 0., 0., 0.};
//	Double_t quarticParams[5] = {1,0.06003195664,3.572599006,0.1070842411,-2.001009431e-06};
	Double_t quarticParams[5] = {6.010649013906247,0.3608310209949935,21.47363869519429,0.643645788281406,-1.202736536178722e-05};
	
	// {1., 2., -41., -42., 360.};
	
	Int_t quarticSolutions = TUCNPolynomial::Instance()->QuarticRootFinder(quarticParams, quarticRoots);
	
	cout << "Solutions: " << quarticSolutions << endl;
	cout << "Roots: " << quarticRoots[0] << "\t" << quarticRoots[1] << "\t" << quarticRoots[2] << "\t" << quarticRoots[3] <<  endl << endl;





//	TVirtualGeoTrack* myTrack = myManager->GetTrack(16);
//	TUCNParticle* myParticle = static_cast<TUCNParticle*>(myTrack->GetParticle());
	
//	Double_t pos[3] = {myParticle->Vx(), myParticle->Vy(), myParticle->Vz()};
//	Double_t mom[3] = {myParticle->DirX(), myParticle->DirY(), myParticle->DirZ()};
//	Double_t kineticEnergy = myParticle->Energy();
//	Double_t startTime = 0;
	
/*	for (Int_t id = 2853; id < particles; id++) {
		// Get a particular Track
//		Int_t id = 16;
		TVirtualGeoTrack* track = myManager->GetTrack(id);
		TUCNParticle* particle = static_cast<TUCNParticle*>(track->GetParticle());
//		cout << "X: " << particle->Vx() << "\t" << "Y: " << particle->Vy() << "\t" << "Z: " << particle->Vz() << "\t" << "T: " << particle->T() << endl;
//		cout << "dX: " << particle->DirX() << "\t" << "dY: " << particle->DirY() << "\t" << "dZ: " << particle->DirZ() << "\t" << "E: " << particle->Energy()/Units::neV << endl;
		
		// Initialise track - Sets navigator's current point/direction/node to that of the particle
		myManager->SetCurrentTrack(track);
		myManager->InitTrack(particle->Vx(), particle->Vy(), particle->Vz(), particle->Px()/particle->P(), particle->Py()/particle->P(), particle->Pz()/particle->P());
		// Propagate track
		Double_t maxStepTime = 0.01*Units::s;
//		Int_t steps = 39875;
		Int_t steps = 39861;
		
//		cout << "my track: " << track->GetId() << "\t" << "current track: " << myManager->GetCurrentTrack()->GetId() << endl;
//		cout << "X: " << myManager->GetCurrentPoint()[0] << "\t" << "Y: " << myManager->GetCurrentPoint()[1] << "\t" << "Z: " << myManager->GetCurrentPoint()[2] << endl;
//		cout << "nX: " << myManager->GetCurrentDirection()[0] << "\t" << "nY: " << myManager->GetCurrentDirection()[1] << "\t" << "nZ: " << myManager->GetCurrentDirection()[2] << endl;
		
		myManager->GetUCNNavigator()->PropagateTrack(track, steps, maxStepTime);
//		track->ResetTrack();
	}
	
	cout << "NUMBER OF PARTICLES: " << particles << endl;
	cout << "NUMBER LOST: " << myManager->GetUCNNavigator()->GetNumberLost() << endl;
	cout << "NUMBER DETECTED: " << myManager->GetUCNNavigator()->GetNumberDetected() << endl;
*/	
	
	// Make track and add to list	
	 
//	TUCNParticle* testParticle = new TUCNParticle(pos, mom, kineticEnergy, startTime);
//	TVirtualGeoTrack* testTrack = myManager->MakeTrack(101, testParticle->GetPdgCode(), testParticle);
//	myManager->AddTrack(testTrack);
//	myManager->SetCurrentTrack(testTrack);
//	testTrack->AddPoint(testParticle->Vx(), testParticle->Vy(), testParticle->Vz(), testParticle->T());	
//	myManager->InitTrack(testParticle->Vx(), testParticle->Vy(), testParticle->Vz(), testParticle->Px()/testParticle->P(), testParticle->Py()/testParticle->P(), testParticle->Pz()/testParticle->P());
//	
//	Double_t maxStepTime = 0.01*Units::s;
//	Int_t steps = 10;
//	cout << "X: " << myManager->GetCurrentPoint()[0] << "\t" << "Y: " << myManager->GetCurrentPoint()[1] << "\t" << "Z: " << myManager->GetCurrentPoint()[2] << endl;
//	cout << "nX: " << myManager->GetCurrentDirection()[0] << "\t" << "nY: " << myManager->GetCurrentDirection()[1] << "\t" << "nZ: " << myManager->GetCurrentDirection()[2] << endl;
//	myManager->GetUCNNavigator()->PropagateTrack(testTrack, steps, maxStepTime);
	
	

//	cout << "Particle 16" << endl;
//	cout << "X: " << pos[0] << "\t" << "Y: " << pos[1] << "\t" << "Z: " << pos[2] << endl;
//	cout << "dX: " << dir[0] << "\t" << "dY: " << dir[1] << "\t" << "dZ: " << dir[2] << endl;
		
	// -- Propagate Tracks
//	Double_t maxStepTime = 0.01*Units::s;

//	Int_t steps = 10000;
//	myManager->PropagateTracks(steps, maxStepTime, kFALSE);
	
//	Double_t runtime = 1.*Units::s;
//	myManager->PropagateTracks(runtime, maxStepTime, kFALSE, kTRUE);
	// ----------------------------------------------					
	
	
	// -- Testing method IsNextPointOnTube
//	TUCNParticle* myParticle = particleCloud->GetParticle(0);
//	myParticle->Print();
//	Double_t point[3] = {0., 0.1, 0.25};
//	Double_t velocity[3] = {myParticle->VelocityX(), myParticle->VelocityY(), myParticle->VelocityZ()};
//	Double_t field[3] = {myManager->GetGravField()->Gx(), myManager->GetGravField()->Gy(), myManager->GetGravField()->Gz()};
//	TUCNGeoTube* myTube = static_cast<TUCNGeoTube*>(innerTube->GetShape());
//	cout <<  myTube->IsNextPointOnTube(point, velocity, field, myTube->GetRmax(), myTube->GetDz(), 0.) << endl;
	
	
	
	benchmark.Stop("UCN Simulation");
	benchmark.Show("UCN Simulation");
			
	// ----------------------------------------------					
	// -- Draw Canvas
	Int_t width = 600;
	Int_t height = 600;
	Int_t positionX = 200;
	Int_t positionY = 10;
	TCanvas * canvas = new TCanvas("canvas", "UCNSIM: Final Particle Positions", positionX, positionY, width, height);
	canvas->Draw();
	
	// -- Draw Volumes
	myManager->GetTopVolume()->Draw();
	myManager->SetVisLevel(4);
	myManager->SetVisOption(0);
	
	// -- Draw Final Points
	TPolyMarker3D FinalPoints(particles, 1);
//	for (Int_t i = 0; i < particleCloud->GetListOfParticles()->GetEntriesFast(); i++) {
//		FinalPoints.SetPoint(i, particleCloud->GetParticle(i)->Vx(), particleCloud->GetParticle(i)->Vy(), particleCloud->GetParticle(i)->Vz());
//	}
	
	FinalPoints.SetPoint(0, particleCloud->GetParticle(2853)->Vx(), particleCloud->GetParticle(2853)->Vy(), particleCloud->GetParticle(2853)->Vz());
	
	FinalPoints.SetMarkerColor(2);
	FinalPoints.SetMarkerStyle(7);
	FinalPoints.Draw();
	
	// -- Draw Tracks
	// This will only work if the option to store tracks in the method PropagateTracks is turned on. 
	myManager->DrawTracks();
	
//	myManager->GetTrack(101)->Draw();
	// ----------------------------------------------					
	
