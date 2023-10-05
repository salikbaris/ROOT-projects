// ROOT headers.
#include <all_headers.h>
using namespace std;

// Function that calculates B^2 and its error from current and current error.
// I_err is 0.01 for all data.
tuple<Double_t, Double_t> CalculateBSquared(Double_t I, Double_t I_err = 0.01){
    
    // Vacuum permeability, in Vs/Am.
    Double_t mu_zero = 1.25663706212e-6;
    
    // Coil radius, in m.
    Double_t R = 0.2;
    
    // Number of turns in the coil.
    Int_t n = 154;
    
    // Calculating B.
    Double_t B = TMath::Power(4.0/5, 3.0/2) * mu_zero * n * I / R;
    
    // Calculating uncertainty in B. This is actually constant in this experiment but 
    // here is a nice place to actually calculate it.
    Double_t B_err = TMath::Power(4.0/5, 3.0/2) * mu_zero * n * I_err / R;

    // Uncertainty in B^2.
    Double_t B_sq_err = 2 * B * B_err;

    return make_tuple(TMath::Sq(B), B_sq_err);

}

// Add point with errors.
void AddPointWithErrors(TGraphErrors *gr, Double_t x, Double_t y, Double_t ex, Double_t ey) {
    gr->AddPoint(x, y);
    gr->SetPointError(gr->GetN() - 1, ex, ey);
}

void qm2() {

    // Indicates array for constant voltage readings.
    Double_t V_array = -1;

    // Indicates array for constant current readings.
    Double_t I_array = -2;

    // Arrays of data. First element indicates the type of the readings,
    // last element is the value that's held constant.

    // Current values in A for V = 154.0 V.
    vector<Double_t> V154_I = {V_array, 3.15, 1.98, 1.43, 1.14, 154.0};
    
    // Current values in A for V = 123.8 V.
    vector<Double_t> V123_I = {V_array, 2.79, 1.65, 1.26, 0.99, 123.8};

    // Voltage values in V for I = 1.82 A.
    vector<Double_t> I182_V = {I_array, 83.6, 134.1, 226, 350, 1.82};

    // Voltage values in V for I = 1.62 A.
    vector<Double_t> I162_V = {I_array, 77.6, 108.7, 186.7, 279, 1.62};

    // Radius values in m, these are the same for all data.
    // Extra first and last elements are for convenience.
    // Every reading at the n th position in the arrays above
    // corresponds to the radius at the n th position.
    vector<Double_t> radius = {0, 0.02, 0.03, 0.04, 0.05, 0};

    // Uncertainty in radius.
    Double_t R_err = 0.0025;

    // Gathering the data arrays in an array to easily loop through them.
    vector<vector<Double_t>> data = {V154_I, V123_I, I182_V, I162_V};

    // Enabling the fit window.
    gStyle->SetOptFit(1111);

    // A 2-D array to store the q/m values and their sigmas.
    // To be used in weighted mean calculation.
    vector<vector<Double_t>> qm;

    // Setting the max digits for the labels on the axes.
    TGaxis::SetMaxDigits(3);

    // Looping through every array.
    for (vector<Double_t> arr : data) {
        
        // Creating a canvas.
        TCanvas *c1 = new TCanvas();

        // Setting it as the current canvas.
        c1->cd();

        // Creating new graph.
        TGraphErrors *gr = new TGraphErrors();
        gr->GetXaxis()->SetTitle("B^{2}R (T^{2}m)");
        gr->GetYaxis()->SetTitle("2V/R (V/m)");

        // Tried to make the x-axis labels the same
        // but couldn't manage to do it.
        // gr->GetXaxis()->SetNdivisions(508);
        // gr->GetXaxis()->SetRangeUser(20e-9, 130e-9);

        // The size of the data array.
        Int_t n = arr.size();

        // Output file name.
        TString filename;
        
        // If the array is constant V array.
        if (arr[0] == V_array) {
            
            // V value is the last element.
            Double_t V = arr.at(n - 1);
            
            // Setting the graph title.
            gr->SetTitle(Form("V = %.1f V", V));
            
            // Setting the file name.
            filename = Form("%.1fV.pdf", V);

            // Device used has higher uncertainty for v > 200.
            Double_t V_err = (V < 200) ? 0.1 : 1;
            
            // Looping through the data array, skipping the first and the last element.
            for (Int_t i = 1; i < n - 1; i++) {
                
                // Getting the radius.
                Double_t R = radius.at(i);
                
                // Calculating the y value.
                Double_t y = 2 * V / R;
                
                // Y error.
                Double_t ey = TMath::Sqrt(
                    + TMath::Sq(2 * V_err / R ) // error from V
                    + TMath::Sq(2 * V * R_err / TMath::Sq(R) ) // error from radius
                ); // sqrt end
                
                // Getting the current value.
                Double_t I = arr.at(i);
                
                // Calculating B squared.
                Double_t B_sq, B_sq_err;
                tie(B_sq, B_sq_err) = CalculateBSquared(I);

                // Calculating the x value.
                Double_t x = B_sq * R;

                // X error
                Double_t ex = TMath::Sqrt(
                    + TMath::Sq(B_sq_err * R) // error from B squared
                    + TMath::Sq(B_sq * R_err) // error from Radius
                    ); // sqrt end
                
                // Adding the point to the graph.
                AddPointWithErrors(gr, x, y, ex, ey);

            } // for end
        } // if end
        
        // If the array is constant I array.
        else if (arr[0] == I_array) {
            
            // I value is the last element.
            Double_t I = arr.at(n - 1);

            // Setting the graph title.
            gr->SetTitle(Form("I = %.2f A", I));

            // Setting the file name.
            filename = Form("%.2fI.pdf", I);

            // Calculating B squared.
            Double_t B_sq, B_sq_err;
            tie(B_sq, B_sq_err) = CalculateBSquared(I);

            // Looping through the data array, skipping the first and the last element.
            for (Int_t i = 1; i < n - 1; i++) {
                
                // Getting the voltage value.
                Double_t V = arr.at(i);

                // Its error.
                Double_t V_err = (V < 200) ? 0.1 : 1;

                // Getting the radius.
                Double_t R = radius.at(i);

                // Calculating y value.
                Double_t y = 2 * V / R;

                // Y error.
                Double_t ey = TMath::Sqrt(
                    + TMath::Sq(2 * V_err / R )                 // error from V
                    + TMath::Sq(2 * V * R_err / TMath::Sq(R) )  // error from radius
                ); // sqrt end

                // Calculating x value.
                Double_t x = B_sq * R;

                // X error.
                Double_t ex = TMath::Sqrt(
                    + TMath::Sq(B_sq_err * R) // error from B squared
                    + TMath::Sq(B_sq * R_err) // error from radius
                ); // sqrt end

                // Adding the point to the graph.
                AddPointWithErrors(gr, x, y, ex, ey);
            
            } // for end
        } // else if end

        // Drawing the graph.
        gr->Draw();

        // Creating the linear function that'll be fitted.
        TF1 *lin = new TF1("lin", "pol1");
        
        // Setting initial parameters closer.
        lin->SetParameters(0, 1e11);

        // Parameter names that'll be shown.
        lin->SetParNames("Constant", "Slope");

        // Fitting the function.
        gr->Fit(lin, "Q");

        // Getting the fit parameters.
        Double_t constant = lin->GetParameter(0);
        Double_t constant_err = lin->GetParError(0);

        // Slope is q/m.
        Double_t slope = lin->GetParameter(1);
        Double_t slope_err = lin->GetParError(1);

        // Adding the q/m values to the array.
        qm.push_back({slope, slope_err});

        // Updating the canvas so that FindObject doesn't return nullptr.
        c1->Update();

        // Positioning the stat box.
        TPaveStats *pt = (TPaveStats *)gr->GetListOfFunctions()->FindObject("stats");
        pt->SetX1NDC(0.511461);
        pt->SetY1NDC(0.147679);
        pt->SetX2NDC(0.872493);
        pt->SetY2NDC(0.348101);

        // Without this, the position of the fit box in the last canvas will not be updated.
        c1->Modified();

        // Saving the canvas.
        c1->Print(filename);

    } // for end

    // Numerator and denominator for the weighted mean.
    Double_t weighted_numerator = 0;
    Double_t weighted_denominator = 0;

    // Calculating the numerator and the denominator
    // for the weighted mean.
    for (vector<Double_t> arr : qm) {
        Double_t weight = 1 / TMath::Sq(arr.at(1));
        weighted_numerator += weight * arr.at(0);
        weighted_denominator += weight;
    }

    // Calculating the weighted mean.
    Double_t weighted_qm = weighted_numerator / weighted_denominator;
    Double_t weighted_sig = 1 / TMath::Sqrt(weighted_denominator);

    // Printing the q/m to the terminal.
    Printf("qm: %g +- %g", weighted_qm, weighted_sig);

}