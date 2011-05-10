#include <iostream>
#include <string>

using namespace std;

//__________________________________________________________________________
Int_t plot_T2_vs_Runs(string configFileName, string statename)
{
   return Analysis::Polarisation::PlotT2_vs_Runs(configFileName, statename);
}
