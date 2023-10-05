// ROOT headers.
#include <all_headers.h>
using namespace std;

// Function that finds the index of the maximum point of the graph in the given range.
// Not well designed but does the job for setting the fit range.
Double_t GetMaxIdx(TGraph *gr, Double_t from, Double_t to) {
    Double_t max_val = -INFINITY; // max value
    Int_t idx; // index of the max point
    Double_t *x = gr->GetX(); // getting the x values from the graph
    Double_t *y = gr->GetY(); // getting the y values from the graph
    
    for (Int_t i = 0; i < gr->GetN(); i++) {

        // Skipping the x values that are not in the given range.
        if (x[i] < from) continue;
        if (x[i] > to) break;
        
        // Comparing the y values and the max value.
        // Storing the idx and y value if a new max is found.
        if (y[i] > max_val) {
            max_val = y[i];
            idx = i;
        } // if end

    } // for end


    return idx;
} // func end

// Function that finds the endpoints of a peak in x axis.
// Starts form start_point and looks for points_searched points before and after the peak.
// If it finds a difference between successive points less than tolerance, returns that point.
// Can be improved a lot but again, does the job for setting the fit range.
tuple<Double_t, Double_t> FindPeakEnds(TGraph *gr, Double_t peak_idx, Int_t start_point = 2, Int_t points_searched = 30, Double_t tolerance = 10) {
    
    // Break booleans.
    Bool_t min_not_found = true;
    Bool_t max_not_found = true;
    
    Double_t x_miny; // y value of the left end point
    
    Double_t x_maxy; // y value of the right end point

    // Endpoints.
    Double_t x_min;
    Double_t x_max;

    Double_t diff; // the difference between y values of the points
    
    // Not starting from the adjacent points because they can be really close,
    // resulting in incorrect results.
    for (Int_t i = start_point; i < start_point + points_searched + 1; i++) {
        
        if (min_not_found) {
            
            x_miny = gr->GetPointY(peak_idx - i);

            // The difference between x_miny and the point "before" x_miny.
            diff = x_miny - gr->GetPointY(peak_idx - i - 1);

            if (diff < tolerance) {
            
                min_not_found = false;
                x_min = gr->GetPointX(peak_idx - i);
            
            } // if end
        
        } // if end

        if (max_not_found) {
            
            x_maxy = gr->GetPointY(peak_idx + i);
            
            // The difference between x_maxy and the point "after" x_maxy.
            diff = x_maxy - gr->GetPointY(peak_idx + i + 1);

            if (diff < tolerance) {
                
                max_not_found = false;
                x_max = gr->GetPointX(peak_idx + i);
            
            } // if end
        
        } // if end
        
        // If both x_min and x_max are found.
        else if (!min_not_found) break;

    }

    return make_tuple(x_min, x_max);
}

// Add point with errors.
void AddPointWithErrors(TGraphErrors *gr, Double_t x, Double_t y, Double_t ex, Double_t ey) {
    gr->AddPoint(x, y);
    gr->SetPointError(gr->GetN() - 1, ex, ey);
}

// Set point with errors.
void SetPointWithErrors(TGraphErrors *gr, Int_t i, Double_t x, Double_t y, Double_t ex, Double_t ey) {
    gr->SetPoint(i, x, y);
    gr->SetPointError(i, ex, ey);
}

void xray() {

    ////////////////////////////////////////////////
    /// Calibration



    // Creating the canvas. This is where the peaks will be fitted fits gaussian functions.
    TCanvas *c_calib_data = new TCanvas();

    // Graph for the calibration data, reading from the file.
    TGraphErrors *gr_calib_data = new TGraphErrors("35.txt");
    
    // Setting the point errors.
    for (Int_t i = 0; i < gr_calib_data->GetN(); i++) {
        Double_t y = gr_calib_data->GetPointY(i);
        gr_calib_data->SetPointError(i, 0.1, TMath::Sqrt(y / 5));

    }

    // Setting the titles.
    gr_calib_data->SetTitle("V_{acc} = 35 kV");
    gr_calib_data->GetXaxis()->SetTitle("Angle (#circ)");
    gr_calib_data->GetYaxis()->SetTitle("Intensity #scale[1.5]{(}#frac{Imp}{s}#scale[1.5]{)}");

    // Number of peaks that'll be fitted.
    const Int_t num_peaks = 7;

    // Rough points enclosing the peaks.
    Double_t calib_peak_x1[num_peaks] = {7  , 7.5, 13, 14, 19.5, 22, 30.5};
    Double_t calib_peak_x2[num_peaks] = {7.5, 8  , 14, 15, 20  , 23, 31  };
    
    // Start and stop points of the fit.
    Double_t fit_x1;
    Double_t fit_x2;

    // Index of the fit.
    Int_t peak_idx;

    // Drawing the graph.
    gr_calib_data->Draw("APZ");
    
    // Saving the canvas.
    c_calib_data->Print("Calibration Data.pdf");


    // Order for Bragg's Law.
    Int_t n;

    // 2-D arrays for the angles and their error.
    vector<vector<Double_t>> k_alpha1_lines_exp;
    vector<vector<Double_t>> k_beta1_lines_exp;

    // Temporary variables.
    Double_t angle_exp;
    Double_t angle_exp_err;

    for (Int_t i = 0; i < num_peaks; i++) {
        // Finding the index of the peak.
        peak_idx = GetMaxIdx(gr_calib_data, calib_peak_x1[i], calib_peak_x2[i]);

        // Finding the peak ends.
        tie(fit_x1, fit_x2) = FindPeakEnds(gr_calib_data, peak_idx);

        // Creating the fit function with defined ranges.
        TF1 *gaus_fit = new TF1("gaus", "gaus", fit_x1, fit_x2);

        // Fitting without drawing and in the function range.
        gr_calib_data->Fit(gaus_fit, "0RQ");

        // Drawing a copy of the function,
        // so that it's not erased in the next iteration.
        gaus_fit->DrawCopy("CSAME");

        // Getting the mean of the fit.
        angle_exp = gaus_fit->GetParameter("Mean");

        // Error of the mean.
        angle_exp_err = gaus_fit->GetParError(gaus_fit->GetParNumber("Mean"));

        // Printing the values to the terminal.
        Printf("n: %d, mean: %f +- %f", (i/2)+1, angle_exp ,angle_exp_err);

        // Adding the values to their respective arrays and counting n.
        if (i % 2 == 0) {
            n +=1;
            k_beta1_lines_exp.push_back({angle_exp, angle_exp_err});
        }
        else {
            k_alpha1_lines_exp.push_back({angle_exp, angle_exp_err});
        }
    } // for end

    // Disabling the fit box. Somehow, enabling it further down affects this.
    gr_calib_data->SetStats(0);

    // Saving the fitted graph.
    c_calib_data->Print("Fitted Calibration.pdf");

    // Arrays for theoretical angles. Their uncertainties may not be used.
    vector<vector<Double_t>> k_alpha1_lines_theo;
    vector<vector<Double_t>> k_beta1_lines_theo;
    
    // Theoretical lambda values and their uncertainty.
    Double_t lambdas[2] = {0.709328e-10, 0.632345e-10};
    Double_t lambdas_err[2] = {0.000022e-10, 0.000038e-10};

    // Lattice parameter for NaCl and its uncertainty.
    Double_t d = 282e-12;
    Double_t d_err = 8e-12;

    // Temporary variables.
    // Uncertainties are calculated but as mentioned, may not be used.
    Double_t angle_alpha;
    Double_t angle_alpha_err;
    Double_t angle_beta;
    Double_t angle_beta_err;

    
    for (Int_t i = 1; i <= n; i++) {
        
        // Calculating K_alpha1 angle.
        angle_alpha = TMath::RadToDeg() * TMath::ASin( i * lambdas[0] / (2 * d) );

        // Its error, may be omitted. 
        angle_alpha_err = TMath::RadToDeg() * TMath::Sqrt(
                + TMath::Sq( i / (2 * d * TMath::Sqrt(1 - TMath::Sq(i*lambdas[0]/(2*d) ) ) ) * lambdas_err[0]) // error from lambda
                + TMath::Sq(i * lambdas[0] / (2 * TMath::Sq(d) * TMath::Sqrt(1 - TMath::Sq(lambdas[0]*i / (2 * d) ) ) ) * d_err) // error from d  
            ); // Sqrt end

        // Adding to the array.
        k_alpha1_lines_theo.push_back({angle_alpha, angle_alpha_err});
        
        // Calculating K_beta1 angle.
        angle_beta = TMath::RadToDeg() * TMath::ASin( i * lambdas[1] / (2 * d) );

        // Its error, may be omitted.
        angle_beta_err = TMath::RadToDeg() * TMath::Sqrt(
                + TMath::Sq( i / (2 * d * TMath::Sqrt(1 - TMath::Sq(i*lambdas[1]/(2*d) ) ) ) * lambdas_err[1]) // error from lamba
                + TMath::Sq(i * lambdas[1] / (2 * TMath::Sq(d) * TMath::Sqrt(1 - TMath::Sq(lambdas[1]*i / (2 * d) ) ) ) * d_err) // error from d
            ); // Sqrt end

        // Adding to the array.
        k_beta1_lines_theo.push_back({angle_beta, angle_beta_err});

    } // for end



    
    // Canvas to compare experimental and theoretical angles.
    TCanvas *c_calib_fit = new TCanvas();
    c_calib_fit->cd();
    gStyle->SetOptFit(1111);

    
    // Creating the graph.
    TGraphErrors *gr_calib_fit = new TGraphErrors();
    // Title.
    gr_calib_fit->SetTitle("Experimental vs Theoretical #theta Values;Experimental #theta Values(#circ);Theoretical #theta Values(#circ)");
    
    // Temporary variables.
    Double_t x;
    Double_t ex;
    Double_t y;
    Double_t ey;

    for (Int_t i = 0; i < k_beta1_lines_exp.size(); i++) {
        
        // Getting the values from the arrays.
        x = k_beta1_lines_exp[i][0];
        ex = k_beta1_lines_exp[i][1];
        y = k_beta1_lines_theo[i][0];
        ey = k_beta1_lines_theo[i][1];
        Printf("found: %f, expected: %f", x, y);

        // Adding them to the graph.
        // ey is not used. Some hard-coded variables need to be changed if used.
        AddPointWithErrors(gr_calib_fit, x, y, ex, 0);
        
        // Checking if we still have alpha1 angles.
        if (i < k_alpha1_lines_exp.size()) {

            // Same as before
            x = k_alpha1_lines_exp[i][0];
            ex = k_alpha1_lines_exp[i][1];
            y = k_alpha1_lines_theo[i][0];
            ey = k_alpha1_lines_theo[i][1];
            AddPointWithErrors(gr_calib_fit, x, y, ex, 0);
            Printf("found: %f, expected: %f", x, y);
        }

    }
    
    
    // Drawing the graph.
    gr_calib_fit->Draw("APL");

    // Creating the calibration function.
    TF1 *calibrate = new TF1("calibrate", "pol1");
    calibrate->SetParNames("Constant", "Slope");
    calibrate->SetParameters(1, 5);

    // Fitting it to the graph.
    gr_calib_fit->Fit(calibrate);

    // Saving the canvas.
    c_calib_fit->Print("Calibration Function.pdf");

    // Getting the parameter values from the function.
    Double_t slope_calib = calibrate->GetParameter(1);
    Double_t slope_err = calibrate->GetParError(1);
    Double_t constant_calib = calibrate->GetParameter(0);
    Double_t constant_err = calibrate->GetParError(0);

    // Scaling the errors by 10 after the fit, for better visualization.
    for (Int_t i = 0; i < gr_calib_fit->GetN(); i++) {
        gr_calib_fit->SetPointError(i, gr_calib_fit->GetErrorX(i) * 10, 0);
    }
    
    // FindObject method will return null pointer without updating the pad.
    c_calib_fit->Update();

    // Positioning the stat box.
    TPaveStats *pt = (TPaveStats *)gr_calib_fit->GetListOfFunctions()->FindObject("stats");
    pt->SetX1NDC(0.492837);
    pt->SetY1NDC(0.183544);
    pt->SetX2NDC(0.855301);
    pt->SetY2NDC(0.383966);




    ////////////////////////////////////////////////
    /// Extracting E and f_max values


    // Path to the data.
    const char *inDir = "$ROOTSYS/../PHYS442/X-ray/";
    char *dir = gSystem->ExpandPathName(inDir);
    void *dirp = gSystem->OpenDirectory(dir);
    
    // Loop variable for the file.
    const char *entry;

    // File name will be stored as TString.
    TString filename;

    // Title of the graph.
    TString title;

    // Constants.
    // Electron charge, in C.
    const Double_t q = 1.60217663e-19;

    // Speed of light, in m/s.
    const Double_t c = 299792458;


    // Voltage applied.
    Double_t V;

    // Temporary variables.
    Double_t angle;
    Double_t angle_err;
    Double_t intensity;
    Double_t intensity_err;
    Double_t lambda_exp;
    Double_t lambda_exp_err;
    Double_t lambda_min;
    Double_t lambda_min_err;
    Double_t f_max;
    Double_t f_max_err;

    // 2-D Array of f_max values and errors.
    vector<vector<Double_t>> f_arr;

    // Array of qV values.
    vector<Double_t> E_arr;

    while ((entry = (char *)gSystem->GetDirEntry(dirp))) {

        filename = entry;
        
        // Skipping the calibration data.
        if (filename.EqualTo("35.txt")) {
            continue;
        }

        // .txt files should be exclusively data files.
        else if (filename.EndsWith(".txt")) {

            // Creating a canvas.
            TCanvas *c_analyze = new TCanvas();

            // Creating the graph from the file.
            TGraphErrors *gr = new TGraphErrors(filename);
            
            // Don't need the .txt suffix anymore.
            filename.ReplaceAll(".txt", "");
            
            // Getting the V value.
            V = filename.Atof() * 1000;

            // Looping through all points to modify.
            for (Int_t i = 0; i < gr->GetN(); i++) {
                
                // Calibrating the angle.
                angle = slope_calib * gr->GetPointX(i) + constant_calib;

                // Converting it to radians, as the functions that'll be used
                // need rad.
                angle *= TMath::DegToRad();
                

                // Eval is also a choice but I'm not sure how error is calculated then.
                // angle = calibrate->Eval(angle);

                // Error on angle
                angle_err = TMath::DegToRad() * TMath::Sqrt(
                    + TMath::Sq(angle * slope_err) // error from the slope
                    + TMath::Sq(slope_calib * 0.1) // error from the angle
                    + TMath::Sq(constant_err) // error from the constant
                ); // Sqrt end

                // Calculating the wavelength, for n = 1.
                lambda_exp = 2 * d * TMath::Sin(angle);

                // Error on the wavelength.
                lambda_exp_err = TMath::Sqrt(
                    // + TMath::Sq(2 * TMath::Sin(angle) * d_err) // d error, may be omitted
                    + TMath::Sq(2 * d * TMath::Cos(angle) * angle_err)
                ); // Sqrt end

                intensity = gr->GetPointY(i);

                // Error on the intensity.
                intensity_err = TMath::Sqrt(intensity / 5);
                
                // Setting the modified point.
                SetPointWithErrors(gr, i, lambda_exp, intensity, lambda_exp_err, intensity_err);
                
            } // for end
            

            // This didn't have any effect unfortunately but maybe the problem is in my system.
            gr->GetXaxis()->SetMaxDigits(2);
            
            // Creating the title.
            title = "V_{acc} = " + filename + " kV";
            
            // Setting the titles.
            gr->SetTitle(title);
            gr->GetXaxis()->SetTitle("Wavelength (m)");
            gr->GetYaxis()->SetTitle("Intensity #scale[1.5]{(}#frac{Imp}{s}#scale[1.5]{)}");

            // Drawing the graph.
            gr->Draw("APL");

            // Setting the fit ranges manually for every case.
            if (V == 15e3) {
                fit_x1 = 0.081e-9;
                fit_x2 = 0.085e-9;
            }
            else if (V == 18e3) {
                fit_x1 = 0.066e-9;
                fit_x2 = 0.072e-9;
            }
            else if (V == 21e3) {
                fit_x1 = 0.06e-9;
                fit_x2 = 0.064e-9;
            }
            else if (V == 24e3) {
                fit_x1 = 0.050e-9;
                fit_x2 = 0.056e-9;
            }
            else if (V == 27e3) {
                fit_x1 = 0.046e-9;
                fit_x2 = 0.06e-9;
            }
            else if (V == 30e3) {
                fit_x1 = 0.048e-9;
                fit_x2 = 0.052e-9;
            }


            // Creating the fit function.
            TF1 *func = new TF1("func", "pol1", gr->GetXaxis()->GetXmin(), gr->GetXaxis()->GetXmax());
            func->SetParameters(1, 1e12);

            // Fitting it with ranges set.
            gr->Fit(func, "0Q", "", fit_x1, fit_x2);
            
            // Drawing the function.
            func->Draw("LSAME");

            // Saving the canvas.
            c_analyze->Print(Form("%d kV Fitted.pdf", (Int_t)(V/1000)));

            // Calculating lambda_min.
            lambda_min = -func->GetParameter(0) / func->GetParameter(1);

            // Error of lambda_min
            lambda_min_err = TMath::Sqrt(
                + TMath::Sq(func->GetParError(0) / func->GetParameter(1)) // error from the constant
                + TMath::Sq(func->GetParameter(0) / (2 * TMath::Sq(func->GetParameter(1))) * func->GetParError(1)) // error from the slope
            ); // Sqrt end

            // Calculating f_max
            f_max = c / lambda_min;

            // Error of f_max
            f_max_err = c / (2 * TMath::Sq(lambda_min)) * lambda_min_err;
            
            Printf("V: %.f, lambda_min: %g +- %g, f_max: %g +- %g", V, lambda_min, lambda_min_err, f_max, f_max_err);
            Double_t h = q * V / f_max;
            Double_t h_err = q * V / (2 * TMath::Sq(f_max)) * f_max_err;
            // Printf("V: %.f kV, h: %g +- %g", V/1000, h, h_err);
            
            // Adding E and f_max to their respective arrays.
            f_arr.push_back({f_max, f_max_err});
            E_arr.push_back(q*V);
            
        } // if end


    } // while end


    ////////////////////////////////////////////////
    /// Drawing and fitting E vs f_max


    // Creating a canvas and setting it as the active pad.
    TCanvas *c_energy = new TCanvas();
    c_energy->cd();

    // Creating the E vs f_max garph.
    TGraphErrors *gr_energy = new TGraphErrors();

    gr_energy->SetTitle("Energy vs Frequency");
    gr_energy->GetXaxis()->SetTitle("Frequency (Hz)");
    gr_energy->GetYaxis()->SetTitle("Energy (J)");
    
    // Filling the graph.
    for (Int_t i = 0; i < f_arr.size(); i++) {
        AddPointWithErrors(gr_energy, f_arr[i][0], E_arr[i], f_arr[i][1], 0);
    }
    
    
    // Sorting the points.
    gr_energy->Sort();

    // Drawing the graph.
    gr_energy->Draw();

    // Creating the function that will be fitted.
    TF1 *planck = new TF1("planck", "pol1");
    planck->SetParNames("Constant", "Slope");
    planck->SetParameters(0, 1e-34);

    // Fitting the function.
    gr_energy->Fit(planck);

    // Repositioning the stat box.
    c_energy->Update();
    TPaveStats *ps = (TPaveStats *)gr_energy->GetListOfFunctions()->FindObject("stats");
    ps->SetX1NDC(0.52149);
    ps->SetY1NDC(0.168776);
    ps->SetX2NDC(0.882521);
    ps->SetY2NDC(0.369198);

    c_energy->Modified();

    c_energy->Print("E vs f.pdf");

}