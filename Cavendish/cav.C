void cav() {

    // Creating the canvas and enabling the grid.
    TCanvas *c1 = new TCanvas();
    c1->SetGrid();

    // Creating the graph using the data in the file.
    // Errors are appended as columns into the file.
    TGraphErrors *gr = new TGraphErrors("voltage.txt");
    gr->SetTitle();
    gr->GetXaxis()->SetTitle("Time (s)");
    gr->GetYaxis()->SetTitle("Voltage (V)");

    
    // Drawing without lines, as points can't be seen when they are connected by lines.
    // Also drawing without error bars because they really distort the visual and
    // do not introduce extra information as the points are too close to each other
    // to get an idea of the error size.
    gr->Draw("APZ");
    c1->Print("full_plot.pdf");


    // Getting the axis range values, to be used later.
    double x_min = gr->GetXaxis()->GetXmin();
    double x_max = gr->GetXaxis()->GetXmax();

    /////////////////////////////////////////////////////
    /// Resonance Mode

    // Resonance mode function.
    TF1 *resonance_func = new TF1("resonance_func", "[0]+[1]*sin([2]*(x-2411)+[3])");
    resonance_func->SetParNames("A", "B", "#omega_{0}", "#phi");
    resonance_func->SetParameters(3.4575, 0.0325, 0.05, 0);
    
    // using (x-2411) eliminated the need to set parameter limits.
    // resonance_func->SetParLimits(0, 3.455, 3.46);
    // resonance_func->SetParLimits(1, 0.03, 0.034);
    // resonance_func->SetParLimits(2, 0.01, 0.1);

    // Enabling fit box with only parameter and error values.
    gStyle->SetOptFit();
    

    // Setting the range closer to the resonance region.
    gr->GetXaxis()->SetRangeUser(2300, 2800);

    // Title for the resonance mode.
    gr->SetTitle("Driven Resonance Mode");
    
    // Fitting in the specified range.
    gr->Fit(resonance_func, "M", "", 2411, 2640);

    // Saving the canvas, i.e. resonance region.
    c1->Print("resonance.pdf");

    

    
    /////////////////////////////////////////////////////
    /// Decay Mode

    // Decay mode function.
    TF1 *decay_func = new TF1("decay_func", "[0]+[1]*exp(-[2]*(x-2800))*sin([3]*(x-2800)+[4])");
    decay_func->SetParNames("A_{d}", "B_{d}", "#beta", "#omega_{d}", "#phi_{d}");
    decay_func->SetParameters(3.445, 0.035, 1, 0.05, 0);
    
    
    // Setting the range closer to the decay region.
    gr->GetXaxis()->SetRangeUser(2700, 4459.4);

    // Title for the decay mode.
    gr->SetTitle("Free Decay Mode");

    // Fitting in the specified range.
    gr->Fit(decay_func, "M", "", 2800, 3600);

    // Saving the canvas, i.e. decay region.
    c1->Print("decay.pdf");


    
    /////////////////////////////////////////////////////    
    /// For reference, setting the axis range to its initial values,
    /// so that fit regions can be seen on the whole data.

    // "Deleting" the fit box.
    // I have spent 2 hours to figure these 3 lines out.
    TPaveStats *ps = (TPaveStats*)c1->GetPrimitive("stats");
    ps->SetX1(0); // It's ridiculous that I can't simply delete this.
    ps->SetX2(0);
    
    // Setting the axis range to its initial values.
    gr->GetXaxis()->SetRangeUser(x_min, x_max);

    // Removing the title.
    gr->SetTitle();
    
    resonance_func->SetRange(2411, 2640);
    // decay_func->SetRange(2700, 4459.4);
    resonance_func->Draw("SAME");

    c1->Print("fits.pdf");


    /////////////////////////////////////////////////////
    /// Calculations
    
    // Making space.
    cout << endl;

    // Given values.

    // Inertia of the rod and small spheres.
    const double inertia = 1.43e-4;
    const double inertia_err = 6.17e-3 * inertia;

    // Mass of the small spheres.
    const double m = 0.014573;
    const double m_err = 6.86e-5 * m;

    // Mass of the large spheres.
    const double M = 1.0385;
    const double M_err = 9.63e-4 * M;

    // The distance between large and small spheres
    // when they are positioned closely.
    const double r = 0.0461025;
    const double r_err = 3.42e-3 * r;

    // The distance of the small sphere center from the axis of rotation.
    const double d = 0.066653;
    const double d_err = 5.57e-4 * d;


    // Measured values.

    // The distance between the rod axis of rotation and the ruler.
    const double L = 163;
    const double L_err = 1;

    // Laser's max-min positions on the ruler
    // corresponding to the max and min values of the resonance mode.
    const double s_min = 10.3;
    const double s_max = 10.8;
    const double s_minmax_err = 0.2;

    // The difference between min-max values on the ruler.
    const double S = TMath::Abs(s_max - s_min);
    const double S_err = sqrt(TMath::Sq(s_minmax_err) + TMath::Sq(s_minmax_err) );
    printf("deltaS: %g +- %g\n", S, S_err);

    // Frequency of the resonance mode.
    const double omega_0 = resonance_func->GetParameter(2);
    const double omega_0_err = resonance_func->GetParError(2);
    
    printf("omega_0: %g +- %g\n", omega_0, omega_0_err);
    

    // Calculated values.

    // Max and min voltage difference in the resonance mode, i.e. 2B.
    const double V = 2 * resonance_func->GetParameter(1);
    const double V_err = 2 * resonance_func->GetParError(1);
    
    printf("deltaV: %g +- %g\n", V, V_err);

    // The difference between A and A_d.
    const double deltaA = resonance_func->GetParameter(0) - decay_func->GetParameter(0);
    
    const double deltaA_err = sqrt(
        TMath::Sq(resonance_func->GetParError(0)) +
        TMath::Sq(decay_func->GetParError(0))
    ); // sqrt end

    printf("deltaA: %g +- %g\n", deltaA, deltaA_err);

    
    

    // Torsion constant
    const double k = pow(omega_0, 2) * inertia;

    const double k_err = sqrt( 
        TMath::Sq(2 * omega_0 * inertia * omega_0_err) + // omega error
        TMath::Sq( TMath::Sq(omega_0) * inertia_err) // inertia error
    ); // sqrt end
    
    printf("k: %g +- %g\n", k, k_err);


    // Conversion constant between angle and voltage.
    const double dtheta = S / (2 * L);

    const double dtheta_err = sqrt(
        TMath::Sq( (1 / (2*L)) * S_err) + // S error
        TMath::Sq( (S / (2 * TMath::Sq(L)) ) * L_err ) // L error
    ); // sqrt end

    printf("dtheta: %g +- %g\n", dtheta, dtheta_err);

    // Torque due to the fiber.
    const double torque =  k * (dtheta / V) * deltaA;
    
    const double torque_err = sqrt(
        TMath::Sq( (dtheta / V) * deltaA * k_err) + // k error
        TMath::Sq(k * (1/V) * deltaA * dtheta_err) + // dtheta error
        TMath::Sq(k * ( dtheta / TMath::Sq(V) ) * deltaA * V_err) + // V error
        TMath::Sq(k * (dtheta / V) * deltaA_err)
    ); // sqrt end

    printf("Torque: %g +- %g\n", torque, torque_err);

    // Gravitational constant
    const double G = (torque * TMath::Sq(r)) / (2 * M * m * d);
    
    const double G_err = sqrt(
        TMath::Sq( (TMath::Sq(r) / (2 * M * m * d) ) * torque_err) + // torque error
        TMath::Sq( (torque * 2 * r / (2 * M * m * d) ) * r_err) + // r error
        TMath::Sq( (torque * TMath::Sq(r)) / (2 * TMath::Sq(M) * m * d) * M_err) + // M error 
        TMath::Sq( (torque * TMath::Sq(r)) / (2 * M * TMath::Sq(m) * d) * m_err) + // m error
        TMath::Sq( (torque * TMath::Sq(r)) / (2 * M * m * TMath::Sq(d)) * d_err) // d error
    ); // sqrt end

    printf("G: %g +- %g\n", G, G_err);

    // Radius of the Earth.
    const double R = 6371000;

    // Gravitational accelaration due to Earth's mass.
    const double g = 9.80665;

    // Mass of the Earth.
    const double earth_mass = g * TMath::Sq(R) / G;
    
    const double earth_mass_err = sqrt(
        TMath::Sq( (g * TMath::Sq(R) / TMath::Sq(G) ) * G_err)
    ); // sqrt end

    printf("Mass of the Earth: %g +- %g\n", earth_mass, earth_mass_err);


    // True value for G.
    const double true_G = 6.67430e-11;
    
    // How many sigmas it's away from the experimental value.
    const double sigmas_G = TMath::Abs(true_G - G) / G_err;
    
    printf("G, sigmas away: %g\n", sigmas_G);

    // True value for the mass of the Earth.
    const double true_earth_mass = 5.972e24;

    // How many sigmas it's away from the experimental value.
    const double sigmas_earth = TMath::Abs(true_earth_mass - earth_mass) / earth_mass_err;
    
    printf("Earth mass, sigmas away: %g\n", sigmas_earth);

    // Making space.
    cout << endl;

}