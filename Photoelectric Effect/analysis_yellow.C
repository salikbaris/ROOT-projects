// A function that will be used when finding 
// the intersection points of the two linear functions.
// From https://root-forum.cern.ch/t/tf1-intersection/5795
TF1 *f1l, *f2l;
Double_t finter(Double_t *x, Double_t *par) {
   return TMath::Abs(f1l->EvalPar(x,par) - f2l->EvalPar(x,par));
}

void analysis_yellow() {

    // Declaring variables for convenience when changing colors.

    // These values relate to the range of the data.
    Double_t xmin = 0;
    Double_t xmax = 3000;
    Double_t ymin = -0.5;
    Double_t ymax = 2.5;

    // These values define the range of the fit functions.
    Double_t f1_xmin = 0;
    Double_t f1_xmax = 166.9;
    Double_t f2_xmin = 1010;
    Double_t f2_xmax = 2783;

    // Setting the divisions in x axis.
    int div = 506;

    // This value defines the range of the dotted continuation of the linear
    // function on the left.
    Double_t f1l_xmax = 400;

    // These values define the position of the tip and the size of the arrow.
    Double_t arr_x = 125;
    Double_t arr_y = 0.35;
    Double_t arr_sz = 0.02;

    // Arrow text offset.
    Double_t arr_ofsx = -45;
    Double_t arr_ofsy = 0.05;

    // These values define the position of the box.
    Double_t p_xmin = 1500;
    Double_t p_xmax = 3000;
    Double_t p_ymin = 1.7;
    Double_t p_ymax = 2.5;

    // Creating graph and getting the point and error values from the file.
    TGraphErrors *gr = new TGraphErrors("data_yellow.txt");

    // Creating the canvas and making the frame invisible.
    TCanvas *c1 = new TCanvas();
    c1->SetFrameLineColor(0);


    // Creating additional X-axis so that it's at Y=0.
    // Also some visual formatting.
    TGaxis *axis1 = new TGaxis(xmin, 0, xmax, 0, xmin, xmax, div, "");
    axis1->SetLabelSize(0.03);
    axis1->SetLabelOffset(-0.06);
    axis1->SetLabelFont(42);
    axis1->SetTitle("Retarding Potential(mV)");
    axis1->SetTitleSize(0.036);
    axis1->SetTitleFont(62);
    axis1->SetTickLength(10);
    axis1->ChangeLabel(1,-1,0,-1,-1,-1,-1);

    // Setting the range of the default X-axis the same with the manually created one.
    // Then making it invisible.
    gr->GetXaxis()->SetRangeUser(xmin, xmax);
    gr->GetXaxis()->SetAxisColor(0);
    gr->GetXaxis()->SetLabelSize(0);

    // Setting the range and some visual formatting for Y-axis.
    // Also some visual formatting.
    gr->GetYaxis()->SetRangeUser(ymin, ymax);
    gr->GetYaxis()->SetTitle("Photocell Current(mA)");
    gr->GetYaxis()->SetTitleOffset(0.85);
    gr->GetYaxis()->SetTitleSize(0.036);
    gr->GetYaxis()->SetTitleFont(62);
    
    gr->SetTitle("Yellow Light");
    gr->SetMarkerStyle(kDot);
    gr->SetMarkerSize(100);

    gr->Draw("APL");
    axis1->Draw();

    // Creating and drawing the fit functions with defined ranges. 
    TF1 *f1 = new TF1("f1", "pol1", f1_xmin, f1_xmax);
    gr->Fit(f1, "R");

    TF1 *f2 = new TF1("f2", "pol1", f2_xmin, f2_xmax);
    gr->Fit(f2, "R+");

    // Getting chi squared / number of degrees of freedom values.
    Double_t ch1 = f1->GetChisquare();
    int ndof1 = f1->GetNDF();
    Double_t ch2 = f2->GetChisquare();
    int ndof2 = f2->GetNDF();


    // Getting the parameters from the fit functions to use in
    // dotted continuations of them.
    Double_t f1_p0 = f1->GetParameter(0);
    Double_t f1_p1 = f1->GetParameter(1);

    Double_t f2_p0 = f2->GetParameter(0);
    Double_t f2_p1 = f2->GetParameter(1);


    // Creating the dotted continuations and setting the parameters.
    f1l = new TF1("f1l", "[1]*x+[0]", f1_xmax, f1l_xmax);
    f1l->SetParameters(f1_p0, f1_p1);
    f1l->SetLineStyle(3);
    f1l->Draw("SAME");

    f2l = new TF1("f2l", "[1]*x+[0]", 0, f2_xmin );
    f2l->SetParameters(f2_p0, f2_p1);
    f2l->SetLineStyle(3);
    f2l->Draw("SAME");


    // Finding the intersection point by using the finter function
    // defined at the begining of the file.
    TF1 *fint = new TF1("fint", finter, f1_xmax, f2_xmin);
    Double_t xint = fint->GetMinimumX();

    // This portion is used during analysis process and is not needed after.
    Double_t y1 = f1l->Eval(xint);
    Double_t y2 = f2l->Eval(xint);
    if (fabs(y1 - y2) < 0.000005f) { // I hate decimals.
        cout << "Intersection point: (" << xint << "),(" << f1l->Eval(xint) << ")" << endl;
        cout << "Stopping Potential(mV): " << xint << endl;
    } else {
        cout << "Error in finding the intersection point." << endl;
    }

    // Creating an arrow at the intersection point.
    TArrow *arr = new TArrow(arr_x, arr_y, xint, y1, arr_sz,"<|");
    arr->Draw();

    // Adding the text above the arrow.
    TLatex *t = new TLatex(arr_x + arr_ofsx, arr_y + arr_ofsy, "#font[22]{#scale[0.8]{V_{s}}}");
    t->Draw();
    
    
    // Summing all the error in fit parameters.
    int i = 0;
    Double_t err = 0;
    while (i < 2) {
        Double_t err1_ph = f1->GetParError(i);
        Double_t err2_ph = f2->GetParError(i);
        err = err + err1_ph + err2_ph;
        i++;
    }


    // Rounding the Stopping Potential and Error values to appropriate decimals 
    // and creating strings. 
    char buffer1 [10];
    sprintf(buffer1, "%.2f",round(xint*100)/100);
    string s1 = buffer1;
    
    char buffer2 [10];
    sprintf(buffer2, "%.2f", round(err*100)/100);
    string s2 = buffer2;

    // Rounding the chi squared values to one decimal 
    // and creating strings.
    char buffer3 [10];
    sprintf(buffer3, "%.1f", round(ch1*10)/10);
    string s3 = buffer3;

    char buffer4 [10];
    sprintf(buffer4, "%.1f", round(ch2*10)/10);
    string s4 = buffer4;
    

    // Creating the strings that will be printed inside the box.
    string sV = "Stopping Potential (V_{s}) = " + s1 + " mV " + "#pm " + s2 + " mV";
    string sC1 = "Fit1: #chi^{2} / ndof = " + s3 + " / " + to_string(ndof1);
    string sC2 = "Fit2: #chi^{2} / ndof = " + s4 + " / " + to_string(ndof2);
    
    // Creating the box.
    TPaveText *pt = new TPaveText(p_xmin, p_ymin, p_xmax, p_ymax);

    // Adding the text to the box and drawing it.
    pt->AddText(sC1.c_str());
    pt->AddText(sC2.c_str());
    pt->AddText(sV.c_str());
    pt->Draw();

    
    //Saving the canvas.
    c1->Print("yellow.pdf");
}