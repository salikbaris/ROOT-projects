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
    
    // Number of turns.
    Int_t n = 154;
    
    // Calculating B.
    Double_t B = TMath::Power(4.0/5, 3.0/2) * mu_zero * n * I / R;
    
    // Calculating uncertainty in B. This is actually constant in this experiment but 
    // here is a nice place to actually calculate it.
    Double_t B_err = TMath::Power(4.0/5, 3.0/2) * mu_zero * n * I_err / R;

    // Uncertainty in B^2.
    Double_t B_sq_err = 2 * B * B_err;

    return make_tuple(TMath::Sq(B), B_err);

}

// Add point with errors.
void AddPointWithErrors(TGraphErrors *gr, Double_t x, Double_t y, Double_t ex, Double_t ey) {
    gr->AddPoint(x, y);
    gr->SetPointError(gr->GetN() - 1, ex, ey);
}

void qm3() {

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
    Double_t R_err = 0.005;

    // Gathering the data arrays in an array to easily loop through them.
    vector<vector<Double_t>> data = {V154_I, V123_I, I182_V, I162_V};

    gStyle->SetOptFit(1111);

    vector<vector<Double_t>> qm;

    for (vector<Double_t> arr : data) {
        
        TCanvas *c1 = new TCanvas();
        c1->cd();
        TGraphErrors *gr = new TGraphErrors();

        Int_t n = arr.size();
        
        if (arr[0] == V_array) {
            
            Double_t V = arr.at(n - 1);
            Double_t V_err = (V < 200) ? 0.1 : 1;
            
            for (Int_t i = 1; i < n - 1; i++) {
                
                Double_t R = radius.at(i);
                
                Double_t y = 2 * V / R;
                
                Double_t ey = TMath::Sqrt(
                    + TMath::Sq(2 * V_err / R ) // error from V
                    + TMath::Sq(2 * V * R_err / TMath::Sq(R) ) // error from radius
                ); // sqrt end
                
                Double_t I = arr.at(i);
                
                Double_t B_sq, B_sq_err;
                tie(B_sq, B_sq_err) = CalculateBSquared(I);
                Double_t x = B_sq * R;
                Double_t ex = TMath::Sqrt( TMath::Sq(B_sq_err * R) + TMath::Sq(B_sq * R_err) );

                AddPointWithErrors(gr, B_sq, B_sq_err, ex, ey);
                Printf("B: %g, err: %f", x, ex);

            } // for end
        } // if end
        
        else if (arr[0] == I_array) {
            
            Double_t I = arr.at(n - 1);
            Double_t B_sq, B_sq_err;
            tie(B_sq, B_sq_err) = CalculateBSquared(I);

            for (Int_t i = 1; i < n - 1; i++) {
                Double_t V = arr.at(i);
                Double_t V_err = (V < 200) ? 0.1 : 1;
                Double_t R = radius.at(i);

                Double_t y = 2 * V / R;

                Double_t ey = TMath::Sqrt(
                    + TMath::Sq(2 * V_err / R ) // error from V
                    + TMath::Sq(2 * V * R_err / TMath::Sq(R) ) // error from radius
                ); // sqrt end

                Double_t x = B_sq * R;
                Double_t ex = TMath::Sqrt( TMath::Sq(B_sq_err * R) + TMath::Sq(B_sq * R_err) );

                AddPointWithErrors(gr, B_sq, B_sq_err, ex, ey);

                Printf("B: %g, err: %g", x, ex);

            } // for end
        } // else if end

        gr->Draw();
        TF1 *lin = new TF1("lin", "pol1");
        lin->SetParameters(0, 1e11);
        lin->SetParNames("constant", "slope");
        gr->Fit(lin);

        Double_t constant = lin->GetParameter(0);
        Double_t constant_err = lin->GetParError(0);

        Double_t slope = lin->GetParameter(1);
        Double_t slope_err = lin->GetParError(1);
        qm.push_back({slope, slope_err});



    } // for end

    Double_t weighted_numerator = 0;
    Double_t weighted_denominator = 0;

    // Calculating the numerator and the denominator
    // for the weighted mean.
    for (vector<Double_t> arr : qm) {
        Double_t weight = 1 / TMath::Sq(arr.at(1));
        weighted_numerator += weight * arr.at(0);
        weighted_denominator += weight;
    }

    // Calculating the weighted mean, i.e. e_final and sig_final.
    Double_t weighted_qm = weighted_numerator / weighted_denominator;
    Double_t weighted_sig = 1 / TMath::Sqrt(weighted_denominator);

    Printf("qm: %g +- %g", weighted_qm, weighted_sig);


}