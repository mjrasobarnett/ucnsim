#include <iostream>
#include <vector>

#include "TSystem.h"
#include "TGeoMatrix.h"

//#include <Polynomial.h>

using namespace std;
//using namespace ROOT::Math;

Int_t mathmoretest() {
	
	gSystem->Load("libUCN");
	
	cout.precision(16);
	Double_t quarticRoots[4] = {0., 0., 0., 0.};
	Double_t quarticParams[5] = {24.042596055625, 5.210702173161462, 20.71514035087484, 2.214387241437159, -0.08291419168491501};
	
	// {1., 2., -41., -42., 360.};
	
	Int_t quarticSolutions = TUCNPolynomial::Instance()->QuarticRootFinder(quarticParams, quarticRoots);
	
	cout << "Solutions: " << quarticSolutions << endl;
	cout << "Roots: " << quarticRoots[0] << "\t" << quarticRoots[1] << "\t" << quarticRoots[2] << "\t" << quarticRoots[3] <<  endl << endl;
	
	Double_t point[3] = {-0.1760561782690933, -0.2, -0.3419575602668274};
	Double_t vel[3] = {4.132717757505896,2.91143196293098,-0.531343748697207};
	Double_t field[3] = {0,0,-9.80665};
	Double_t t = quarticRoots[0];
	
	cout << "Before - X:" << point[0] << "\t" << "Y:" << point[1] << "\t" << "Z:" << point[2] << endl;
	cout << "Sqrt(X^2 + Y^2): " << TMath::Sqrt(point[0]*point[0] + point[1]*point[1]) << endl;
	
	for (Int_t i = 0; i < 3; i++) point[i] += vel[i]*t + 0.5*field[i]*t*t; 
	
	cout << "After - X:" << point[0] << "\t" << "Y:" << point[1] << "\t" << "Z:" << point[2] << endl;
	cout << "Sqrt(X^2 + Y^2): " << TMath::Sqrt(point[0]*point[0] + point[1]*point[1]) << endl;
	
	TGeoRotation r1;
	r1.SetAngles(0,90,0);
	TGeoTranslation t1(-0.4,-0.1,0.);
	TGeoCombiTrans c1(t1,r1);
	TGeoHMatrix bendm = c1;
	bendm.Print();
	
	Double_t lpoint[3] = {0,0,0};
	bendm.MasterToLocal(point,&lpoint[0]);
	
	cout << "After - X:" << lpoint[0] << "\t" << "Y:" << lpoint[1] << "\t" << "Z:" << lpoint[2] << endl;
	cout << "Sqrt(X^2 + Y^2): " << TMath::Sqrt(lpoint[0]*lpoint[0] + lpoint[1]*lpoint[1]) << endl;
	
	Double_t a3 = 0.2167279340844047;
	Double_t a2 = 0.8616016466336769;
	Double_t cubicSolution = 0.849858897380718;
	cout << "(0.25*a3*a3) - a2: " << (0.25*a3*a3) - a2 << endl;
	cout << "(0.25*a3*a3) - a2 + cubicSolution: " << (0.25*a3*a3) - a2 + cubicSolution << endl;
	
	ROOT::Math::Polynomial quartic(24.042596055625, 5.210702173161462, 20.71514035087484, 2.214387241437159, -0.08291419168491501);
	vector<Double_t> roots = quartic.FindRealRoots();
	
	cout << roots.size() << endl;
	cout << roots[0] << "\t" << roots[1] << endl;
	
	return 0;
}



// solution[0]: 0.02932800493137973	solution[1]: -0.1377019801973668	solution[2]: 0	solution[3]: 0
// solution[0]: 0.02933085150594203	solution[1]: -0.137704826760836	solution[2]: 0	solution[3]: 0


// 0.02932906346030008  2: -0.1377030387049859 3: 0  4: 0

/*
Sim:
QuarticRootFinder - a: 24.042596055625	b: 5.210702173161462	c: 20.71514035087484	d: 2.214387241437159	e: -0.08291419168491501
QuarticRootFinder - a3: 0.2167279340844047	a2: 0.8616016466336769	a1: 0.09210266796122801	a0: -0.003448637222581312
QuarticRootFinder - c1: -0.8616016466336769	c2: 0.03375576984122409	c3: -0.02020632155399071
CubicRootFinder - Input Params -  a: 1	b: -0.8616016466336769	c: 0.03375576984122409	d: -0.02020632155399071
CubicRootFinder - Eliminate leading coefficient -  a2: -0.8616016466336769	a1: 0.03375576984122409	a0: -0.02020632155399071
CubicRootFinder - Vieta's Form -  p: -0.2136966959860638	q: 0.05789063526565199
AnalyticCubicAlgorithm - Input Params -  p: -0.2136966959860638	q: 0.05789063526565199	a2: -0.8616016466336769
AnalyticCubicAlgorithm - Polynomial Discriminant -  D: 0.0004763968684227449
AnalyticCubicAlgorithm - One Real Roots - Two Imaginary Roots
AnalyticCubicAlgorithm - C: 1.522521697621109
AnalyticCubicAlgorithm - CASE 2a -  y[0]: 1.054087091896248
AnalyticCubicAlgorithm - 1 Real Root: 0.849858897380718
QuarticRootFinder - Cubic Roots -- 1: 0.849858897380718	2: 0	3: 0
cubicSolution: 0.849858897380718
WSquared: 1.001645433262865e-10	W: 1.000822378478252e-05
W != 0.0	USquared: -3.403849106548405	VSquared: 0.02789901593210931
2 solutions: 	solution[0]: 0.02932800493137973	solution[1]: -0.1377019801973668	solution[2]: 0	solution[3]: 0
QuarticRootFinder - W: 1.000822378478252e-05	U: 0	V: 0.1670299851287466
QuarticRootFinder - No. of Real Solutions: 2
QuarticRootFinder - Root 1: 0.02932800493137973	2: -0.1377019801973668	3: 0	4: 0


/*
This:

QuarticRootFinder - a: 24.042596055625	b: 5.210702173161462	c: 20.71514035087484	d: 2.214387241437159	e: -0.08291419168491501
QuarticRootFinder - a3: 0.2167279340844046	a2: 0.8616016466336767	a1: 0.09210266796122799	a0: -0.003448637222581312
QuarticRootFinder - c1: -0.8616016466336767	c2: 0.03375576984122407	c3: -0.02020632155399071
CubicRootFinder - Input Params -  a: 1	b: -0.8616016466336767	c: 0.03375576984122407	d: -0.02020632155399071
CubicRootFinder - Eliminate leading coefficient -  a2: -0.8616016466336767	a1: 0.03375576984122407	a0: -0.02020632155399071
CubicRootFinder - Vieta's Form -  p: -0.2136966959860636	q: 0.05789063526565195
AnalyticCubicAlgorithm - Input Params -  p: -0.2136966959860636	q: 0.05789063526565195	a2: -0.8616016466336767
AnalyticCubicAlgorithm - Polynomial Discriminant -  D: 0.0004763968684227445
AnalyticCubicAlgorithm - One Real Roots - Two Imaginary Roots
AnalyticCubicAlgorithm - C: 1.52252169762111
AnalyticCubicAlgorithm - CASE 2a -  y[0]: 1.054087091896248
AnalyticCubicAlgorithm - 1 Real Root: 0.8498588973807175
QuarticRootFinder - Cubic Roots -- 1: 0.8498588973807175	2: 0	3: 0
cubicSolution: 0.8498588973807175
WSquared: 1.001643212816816e-10	W: 1.000821269166886e-05
W != 0.0	USquared: -3.403851008430337	VSquared: 0.02790091781404258
2 solutions: 	solution[0]: 0.02933085150594203	solution[1]: -0.137704826760836	solution[2]: 0	solution[3]: 0
QuarticRootFinder - W: 1.000821269166886e-05	U: 0	V: 0.167035678266778
QuarticRootFinder - No. of Real Solutions: 2
QuarticRootFinder - Root 1: 0.02933085150594203	2: -0.137704826760836	3: 0	4: 0
Solutions: 2
Roots: 0.02933085150594203	-0.137704826760836	0	0

Before - X:-0.1760561782690933	Y:-0.2	Z:-0.3419575602668274
Sqrt(X^2 + Y^2): 0.2664503291548329
After - X:-0.05484004740771814	Y:-0.1146052214256181	Z:-0.361760649717464
Sqrt(X^2 + Y^2): 0.127050334819298
matrix  - tr=1  rot=1  refl=0  scl=0
  1.000000   -0.000000    0.000000    Tx =  -0.400000
  0.000000    0.000000   -1.000000    Ty =  -0.100000
  0.000000    1.000000    0.000000    Tz =   0.000000
After - X:0.3451599525922819	Y:-0.361760649717464	Z:0.01460522142561809
Sqrt(X^2 + Y^2): 0.5000061605195559


*/
