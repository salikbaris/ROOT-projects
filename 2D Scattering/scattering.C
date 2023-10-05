void scattering() {
    
    //////////////////////////////////////////////////////////////////////////
    //// HISTOGRAM

    // Disabling the stat window. It was blocking the last bin.
    gStyle->SetOptStat(0);

    // Creating the canvas and enabling the grid on y axis for readability.
    TCanvas *c1 = new TCanvas("", "Histogram");
    c1->SetGridy();

    // number of bins = number of data points + 0-10 and 350-360 intervals.
    const int N_BINS = 19;
    
    // Size of the angle interval, in degrees.
    const double dtheta = 20;

    // Array to define the edges of the bins.
    double EDGES[N_BINS+1] = { 0 };

    // Skipping the first element because it's already 0 and the method relies on i-1 th element.
    for(int i = 1; i < N_BINS + 1; i++) { 
        if(i == N_BINS || i == 1) EDGES[i] = EDGES[i-1] + dtheta / 2;
        else EDGES[i] = EDGES[i-1] + dtheta;
    }
    
    // Creating the histogram with N_BINS bins and with bin edges defined by EDGES.
    TH1D *hist = new TH1D("hist", "dN vs #theta;#theta(#circ);dN(#)", N_BINS, EDGES);
    
    // I believe in colors.
    hist->SetFillColorAlpha(kCyan-9, 0.35);

    // Getting the data from the file and filling the histogram.
    fstream file;
    
    file.open("counts.txt", ios::in);
    
    // Dummy variable
    double value;

    // The file was in reverse order. Also skipping the first and the last bins as they are ignored.
    for(int i = N_BINS-1; i > 1; i--) {
        file >> value;
        hist->SetBinContent(i, value);
        hist->SetBinError(i, TMath::Sqrt(value) );
    }

    file.close();

    // Aligning the tick marks with the center of the bins and the default label size annoyed me a bit.
    hist->GetXaxis()->SetNdivisions(19);
    hist->GetXaxis()->SetLabelSize(0.03);

    // Drawing the histogram.
    hist->Draw("HIST E1");

    // To proof-check, with underflow and overflow bins.
    for(int i = 0; i < N_BINS+2; i++) {
        printf(
            "bin %d: %f: %f +- %f\n",
            i, hist->GetBinCenter(i),
            hist->GetBinContent(i),
            hist->GetBinError(i));
    }

    // Saving the histogram canvas.
    c1->Print("histogram.pdf");

    //////////////////////////////////////////////////////////////////////////
    //// GRAPH
    
    // Creating the canvas
    TCanvas *c2 = new TCanvas("", "Graph");

    // Setting the new canvas as the active pad.
    c2->cd();
    // Enabling the grid.
    c2->SetGrid();
    // Enabling the fit window.    
    gStyle->SetOptFit();

    // Number of data points for the graph. Note that this includes the ignored 0-10 range.
    const int n = (N_BINS + 1) / 2;

    // Initializing the arrays that will define the graph.
    double x[n];
    double y[n];
    double ey[n];
    
    // Number of shots per position of the gun.
    double shots = 20;
    
    // Total number of turns we've made.
    double turns = 43;
    
    // Total distance the gun moved, in cm;
    double distance_gun = 5.8;
    
    // Error in the distance_gun.
    double gun_err = 0.3; // 0.2 cm from marking the positions, 0.1 from the ruler precision.
    
    // The distance the gun moves in one turn.
    double distance_turn = distance_gun / turns;
    
    // Error in the distance_turn.
    double turn_err = gun_err / turns;
    
    // Incident flux on the target, shots/cm.
    double flux = shots / distance_turn;
    
    // Error in the flux.
    double flux_err = (shots / TMath::Sq(distance_turn) ) * turn_err;

    

    // Filling the arrays back to the front.
    // Starts from the middle(180 deg), n th bin and sums n-i th and n+i th bins for y values.
    // Also summing the bin errors for the error on the corresponding y value.
    // Getting the theta value for calculating the x value from the n-i th bin center.
    // Note that the ignored 0-10 deg and 350-360 deg intervals are also present. 
    for(int i = 0; i < n; i++) {
        x[(n-1)-i] = TMath::Sin( (TMath::DegToRad() * hist->GetBinCenter(n - i) ) / 2);
        y[(n-1)-i] = hist->GetBinContent(n - i) + hist->GetBinContent(n + i);
        ey[(n-1)-i] = hist->GetBinError(n - i) + hist->GetBinError(n + i);
    }

    // Creating the graph with the arrays.
    TGraphErrors *gr = new TGraphErrors(n, x, y, nullptr, ey);

    // Removing the first point, as it corresponds to the ignored 0-10 deg and 350-360 deg intervals.
    gr->RemovePoint(0);

    // Setting the titles. Couldn't think of a symbol for sin value, if there is one.
    gr->SetTitle("dN vs sin(#theta/2)");
    gr->GetXaxis()->SetTitle("sin#scale[2]{(}#frac{#theta}{2}#scale[2]{)}");
    gr->GetYaxis()->SetTitle("dN(#)");
    
    // Setting the marker style.
    gr->SetMarkerStyle(kFullCircle);
    
    // To see the points on the terminal.
    cout << endl;
    gr->Print();
    cout << endl;

    // Drawing the graph.
    gr->Draw("ALP");

    // Creating the function that will be fitted.
    // Also setting the parameter names that will be shown in the fit window.
    TF1 *func = new TF1("func", "pol1");
    func->SetParNames("Constant", "Slope");

    // Fitting the function, without drawing it.
    // In case of setting a fit range.
    gr->Fit(func, "0");

    // Setting the function range the same with the existing axis,
    // so that it is drawn throughout the current axis.
    func->SetRange( gr->GetXaxis()->GetXmin(), gr->GetXaxis()->GetXmax() );

    // Drawing the function.
    func->Draw("SAME");
    
    // Saving the graph canvas.
    c2->Print("graph.pdf");


    // Getting the slope and its error values.
    double slope =  func->GetParameter(1);
    double slope_err = func->GetParError(1);


    // Calculating the experimental radius from the slope.
    double exp_radius = 2 * slope / (flux * TMath::DegToRad() * dtheta);

    // Calculating its error.
    double exp_err = TMath::Sqrt(
        TMath::Sq(2 * slope_err / (flux * TMath::DegToRad() * dtheta) )
      + TMath::Sq(2 * slope * flux_err / (TMath::Sq(flux) * TMath::DegToRad() * dtheta) )
        );



    // Initializing the cross section variables.
    double cross_section = 0;
    double cross_section_err = 0;
    double differential_cross_section;

    // Calculating the cross section.
    for(int i = 0; i < n-1; i++) {
        // The differential cross section corresponding to the i th point on the graph.
        differential_cross_section = slope / (flux * TMath::DegToRad() * dtheta) * gr->GetPointX(i);

        // Adding it to the total cross section.
        cross_section += differential_cross_section;

        // Calculating the error on the differential cross section and adding it to the total error.
        cross_section_err += TMath::Sqrt(
            TMath::Sq( slope_err / (flux * TMath::DegToRad() * dtheta) * gr->GetPointX(i) )
          + TMath::Sq( slope * flux_err / (TMath::Sq(flux) * TMath::DegToRad() * dtheta) * gr->GetPointX(i) )
            );
    }
    


    // Initializing the numerator and the denominator values for the theoretical value.
    double numerator = 0, denominator = 0, denominator_err = 0;

    // Summing the values on the numerator and the denominator.
    // Also summing the errors for the denominator.
    for(int i = 0; i < n - 1; i++) {
        numerator += gr->GetPointX(i);
        denominator += TMath::Sq( gr->GetPointX(i) ) * dtheta / gr->GetPointY(i);
        denominator_err += TMath::Sq( gr->GetPointX(i) ) * dtheta * gr->GetErrorY(i)
            / TMath::Sq( gr->GetPointY(i) );
    }

    // Calculating the theoretical radius.
    // Not multiplying by 2 because y values are already counted twice
    // and assuming symmetry.
    double theoretical_radius = numerator / denominator;

    // Its error, from the least squares method and dN.
    double theoretical_err = TMath::Sqrt( (1 / denominator) 
        + TMath::Sq( numerator * denominator_err / TMath::Sq(denominator) ) );

    
    
    // Printing the relevant variables to the terminal.
    printf("Initial and final position distance: %f +- %f\n", distance_gun, gun_err);
    printf("Turn distance: %f +- %f\n", distance_turn, turn_err);
    printf("Flux: %f +- %f\n", flux, flux_err);
    printf("Slope: %f +- %f\n", slope, slope_err);

    // Printing the experimental radius and its error to the terminal.
    printf("Experimental radius: %f +- %f\n", exp_radius, exp_err);

    // Printing the cross section and its error,
    // and the radius from the cross section and its error to the terminal.
    printf("Cross section: %f +- %f\nCross radius: %f +- %f\n",
        cross_section, cross_section_err, cross_section/2, cross_section_err/2);
    

    // Printing the theoretical radius and its error to the terminal.
    printf("Theoretical radius: %f +- %f\n",
        theoretical_radius, theoretical_err);

    
    

}