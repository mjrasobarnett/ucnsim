void set_my_style(void) 
{ }

void load_library(TString name)
{
	if ( gSystem->Load(name) == 0 ) {
    cout << "Successfully loaded library:" << name << endl;
  }
	else if ( gSystem->Load(name) > 0 ) {
		cout << "Library " << name << " has already been loaded" << endl;
	}
	else if ( gSystem->Load(name) < 0 ) {
		cout << "Failed to load library:" << name << endl;
	}
}