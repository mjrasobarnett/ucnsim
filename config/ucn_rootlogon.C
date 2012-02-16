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
   set_my_style();
} 
