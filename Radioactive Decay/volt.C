void volt() {
    
    // Creating the canvas and enabling the grid.
    TCanvas *c1 = new TCanvas();
    c1->SetGrid();

    // Initializing the arrays and dummy variables.
    double dec[4], sig[4];
    double decay, sigma;

    
    // Reading the data into the array. 2500V is the first element, 3000V the second, etc.
    fstream file;

    file.open("decay.txt", ios::in);
    
    for(int i = 0; i < 16; i++) {
        file >> decay >> sigma;
        if(i < 4) {
            dec[0] += decay / 4; // Dividing now to not loop through again.
            sig[0] += sigma / 4;
            }
        else if(i < 8) {
            dec[1] += decay / 4;
            sig[1] += sigma / 4;
            }
        else if(i < 12) {
            dec[2] += decay / 4;
            sig[2] += sigma / 4;
            }
        else if(i < 16) {
            dec[3] += decay / 4;
            sig[3] += sigma / 4;
            }
    }
    
    file.close();
    

    // Initializing the array for the x-axis.
    double volts[4] = {2500, 3000, 3500, 4000};

    
    // Creating the graph and some visual formatting.
    TGraphErrors *gr = new TGraphErrors(4, volts, dec, nullptr, sig);
    
    gr->SetTitle("#lambda as a Function of High Voltage");
    gr->GetXaxis()->SetNdivisions(505);
    gr->GetXaxis()->SetTitle("High Voltage(V)");
    gr->GetYaxis()->SetTitle("#lambda (#scale[0.8]{#times 10^{-3} #frac{1}{s}} )");
    gr->GetYaxis()->SetTitleSize(0.06); 
    gr->GetYaxis()->SetTitleOffset(0.7);
    gr->GetYaxis()->SetMaxDigits(3);
    gr->GetYaxis()->SetRangeUser(0.005, 0.013); // setting the same with the other graph for reference
    gr->SetMarkerStyle(7);
    
    
    gr->Draw();

    // Saving the canvas.
    c1->Print("vlt.pdf");


    // Printing the points to the terminal.
    gr->Print();
    
}