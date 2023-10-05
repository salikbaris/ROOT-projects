void cs10() {
    
    // Declaring variables for convenience while analyzing other data.


    // Title of the histogram.
    const char *title = "Cs, 10 second interval;Number of #gamma-rays Emitted;Number of Occurance";

    // Name of the data file.
    const char *file_name = "CS10.txt";

    // Name of the file that's created at the end.
    const char *print_name = "cs10.pdf";
    
    // Histogram x-axis range.
    // Mainly used automatic range setting but in some cases of overflow,
    // ranges were set manually.
    Double_t h_xmin = 1;
    Double_t h_xmax = 1;

    // Positioning of the stat box, user range.
    Double_t pt_xmin = 115.688;
    Double_t pt_ymin = 11.3373;
    Double_t pt_xmax = 135.996;
    Double_t pt_ymax = 26.3563;

    // Positioning of the legend, NDC.
    Double_t lg_xmin = 0.136103;
    Double_t lg_ymin = 0.727848;
    Double_t lg_xmax = 0.37106;
    Double_t lg_ymax = 0.871308;



    // Creating the canvas.
    TCanvas *c1 = new TCanvas();

    // Disabling the automatic stat box.
    gStyle->SetOptStat(0);
    
    // Creating the histogram and setting fill color.
    TH1D *h1 = new TH1D("h1", title, 10, h_xmin, h_xmax);
    h1->SetFillColor(kCyan-9);

    // Filling the histogram with the data from the file.
    fstream file;
    double value;

    file.open(file_name, ios::in);
    
    while(1){
        file >> value;
        h1->Fill(value);
        if(file.eof()) break;
    }
    
    file.close();

    // Getting the relevant histogram values.
    Double_t area = h1->Integral("width");
    Double_t mean = h1->GetMean();
    Double_t std_dev = h1->GetStdDev();
    

    // Creating the distribution functiouns and setting the initial parameters
    // the same with the histogram as a headstart.
    // Also setting the colors.
    TF1 *poi = new TF1("poi", "[0]*TMath::Poisson(x, [1])");
    poi->SetParameters(area, mean);
    poi->SetParNames("constant", "mean");
    poi->SetLineColor(kRed);

    TF1 *gs = new TF1("gs", "gausn");
    gs->SetParameters(area, mean, std_dev);
    gs->SetParNames("constant", "mean", "sigma");
    gs->SetLineColor(kBlue);


    // Fitting the functions using log likelihood method
    // as suggested in ROOT documentation.
    h1->Fit(poi, "L");
    h1->Fit(gs, "L+");

    // Drawing the histogram.
    h1->Draw();


    // Getting the parameters from the Poisson fit.

    // Chi squared and number of degrees of freedom.
    Double_t poi_chi = poi->GetChisquare();
    int poi_ndf = poi->GetNDF();

    // Constant and its error.
    Double_t poi_cons = poi->GetParameter("constant");
    Double_t poi_cons_err = poi->GetParError(poi->GetParNumber("constant"));
    
    // Mean and its error.
    Double_t poi_mu = poi->GetParameter("mean");
    Double_t poi_mu_err = poi->GetParError(poi->GetParNumber("mean"));

    
    // Getting the parameters from the Gaussian fit.

    // Chi squared and number of degrees of freedom.
    Double_t gs_chi = gs->GetChisquare();
    int gs_ndf = gs->GetNDF();
    
    // Constant and its error.
    Double_t gs_cons = gs->GetParameter("constant");
    Double_t gs_cons_err = gs->GetParError(gs->GetParNumber("constant"));
    
    // Mean and its error.
    Double_t gs_mu = gs->GetParameter("mean");
    Double_t gs_mu_err = gs->GetParError(gs->GetParNumber("mean"));
    
    // Sigma and its error.
    Double_t gs_sig = gs->GetParameter("sigma");
    Double_t gs_sig_err = gs->GetParError(gs->GetParNumber("sigma"));
    
    
    // Formatting the previous values into text.

    // Histogram values.

    // Area.
    char ha[100];
    sprintf(ha, "Area = %.1f", area);

    // Mean.
    char hm[100];
    sprintf(hm, "Mean = %.1f", mean);

    // Standard deviation.
    char hsd[100];
    sprintf(hsd, "Std Dev = %.3f", std_dev);;


    // Poisson fit values.

    // Chi squared and number of degrees of freedom.
    char pch[100];
    sprintf(pch, "#chi^{2} / ndf = %.4f / %d", poi_chi, poi_ndf);

    // Constant
    char pco[100];
    sprintf(pco, "Constant = %.1f #pm %.1f", poi_cons, poi_cons_err);

    // Mean
    char pm[100];
    sprintf(pm, "Mean = %.1f #pm %.1f", poi_mu, poi_mu_err);

    
    // Gaussian fit values.

    // Chi squared and number of degrees of freedom.
    char gch[100];
    sprintf(gch, "#chi^{2} / ndf = %.4f / %d", gs_chi, gs_ndf);

    // Constant.
    char gco[100];
    sprintf(gco, "Constant = %.1f #pm %.1f", gs_cons, gs_cons_err);

    // Mean.
    char gm[100];
    sprintf(gm, "Mean = %.1f #pm %.1f", gs_mu, gs_mu_err);

    // Sigma.
    char gsd[100];
    sprintf(gsd, "Sigma = %.3f #pm %.3f", gs_sig, gs_sig_err);


    // Creating the box.
    TPaveText *pt = new TPaveText(pt_xmin, pt_ymin, pt_xmax, pt_ymax);
    
    // Adding the created texts into the box.
    // Also adding lines for readability.
    pt->AddText("Histogram");
    pt->AddText(ha);
    pt->AddText(hm);
    pt->AddText(hsd);

    pt->AddLine(.1, .685, .9, .685);
    
    pt->AddText("Poisson Fit");
    pt->AddText(pch);
    pt->AddText(pco);
    pt->AddText(pm);

    pt->AddLine(.1, .38, .9, .38);
    
    pt->AddText("Gaussian Fit");
    pt->AddText(gch);
    pt->AddText(gco);
    pt->AddText(gm);
    pt->AddText(gsd);
    
    // Setting text align and font.
    pt->SetTextAlign(22);
    pt->SetTextFont(62);
    pt->SetAllWith("=", "align", 12);
    pt->SetAllWith("=", "font", 42);

    // Drawing the box.
    pt->Draw();
    
    // Creating the legend.
    TLegend *legend = new TLegend(lg_xmin, lg_ymin, lg_xmax, lg_ymax);

    // Adding the entries to the legend.
    legend->AddEntry(poi, "Poisson Distribution", "L");
    legend->AddEntry(gs, "Gaussian Distribution", "L");
    
    // Drawing the legend.
    legend->Draw();
    
    // Saving the canvas.
    c1->Print(print_name);

}