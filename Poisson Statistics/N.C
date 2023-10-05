void N() {

    // Creating the canvas and setting the window size, just to be safe.
    TCanvas *c1 = new TCanvas();
    c1->SetWindowSize(1200, 600);
    c1->SetFixedAspectRatio();
    
    // Disabling the automatic stat box.
    gStyle->SetOptStat(0);
    
    // Dividing the canvas into 2, one for each case.
    c1->Divide(2, 1);

    /////////////////////////////////////////////////////////////
    //// n=0 case
    
    // Setting the first division as the active pad.
    c1->cd(1);

    // Creating the histogram and setting fill color.
    TH1D *h0 = new TH1D("h0", "n = 0 case;Time(s);Number of Occurance", 12, 0, 50);
    h0->SetFillColor(kCyan-9);


    // Taking the data from the file and filling the histogram.
    // Also counting the number of peaks and total time.
    fstream file;
    Double_t value;
    Double_t peaks, total_time;

    file.open("n0.txt", ios::in);

    while (1) {
        
        file >> value;
        peaks += 1;
        value *= 10; // turning cm into mm.
        total_time += value;  
        h0->Fill(value);
        if(file.eof()) break;

    }
    
    file.close();

    peaks += 1; // Counting the peak at the end.


    // Calculating alpha and formatting it into char,
    // for displaying in the box.
    Double_t alpha = peaks / total_time;
    char exp_alpha[100];
    sprintf(exp_alpha, "Expected #alpha = %.4f", alpha);

    
    // Getting the histogram values.
    Double_t area0 = h0->Integral("width");
    Double_t mean0 = h0->GetMean();
    Double_t stddev0 = h0->GetStdDev();
    
    // Creating the n=0 case of the modified Poisson distribution.
    TF1 *n0 = new TF1("n0", "[0]*TMath::Exp(-[1]*x)*[1]");
    
    // Setting the initial parameters.
    n0->SetParameters(area0, alpha);

    
    // Fitting and drawing.
    // Using log likelihood method as suggested in ROOT documentation.
    h0->Fit(n0, "L");
    h0->Draw();

    
    // Getting the fit data and errors.
    Double_t n0_chi = n0->GetChisquare();
    int n0_ndf = n0->GetNDF();
    Double_t n0_const = n0->GetParameter(0);
    Double_t n0_c_err = n0->GetParError(0);
    Double_t n0_alp = n0->GetParameter(1);
    Double_t n0_alp_err = n0->GetParError(1);

    
    // Formatting the values into char to display in the box.
    char h0a[100];
    sprintf(h0a, "Area = %.2f", area0);

    char h0m[100];
    sprintf(h0m, "Mean = %.3f", mean0);

    char h0s[100];
    sprintf(h0s, "Std Dev = %.4f", stddev0);

    char n0chnd[100];
    sprintf(n0chnd, "#chi^{2} / ndf = %.3f / %d", n0_chi, n0_ndf);

    char n0co[100];
    sprintf(n0co, "Constant = %.2f #pm %.2f", n0_const, n0_c_err);

    char n0alp[100];
    sprintf(n0alp, "Experimental #alpha = %.4f #pm %.4f", n0_alp, n0_alp_err);

    
    
    // Creating the text box and adding the text.
    // Also adding a line for readability.
    TPaveText *pt0 = new TPaveText(27.8802, 23.8481, 55, 50.4131);
    
    pt0->AddText("Histogram");
    pt0->AddText(h0a);
    pt0->AddText(h0m);
    pt0->AddText(h0s);

    pt0->AddLine(0.1, 0.55, 0.9, 0.55);

    pt0->AddText("Fit");
    pt0->AddText(n0chnd);
    pt0->AddText(n0co);
    pt0->AddText(n0alp);

    pt0->AddLine(0.1, 0.11, 0.9, 0.11);

    pt0->AddText(exp_alpha);
    
    // Setting text alignment and font.
    pt0->SetTextAlign(22);
    pt0->SetTextFont(62);
    pt0->SetAllWith("=", "align", 12);
    pt0->SetAllWith("=", "font", 42);
    
    // Drawing the box.
    pt0->Draw();


    /////////////////////////////////////////////////////////////
    //// n=1 case

    // Setting the second division as the active pad.
    c1->cd(2);


    // Creating the histogram and setting fill color.
    TH1D *h1 = new TH1D("h1", "n = 1 case;Time(s);Number of Occurance", 12, 0, 60.1);
    h1->SetFillColor(kCyan-9);

    
    // Taking the data from the file and filling the histogram.
    file.open("n1.txt", ios::in);

    while (1) {
        file >> value;
        value *= 10; // turning cm into mm.
        h1->Fill(value);
        if(file.eof()) break;
    }

    file.close();


    // Getting the histogram values.
    Double_t area1 = h1->Integral("width");
    Double_t mean1 = h1->GetMean();
    Double_t stddev1 = h1->GetStdDev();

    
    // Creating the n=1 case of the modified Poisson distribution.
    TF1 *n1 = new TF1("n1", "[0]*([1]*x)*TMath::Exp(-[1]*x)*[1]");
    n1->SetParameters(area1, alpha);

    
    // Fitting and drawing.
    // Using log likelihood method as suggested in ROOT documentation.
    h1->Fit(n1, "L");
    h1->Draw();


    
    // Getting the fit data and errors.
    Double_t n1_chi = n1->GetChisquare();
    int n1_ndf = n1->GetNDF();
    Double_t n1_const = n1->GetParameter(0);
    Double_t n1_c_err = n1->GetParError(0);
    Double_t n1_alp = n1->GetParameter(1);
    Double_t n1_alp_err = n1->GetParError(1);
    


    // Formatting the values into char to display in the box.
    char h1a[100];
    sprintf(h1a, "Area = %.2f", area1);

    char h1m[100];
    sprintf(h1m, "Mean = %.3f", mean1);

    char h1s[100];
    sprintf(h1s, "Std Dev = %.4f", stddev1);

    char n1chnd[100];
    sprintf(n1chnd, "#chi^{2} / ndf = %.3f / %d", n1_chi, n1_ndf);

    char n1co[100];
    sprintf(n1co, "Constant = %.2f #pm %.2f", n1_const, n1_c_err);

    char n1alp[100];
    sprintf(n1alp, "Experimental #alpha = %.4f #pm %.4f", n1_alp, n1_alp_err);



    // Creating the text box and adding the text.
    // Also adding a line for readability.
    TPaveText *pt1 = new TPaveText(33.2508, 18.6638, 66.11, 39.4538);
    pt1->AddText("Histogram");
    pt1->AddText(h1a);
    pt1->AddText(h1m);
    pt1->AddText(h1s);

    pt1->AddLine(0.1, 0.55, 0.9, 0.55);

    pt1->AddText("Fit");
    pt1->AddText(n1chnd);
    pt1->AddText(n1co);
    pt1->AddText(n1alp);

    pt1->AddLine(0.1, 0.11, 0.9, 0.11);

    pt1->AddText(exp_alpha);
    
    pt1->SetTextAlign(22);
    pt1->SetTextFont(62);
    pt1->SetAllWith("=", "align", 12);
    pt1->SetAllWith("=", "font", 42);
    
    // Drawing the box.
    pt1->Draw();


    // Saving the canvas.
    c1->Print("N.pdf");

}