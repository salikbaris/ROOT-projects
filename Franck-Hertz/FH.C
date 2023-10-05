#include <all_headers.h>

// Function that finds the x value of the maximum point of the graph in the given range.
// Not well designed but does the job for setting the fit range.
Double_t GetMaxX(TGraph *gr, Double_t from, Double_t to) {
    Double_t max_val = -1e20; // arbitrary small number
    Double_t max_x;
    Double_t *x = gr->GetX(); // getting the x values from the graph
    Double_t *y = gr->GetY(); // getting the y values from the graph
    
    for (Int_t i = 0; i < gr->GetN(); i++) {

        // Skipping the x values that are not in the given range.
        if (x[i] < from)
            continue;
        if (x[i] > to)
            break;
        
        // Comparing the y values and the max value.
        // Storing the x and y value if a new max is found.
        if (y[i] > max_val) {
            max_val = y[i];
            max_x = x[i];
        } 

        // Declaring the max_x as the point in between the successive maxes.
        else if (y[i] == max_val && y[i - 1] == max_val) {
            max_x = (max_x + x[i]) / 2;
        }

    } // for end

    if (!max_x) {
        Printf("Initial max_val too high");
    }
    return max_x;
} // func end


// The code relies heavily on the file name format of the data.
// It needs to be "Txxx-U1x.xx-U3x.xx.txt" where x are digits(except in txt of course).
// Also, the data is assumed to be in the 4th and 5th columns, 4th being y axis and 5th x axis.
void FH() {

    ///////////////////////////////////////////
    /// DRAWING AND FITTING THE GRAPHS

    // Path to the data.
    const char *inDir = "$ROOTSYS/../PHYS442/Franck-Hertz";
    char *dir = gSystem->ExpandPathName(inDir);
    void *dirp = gSystem->OpenDirectory(dir);

    // This is the loop variable for file names.
    const char *entry;

    // The file name will be stored as a TString. 
    TString filename;

    // The title for the graphs.
    TString title;

    // T, U1, U3 values extracted from the filename.
    TString T_str, U1_str, U3_str;

    // T, U1, U3 values as Double_t.
    Double_t T, U1, U3;

    // Arrays of T, U1, U3.
    std::vector<Double_t> T_arr, U1_arr, U3_arr;

    // Temporary x and y values, used to swap X and Y axes
    // Because in the files y column is before the x column.
    Double_t temp_x, temp_y;
    
    // Errors for the x and y values of the final graph.
    Double_t ex = 0.05;
    Double_t ey = 0.005;

    // X values that define the fit range.
    Double_t fit_x1, fit_x2;

    // Number of peaks in the data that will be fitted.
    Int_t num_peaks;

    // X value of the first peak.
    Double_t peak_x;

    // X values that define the range of the first peak.
    // Used to find the peak.
    Double_t first_peak_x_min;
    Double_t first_peak_x_max;

    // The distance between peaks, roughly.
    // Accuracy here is not that important as it's used to find the peak.
    Double_t peak_offset = 5;

    // The distance between fit limits and the peak.
    Double_t fit_range;
    
    // Arrays of mean and std dev values from the fits.
    std::vector<Double_t> gaus_mean_arr;
    std::vector<Double_t> gaus_std_dev_arr;
    
    // Array of E values from each two successive peaks from one data.
    std::vector<Double_t> e_temp_arr;
    // E value from each two successive peaks of one data.
    Double_t e_temp;
    
    // Array of sigma value for e_temp from one data.
    std::vector<Double_t> sig_temp_arr;
    // Sigma value for e_temp from one data.
    Double_t sig_temp;
    
    // Array of weighted means of e_temp for each data.
    std::vector<Double_t> e_final_arr;
    // Weighted mean of e_temp for one data.
    Double_t e_final;
    
    // Array of sigma values of the weighted means.
    std::vector<Double_t> sig_final_arr;
    // Sigma value of the weighted mean for one data.
    Double_t sig_final;

    // Weight for the weighted mean calculations.
    Double_t weight;

    // Numerator and denominator for the weighted mean calculations.
    Double_t weighted_numerator;
    Double_t weighted_denominator;


    // Looping through every file in the path dirp.
    while ((entry = (char *)gSystem->GetDirEntry(dirp))) {

        // Storing the file name as TString.
        filename = entry;

        // Checking if the file is a data file.
        // It's important that .txt files are exclusively data files.
        if (filename.EndsWith(".txt")) {
            
            // Creating a canvas if it's a data file.
            TCanvas *c1 = new TCanvas();

            // Resetting the number of peaks and fit range.
            // fit_range doesn't change actually but can be modified if desired.
            num_peaks = 0;
            fit_range = 1;

            // Getting the T value from the file name.
            // This section relies on the file name format.
            T_str = filename.operator()(1, 3);
            T = T_str.Atof();
            
            // Getting the U1 value from the file name.
            U1_str = filename.operator()(7, 4);
            U1 = U1_str.Atof();
            
            // Getting the U3 value from the file name.
            U3_str = filename.operator()(14, 4);
            U3 = U3_str.Atof();
            
            // Creating the temporary graph from the file.
            // Our desired x and y axes are reversed.
            // Here our desired x values correspond to the 5th column and
            // y values correspond to the 4th column.
            TGraph *temp_gr = new TGraph(filename, "%*lg %*lg %*lg %lg %lg %*lg");

            // Creating the graph that we'll use.
            TGraphErrors *gr = new TGraphErrors();

            // Getting the y and x values from temp_gr
            // as x and y values for gr.
            for (Int_t i = 0; i <  temp_gr->GetN(); i++) {
                temp_x = temp_gr->GetPointX(i);
                temp_y = temp_gr->GetPointY(i);
                gr->AddPoint(temp_y, temp_x);
                gr->SetPointError(i, ex, ey); // adding the errors
            } // for end
            
            // Destructing the temp_gr object as we don't need it anymore.
            temp_gr->~TGraph();
            
            // Creating the title of the graph.
            title = "T = " + T_str + ", U_{1} = " + U1_str + ", U_{3} = " + U3_str;

            // Setting the graph and axes titles.
            gr->SetTitle(title);
            gr->GetXaxis()->SetTitle("U_{2} (V)");
            gr->GetYaxis()->SetTitle("I (nA)");

            // Drawing the graph.
            gr->Draw("AP");

            // Saving the raw graph.
            filename.ReplaceAll(".txt", ".pdf");
            c1->Print(filename);
            
            // Setting the data specific values.
            // This could be grouped and made more organized 
            // but with the flow, turned out like this.

            if (T == 171 && U1 == 0.58 && U3 == 1.69) {
                num_peaks = 5;
                first_peak_x_min = 9.4; // first peak doesn't look very gaussian
                first_peak_x_max = 11.4;
            }

            else if (T == 171 && U1 == 0.62 && U3 == 1.83) {
                gr->RemovePoint(0); // has an initial noise
                num_peaks = 5; // ignoring the last peak as it's flat
                first_peak_x_min = 5;
                first_peak_x_max = 7;
            }

            else if (T == 186 && U1 == 0.62 && U3 == 1.69) {
                num_peaks = 5;
                first_peak_x_min = 10;
                first_peak_x_max = 12;
            }

            else if (T == 192 && U1 == 0.62 && U3 == 1.69) {
                num_peaks = 4;
                first_peak_x_min = 14;
                first_peak_x_max = 16;
            }

            else if (T == 171 && U1 == 0.58 && U3 == 1.56) {
                gr->RemovePoint(0); // has initial noise
                num_peaks = 6;
                first_peak_x_min = 4;
                first_peak_x_max = 6;
            }
            
            else if (T == 171 && U1 == 0.59 && U3 == 1.83) {
                num_peaks = 6;
                first_peak_x_min = 5;
                first_peak_x_max = 6;

            }

            else if (T == 171 && U1 == 0.68 && U3 == 1.83) {
                gr->RemovePoint(0); // has initial noise  
                num_peaks = 5; // last peak is flat 
                first_peak_x_min = 4;
                first_peak_x_max = 7;
            }

            else if (T == 176 && U1 == 0.62 && U3 == 1.69) {
                gr->RemovePoint(0); // has initial noise
                num_peaks = 6;
                first_peak_x_min = 4;
                first_peak_x_max = 7;
            }
            
            else if (T == 171 && U1 == 0.58 && U3 == 1.96) {
                num_peaks = 5;
                first_peak_x_min = 9.4;
                first_peak_x_max = 11.4;
            }
            
            
            // Ignoring the bad cases that are not checked for, i.e. T=181 case.
            if (num_peaks == 0) continue;
            
            // Fitting every accepted peak with a Gaussian distribution.
            for (Int_t i = 0; i < num_peaks; i++) {

                // Finding the peak at every peak_offset interval,
                // Starting from the first_peak_x_min - first_peak_x_max range.
                peak_x = GetMaxX(gr, first_peak_x_min + i * peak_offset, first_peak_x_max + i * peak_offset);

                // Setting the fit range by adding and subtracting fit_range
                // from peak_x.
                fit_x1 = peak_x - fit_range;
                fit_x2 = peak_x + fit_range;

                // Creating the fit function with the defined range.
                TF1 *func = new TF1("func", "gaus", fit_x1, fit_x2);

                // Fitting the function without drawing.
                // Also in silent mode to not drown the terminal.
                gr->Fit(func, "QNR", "");

                // Drawing the copy of the fitted function
                // so that it'll stay when the next peak is fitted.
                func->DrawCopy("CSAME");

                // Adding the values from the fit to their respective arrays.
                gaus_mean_arr.push_back(func->GetParameter("Mean"));
                gaus_std_dev_arr.push_back(func->GetParameter("Sigma"));
            
            } // for end
            

            // Saving the fitted graph.
            filename.Prepend("(Fitted)");
            c1->Print(filename);

            // Calculating e_temp values for the data 
            // by subtracting successive fit means.
            for (Size_t i = 0; i < gaus_mean_arr.size() - 1; i++) {

                // e_temp.
                e_temp = gaus_mean_arr.at(i+1) - gaus_mean_arr.at(i);
                
                // Error of e_temp.
                sig_temp = TMath::Sqrt( 
                    TMath::Sq(gaus_std_dev_arr.at(i+1)) +
                    TMath::Sq(gaus_std_dev_arr.at(i))
                    ); // Sqrt end
                
                // Adding e_temp and sig_temp to their respective arrays.
                // Note that sigma of the nth element of e_temp_arr
                // is the nth element of sig_temp_arr.
                e_temp_arr.push_back(e_temp);
                sig_temp_arr.push_back(sig_temp);

            } // for end

            
            // Resetting weighted variables from the previous iteration.
            weighted_numerator = 0;
            weighted_denominator = 0;

            // Calculating the numerator and the denominator
            // for the weighted mean.
            for (Size_t i = 0; i < e_temp_arr.size(); i++) {
                weight = 1 / TMath::Sq(sig_temp_arr.at(i));
                weighted_numerator += weight * e_temp_arr.at(i);
                weighted_denominator += weight;
            }
            
            // Calculating the weighted mean, i.e. e_final and sig_final.
            e_final = weighted_numerator / weighted_denominator;
            sig_final = 1 / TMath::Sqrt(weighted_denominator);

            // Adding the e_final and sig_final to their respective arrays.
            // Note that sigma of the nth element of e_final_arr
            // is the nth element of sig_final_arr.
            e_final_arr.push_back(e_final);
            sig_final_arr.push_back(sig_final);

            // Also adding the T, U1, U3 values to their arrays for reference.
            // Same nth element case.
            T_arr.push_back(T);
            U1_arr.push_back(U1);
            U3_arr.push_back(U3);


            // Clearing the arrays corresponding to one specific data set.
            gaus_mean_arr.clear();
            gaus_std_dev_arr.clear();
            e_temp_arr.clear();
            sig_temp_arr.clear();
       
        } // if end

    } // while end


    //////////////////////////////////////////////
    /// HISTOGRAM


    // Creating the canvas for the histogram.
    TCanvas *c2 = new TCanvas("c2", "Histogram");

    // Setting c2 as the current pad.
    c2->cd();

    // Enabling the histogram stat box without name.
    gStyle->SetOptStat(1110);

    // Enabling the fit box with probability.
    gStyle->SetOptFit(1111);

    // Setting the significant digits on the fit box.
    gStyle->SetFitFormat(".2f");

    // Number of bins of the histogram.
    const Int_t N_BINS = 10;

    // Array that defines the edges of the bins.
    Double_t EDGES[N_BINS + 1] = { 0 };

    // Low edge of the first bin.
    Double_t bin_start = 4.88;

    // Width of the bins.
    Double_t bin_width = 0.02;

    // Creating the edges with fixed bin widths.
    for (Int_t i = 0; i < N_BINS + 1; i++) {
        EDGES[i] = bin_start + (i * bin_width);
    }

    // Creating the histogram.
    TH1D *e_hist = new TH1D("e_hist", "E_{final}", N_BINS, EDGES);

    // Setting the fill color for the histogram.
    e_hist->SetFillColorAlpha(kCyan-9, 0.35);

    // Filling the histogram with e_final values.
    for (Size_t i = 0; i < e_final_arr.size(); i++) {

        // Also printing them with their T, U1, U3 values.
        Printf("T: %.f, U1: %.2f, U3: %.2f, E_final: %.2f, Sig_final: %.2f",
        T_arr.at(i), U1_arr.at(i), U3_arr.at(i), e_final_arr.at(i), sig_final_arr.at(i)
        ); // Printf end

        e_hist->Fill(e_final_arr.at(i));


    } // for end


    // E_final values in the histogram.
    Double_t hist_e;

    // Error of the histogram bin squared.
    Double_t bin_sig_sq;
    
    // Error of the histogram bin.
    Double_t bin_sig;

    // Low edge of the histogram bin.
    Double_t bin_low_edge;

    // Upper edge of the histogram bin
    Double_t bin_upper_edge;


    // Looping through the bins of the histogram
    for (Int_t i = 1; i <= N_BINS; i++) {

        // Getting the edges of the bin
        bin_low_edge = e_hist->GetBinLowEdge(i);
        bin_upper_edge = bin_low_edge + bin_width;

        // Resetting bin_sig_sq from the previous iteration.
        bin_sig_sq = 0;

        // Iterating through all e_final values.
        for (Int_t j = 0; j < e_final_arr.size(); j++) {
            
            // e_final value.
            hist_e = e_final_arr.at(j);

            // Checking if hist_e falls in the bin.
            if ( hist_e >= bin_low_edge && hist_e < bin_upper_edge) {

                // Getting the corresponding sigma squared for hist_e.
                bin_sig_sq += TMath::Sq(sig_final_arr.at(j));
            
            } // if end

        } // for end

        // Taking the square root.
        bin_sig = TMath::Sqrt(bin_sig_sq);

        // Setting the error on the bin.
        e_hist->SetBinError(i, bin_sig);

    } // for end

    // Drawing the histogram.
    e_hist->Draw("E1 HIST");

    // Saving the unfitted histogram.
    c2->Print("Histogram.pdf");

    // Disabling the stat box.
    gStyle->SetOptStat(0);
    // c2->Modified();

    // Fitting a gaussian distribution to the histogram.
    e_hist->Fit("gaus");
    
    // Drawing the fitted function.
    // I thought it should be drawn automatically
    // but didn't.
    e_hist->GetFunction("gaus")->Draw("SAME");

    c2->Print("(Fitted)Histogram.pdf");

    
    // Extra weighted mean calculation.
    weighted_numerator = 0;
    weighted_denominator = 0;
    
    // Calculating the numerator and the denominator
    // for the weighted mean.
    for (Size_t i = 0; i < e_final_arr.size(); i++) {
        weight = 1 / TMath::Sq(sig_final_arr.at(i));
        weighted_numerator += weight * e_final_arr.at(i);
        weighted_denominator += weight;
    }
    
    // Calculating the weighted mean of e_finals.
    Double_t e_final_weighted = weighted_numerator / weighted_denominator;
    Double_t sig_final_weighted = 1 / TMath::Sqrt(weighted_denominator);

    Printf("E_final_weighted: %.2f, Sig_final_weighted: %.2f", e_final_weighted, sig_final_weighted);



}