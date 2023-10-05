void analysis_potential() {

    // Creating the canvas and making the frame invisible.
    TCanvas *c1 = new TCanvas();
    c1->SetFrameLineColor(0);

    // Creating the graph from the file and naming the axis titles.
    TGraphErrors *gr = new TGraphErrors("data_potential.txt");
    gr->SetTitle("");
    gr->GetXaxis()->SetTitle("Frequency(x10^{12} Hz)");
    gr->GetYaxis()->SetTitle("Stopping Potential(V)");
    
    // Drawing the graph.
    gr->Draw("AL");


    // Creating the fit function and setting the ranges.
    // Also setting closer initial parameters so that the minimizer works properly.
    TF1 *f1 = new TF1("f1", "pol1", 5.19e+14, 7.41e+14);
    f1->SetParameters(-2.80733e-01, 4.1356677e-15);
    
    // Fitting the function to the graph.
    gr->Fit("f1", "RBF");


    // Getting the chi squred, ndof, parameter and parameter error values
    // from the fit function.
    Double_t ch = f1->GetChisquare();
    int ndof = f1->GetNDF();
    Double_t p0 = f1->GetParameter(0);
    Double_t p1 = f1->GetParameter(1);
    Double_t p0_err = f1->GetParError(0);
    Double_t p1_err = f1->GetParError(1);


    // Formatting the aforementioned values into strings.
    char buffer1 [10];
    sprintf(buffer1, "%.1e", ch);
    string s1 = buffer1;

    char buffer2 [10];
    sprintf(buffer2, "%.2e", p0);
    string s2 = buffer2;

    char buffer3 [10];
    sprintf(buffer3, "%.2e", p0_err);
    string s3 = buffer3;

    char buffer4 [10];
    sprintf(buffer4, "%.2e", p1);
    string s4 = buffer4;

    char buffer5 [10];
    sprintf(buffer5, "%.2e", p1_err);
    string s5 = buffer5;

    
    // Creating the strings that will be added into the box.
    string sC = "#chi^{2} / ndof = " + s1 + " / " + to_string(ndof);
    string sP0 = "p0 = " + s2 + " #pm " + s3;
    string sP1 = "p1 = " + s4 + " #pm " + s5;

    // Creating the box and adding the texts.
    // The reason for this is that SetOptFit() was covering the graph
    // and I couldn't manage to change its position. 
    TPaveText *pt = new TPaveText(6.63398e+14, 0.336775, 7.83633e+14, 0.411041);
    pt->AddText(sC.c_str());
    pt->AddText(sP0.c_str());
    pt->AddText(sP1.c_str());

    // Changing the font of the lines that contain "=" in the box
    // as I was unable to find another way to select all the lines.
    pt->SetAllWith("=", "font", 42);
    pt->Draw();

    // Saving the canvas.
    c1->Print("potential.pdf");
}