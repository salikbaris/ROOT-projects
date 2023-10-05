void plot() {

    // Creating the canvas and making the frame invisible.
    TCanvas *c1 = new TCanvas();
    c1->SetFrameLineColor(0);

    // Enabling the automatic fit stats window.
    gStyle->SetOptFit();
    
    // Creating the arrays of mu and sqrt(mu)/sigma.
    Double_t x[4] = {1.8, 9.8, 18.8, 100.2};
    Double_t y[4] = {TMath::Sqrt(1.8)/1.354, TMath::Sqrt(9.8)/ 3.407,
            TMath::Sqrt(18.8)/4.136, TMath::Sqrt(100.2)/9.994};

    
    // Creating the graph with the arrays and some visual formatting.
    TGraph *gr = new TGraph(4, x, y);
    gr->GetYaxis()->SetRangeUser(0,2);
    gr->SetTitle();
    gr->GetXaxis()->SetTitle("#mu");
    gr->GetXaxis()->SetTitleSize(0.05);
    gr->GetXaxis()->SetTitleOffset(0.6);
    gr->SetMarkerStyle(kFullCircle);
    gr->SetMarkerSize(0.7);

    // Fitting a line to see if the slope is really 1.
    gr->Fit("pol1");
    // Setting the parameter names that will be shown.
    gr->GetFunction("pol1")->SetParNames("Constant", "Slope");
    
    // Drawing the graph.
    gr->Draw("APL");


    // Adding additional text as axis title.
    // Because I wasn't able to rotate the title from SetTitle() method.
    TLatex *l = new TLatex(-12, 1.8, "#frac{#sqrt{#mu}}{#sigma}");
    l->Draw();

    // Saving the canvas.
    c1->Print("plot.pdf");
}