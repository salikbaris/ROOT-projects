void V3000_5() {
    
    // Declaring variables for convenience when analyzing other data.
    
    // Number of data points.
    int n = 22;

    // Name of the Data file.
    const char *file_name = "3000-5.txt";

    // Title of the graph.
    const char *title = "3000V, 5 squeezes";

    // Name of the output file.
    const char *print_name = "3000-5.pdf";
    
    
    
    // Creating the canvas, enabling the grid and the fit window.
    TCanvas *c1 = new TCanvas();
    c1->SetGrid();
    gStyle->SetOptFit();
    c1->SetFrameLineColor(0);
    
    
    // Initializing the variables.
    double s, total_time, x[n], y[n], ex[n], ey[n];

    
    // Reading the data from the file.
    fstream file;
    file.open(file_name, ios::in);
    
    for(int i = 0; i < n; i++) {
        file >> s;
        x[i] = s / 2 + total_time; // Calculating T_i
        y[i] = 1 / s;
        ex[i] = (i != 0) ? ex[i-1] + 0.15 : 0.15; // T_i error
        ey[i] = 0.3 / pow(s, 2); // (1/s)_i error
        total_time += s;
    }

    file.close();
    
    
    // Creating the graph with the data points and errors.
    TGraphErrors *gr = new TGraphErrors(n, x, y, ex, ey);
    gr->SetTitle(title);
    gr->GetXaxis()->SetTitle("Time(s)");
    gr->GetYaxis()->SetTitle("1/Charging Period(1/s)");
    
    // Disabling the ticks as they were blocking some points.
    gr->GetYaxis()->SetTickSize(0);
    gr->GetXaxis()->SetTickSize(0);


    
    // Creating the exponential function.
    TF1 *f1 = new TF1("f1", "[0]*TMath::Exp(-[1]*x)");

    // Setting the parameter names that will be shown.
    f1->SetParNames("N_0", "Decay Constant");

    // Setting initial parameter values so that the minimizer works properly.
    f1->SetParameters(5, 0.01246667591);

    
    // Fitting and drawing the graph.
    gr->Fit(f1);
    gr->Draw("AP");

    // Saving the canvas.
    c1->Print(print_name);

    // Saving the decay constant and its error in a file.
    file.open("decay.txt", ios::app);
    
    double decay = f1->GetParameter(1);
    double decay_err = f1->GetParError(1);
    
    char buffer[100];
    sprintf(buffer, "%.9f %.9f\n", decay, decay_err);
    
    file << buffer;

    file.close();

    
    // Extra bit to see all the data points in the terminal.
    for(int i = 0; i < n; i++) {
        printf("%d: X = %f + %f -- Y = %f + %f\n\n", i, gr->GetPointX(i), gr->GetErrorX(i), gr->GetPointY(i), gr->GetErrorY(i));
    }

}