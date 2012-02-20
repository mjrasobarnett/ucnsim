{
/*   cout <<"==================================================="<< endl;
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
   if ( gSystem->Load("libRGL.so") == 0 ) {
       cout << "Successfully loaded libMathCore.so" << endl;
   }
   
   TString ucnsim = gSystem->Getenv("UCN_DIR");
   if ( ucnsim.Length() == 0 ) {
      cerr << "Error: Failed to find env. variable UCN_DIR" << endl;
   } else {
      TString ucnlib = ucnsim + "/lib/libUCN.so";
      load_library(ucnlib);
   }
*/   
  
  // Add project's src directory to CINT's include path so that it knows where
  // our headers are located when including them in scripts 
  gInterpreter->AddIncludePath("${UCN_DIR}/src");

  // My custom style setup defined in $ROOTALIAS
  set_my_style();
} 
