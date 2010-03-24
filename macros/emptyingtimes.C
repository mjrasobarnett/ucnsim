
Int_t emptyingtimes() {

   const Int_t points = 11;
   Double_t x[points] = {0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
   Double_t y[points] = {15.66,12.11,12.93,13.95,12.75,14.74,13.81,12.7,13.19,13.73,14.4};
   Double_t ex[points] = 0;
   Double_t ey[points] = {0.79,0.58,0.55,0.66,0.61,0.67,0.64,0.6,0.61,0.67,0.7};
   
   TGraph* graph  = new TGraphErrors(points, x, y, 0, ey);
   graph->SetTitle("roughness vs emptying time");
   graph->GetXaxis()->SetTitle("Roughness parameter");
   graph->GetYaxis()->SetTitle("Emptying time, (s)");
   graph->GetXaxis()->SetLimits(-0.1,1.1);
//   graph->GetYaxis()->SetRangeUser(0.,18.);
   graph->SetMarkerColor(4);
   graph->SetMarkerStyle(21);
   graph->Draw("AP");
   
   return 0;
}