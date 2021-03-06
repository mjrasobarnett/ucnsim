#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// ----------------------------------------------------------------------
// -- Classes
#pragma link C++ class Run+;
#pragma link C++ class Experiment+;
#pragma link C++ class FieldManager+;
#pragma link C++ class Box+;
#pragma link C++ class Tube+;
#pragma link C++ class Element+;
#pragma link C++ class Material+;
#pragma link C++ class Volume+;
#pragma link C++ class TrackingVolume+;
#pragma link C++ class Boundary+;
#pragma link C++ class Detector+;
#pragma link C++ class BlackHole+;
#pragma link C++ class Particle+;
#pragma link C++ class Observable+;
#pragma link C++ class Spin+;
#pragma link C++ class Spinor+;
#pragma link C++ class State+;
#pragma link C++ class Propagating+;
#pragma link C++ class Decayed+;
#pragma link C++ class Absorbed+;
#pragma link C++ class Detected+;
#pragma link C++ class Anomalous+;
#pragma link C++ class Lost+;
#pragma link C++ class GravField+;
#pragma link C++ class FileParser+;
#pragma link C++ class Parabola+;
#pragma link C++ class Polynomial+;
#pragma link C++ class Data+;
#pragma link C++ class Field+;
#pragma link C++ class MagField+;
#pragma link C++ class UniformMagField+;
#pragma link C++ class ConfigFile+;
#pragma link C++ class CompositeShape+;
#pragma link C++ class BoolNode+;
#pragma link C++ class Union+;
#pragma link C++ class Subtraction+;
#pragma link C++ class Intersection+;
#pragma link C++ class FieldArray+;
#pragma link C++ class MagFieldArray+;
#pragma link C++ class ElecField+;
#pragma link C++ class InitialConfig+;
#pragma link C++ class RunConfig+;
#pragma link C++ class Observer+;
#pragma link C++ class SpinObserver+;
#pragma link C++ class SpinData+;
#pragma link C++ class BounceObserver+;
#pragma link C++ class BounceData+;
#pragma link C++ class TrackObserver+;
#pragma link C++ class Track+;
#pragma link C++ class Point+;
#pragma link C++ class FieldMap+;
#pragma link C++ class MagFieldMap+;
#pragma link C++ class KDTree+;
#pragma link C++ class KDTreeNode+;
#pragma link C++ class FieldVertex+;
#pragma link C++ class VertexStack+;
#pragma link C++ class FieldObserver+;
#pragma link C++ class FieldData+;
#pragma link C++ class UniformElecField+;
#pragma link C++ class ElecFieldArray+;
#pragma link C++ class ParticleManifest+;
#pragma link C++ class Listing+;
#pragma link C++ class TRandom3a;
#pragma link C++ class TRandom3State+;
#pragma link C++ class PopulationObserver+;
#pragma link C++ class PopulationData+;
#pragma link C++ class MagFieldDipole+;
#pragma link C++ class MagFieldLoop+;

// ----------------------------------------------------------------------
// -- Elements and Materials Namespaces are used for defining common material properties
#pragma link C++ namespace Elements;
#pragma link C++ namespace Materials;
#pragma link C++ function Materials::BuildMaterials(TGeoManager*);

// ----------------------------------------------------------------------
// -- Algorithms namespace contains a number of useful functions relevant project-wide
#pragma link C++ namespace Algorithms;

#pragma link C++ namespace Algorithms::Precision;
#pragma link C++ function Algorithms::Precision::IsEqual(double,double,double);
#pragma link C++ function Algorithms::Precision::IsNotEqual(double,double,double);
#pragma link C++ function Algorithms::Precision::IsGreaterOrEqual(double,double,double);
#pragma link C++ function Algorithms::Precision::IsLessOrEqual(double,double,double);

#pragma link C++ namespace Algorithms::String;
#pragma link C++ function Algorithms::String::ConvertToInt(const std::string, int&);
#pragma link C++ function Algorithms::String::ConvertToBool(const std::string, bool&);
#pragma link C++ function Algorithms::String::ConvertToDouble(const std::string, double&);
#pragma link C++ function Algorithms::String::ConvertVectorToInt(const std::vector<std::string>&, std::vector<int>&);
#pragma link C++ function Algorithms::String::ConvertVectorToDouble(const std::vector<std::string>&, std::vector<double>&);
#pragma link C++ function Algorithms::String::FactorString(const std::string, const char);

#pragma link C++ namespace Algorithms::FileSystem;
#pragma link C++ function Algorithms::FileSystem::ExpandShellVar(const std::string);
#pragma link C++ function Algorithms::FileSystem::ExpandFilePath(const std::string);

#pragma link C++ namespace Algorithms::ProgressBar;
#pragma link C++ function Algorithms::ProgressBar::PrintProgress(int, float, int);

// ----------------------------------------------------------------------
// -- The Analysis namespace (and its sub-namespaces) contains functions relevant for Data-Analysis-type tasks
#pragma link C++ namespace Analysis;

#pragma link C++ namespace Analysis::DataFile;
#pragma link C++ function Analysis::DataFile::OpenRootFile(std::string,std::string);
#pragma link C++ function Analysis::DataFile::LoadRunConfig(TFile&);
#pragma link C++ function Analysis::DataFile::LoadGeometry(TFile&);
#pragma link C++ function Analysis::DataFile::LoadParticleManifest(TFile&);
#pragma link C++ function Analysis::DataFile::LoadParticleDataTree(TFile&);
#pragma link C++ function Analysis::DataFile::IsRootFile(const std::string);
#pragma link C++ function Analysis::DataFile::IsValidStateName(const std::vector<std::string>&);
#pragma link C++ function Analysis::DataFile::IsValidStateName(const std::string);
#pragma link C++ function Analysis::DataFile::ConcatenateStateNames(std::vector<std::string>&);
#pragma link C++ function Analysis::DataFile::NavigateToHistDir(TFile&);
#pragma link C++ function Analysis::DataFile::CopyDirectory(TDirectory* const, TDirectory* const);
#pragma link C++ function Analysis::DataFile::CopyDirectoryContents(TDirectory* const, TDirectory* const);
#pragma link C++ function Analysis::DataFile::GetParticleBranch(const std::string&, TTree*);

#pragma link C++ namespace Analysis::FitFunctions;
#pragma link C++ function Analysis::FitFunctions::SpinPrecession(double*, double*);
#pragma link C++ function Analysis::FitFunctions::ExponentialDecay(double*, double*);
#pragma link C++ function Analysis::FitFunctions::MaxwellBoltzmann(double*, double*);
#pragma link C++ function Analysis::FitFunctions::DoubleExponential(double*, double*);

#pragma link C++ namespace Analysis::Polarisation;
#pragma link C++ struct Analysis::Polarisation::Coords;
#pragma link C++ function Analysis::Polarisation::PlotSpinPolarisation(const std::string, const std::vector<int>, TTree*, const RunConfig&);
#pragma link C++ function Analysis::Polarisation::PlotField(const std::string, const std::vector<int>, TTree*, const RunConfig&);
#pragma link C++ function Analysis::Polarisation::CalculateT2(TFile&, std::vector<std::string>, double&, double&);
#pragma link C++ function Analysis::Polarisation::CreateT2AlphaGraph(std::vector<TDirectory*>, double, unsigned int);
#pragma link C++ function Analysis::Polarisation::PlotPhaseAngleSnapShots(std::vector<std::vector<Analysis::Polarisation::Coords> >& , const unsigned int );
#pragma link C++ function Analysis::Polarisation::PlotT2_vs_Runs(const std::string, const std::string);
#pragma link C++ function Analysis::Polarisation::CalculateMeanPhase(std::vector<std::vector<Analysis::Polarisation::Coords> >& , const unsigned int);
#pragma link C++ function Analysis::Polarisation::CalculateAlpha(std::vector<std::vector<Analysis::Polarisation::Coords> >& , const unsigned int , const double );

#pragma link C++ namespace Analysis::Bounces;
#pragma link C++ function Analysis::Bounces::PlotBounceCounters(const std::string, const std::vector<int>, TTree*);

#pragma link C++ namespace Analysis::FinalStates;
#pragma link C++ function Analysis::FinalStates::PlotFinalState(const std::string, const std::vector<int>, TTree*, const RunConfig&);
#pragma link C++ function Analysis::FinalStates::DrawFinalPositions(const std::string, const std::vector<int>, TTree*, TGeoManager&, double*);
#pragma link C++ function Analysis::FinalStates::PlotEmptyingTime(const std::string, const std::vector<int>, TTree*, const RunConfig&, const int, const double, const double, TF1*);
#pragma link C++ function Analysis::FinalStates::PlotFinalTime(const std::string, const std::vector<int>, TTree*, TH1F*);


#pragma link C++ namespace Analysis::Tracks;
#pragma link C++ function Analysis::Tracks::PlotParticleHistories(TDirectory* const, const std::vector<TDirectory*>, TGeoManager&);
#pragma link C++ function Analysis::Tracks::CalculateParticleHistory(const Track&, TGeoManager&);

#pragma link C++ namespace Analysis::Geometry;
#pragma link C++ function Analysis::Geometry::DrawGeometry(TCanvas&, TGeoManager&, double*);
#pragma link C++ function Analysis::Geometry::GlobalToLocalPoint(const Point&, Point&, const TGeoMatrix&);
#pragma link C++ function Analysis::Geometry::LocalToGlobalPoint(const Point&, Point&, const TGeoMatrix&);
// ----------------------------------------------------------------------

#endif