{
	cout << "rootlogon.C for UCNSIM" << endl;
	cout << "-------------------------------------" << endl;
  	if ( gSystem->Load("libPhysics") == 0 ) {
    cout << "Successfully loaded libPhysics.so" << endl;
  	} 
	if ( gSystem->Load("libGeom.so") == 0 ) {
    cout << "Successfully loaded libGeom.so" << endl;
  	} 
  	if ( gSystem->Load("libGeomPainter.so") == 0 ) {
    cout << "Successfully loaded libGeomPainter.so" << endl;
  	} 
 	if ( gSystem->Load("libTree.so") == 0 ) {
	    cout << "Successfully loaded libTree.so" << endl;
	}
 	if ( gSystem->Load("libEG.so") == 0 ) {
	    cout << "Successfully loaded libEG.so" << endl;
	}
	if ( gSystem->Load("libMathCore.so") == 0 ) {
	    cout << "Successfully loaded libMathCore.so" << endl;
	}
	if ( gSystem->Load("libMathMore.so") == 0 ) {
	    cout << "Successfully loaded libMathCore.so" << endl;
	}
	TString ucnsim = gSystem->Getenv("UCN_DIR");
	if ( ucnsim.Length() == 0 ) {
		cerr << "-------------------------------------" << endl;
		cerr << "Warning: Failed to find env. variable UCNSIM" << endl;
		cerr << "-------------------------------------" << endl;
	}
	else {
		TString ucnlib = ucnsim + "/lib/libUCN.so";
		cerr << ucnlib.Data() << endl;
		if ( gSystem->Load(ucnlib.Data()) == 0 ) {
		   cout << "Successfully loaded libUCN.so" << endl;
  	   }
	}
   
   TString magdir = gSystem->Getenv("MAGDIR");
	if ( magdir.Length() == 0 ) {
		cerr << "-------------------------------------" << endl;
		cerr << "Warning: Failed to find env. variable MAGDIR" << endl;
		cerr << "-------------------------------------" << endl;
	}
	else {
		TString maglib = magdir + "/lib/libEDM.so";
		cerr << maglib.Data() << endl;
		if ( gSystem->Load(maglib.Data()) == 0 ) {
		   cout << "Successfully loaded libEDM.so" << endl;
  	   }
   }
	cout << "-------------------------------------" << endl;
	set_my_style();
} 
