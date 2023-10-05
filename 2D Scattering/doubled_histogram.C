void n_histogram() {

    TCanvas *c1 = new TCanvas();

    Int_t N_BINS = 10;

    Double_t EDGES[11] = {0, 10, 30, 50, 70, 90, 110, 130, 150, 170, 180};

    TH1D *hist = new TH1D("hist", "dN vs #theta;#theta;dN", N_BINS, EDGES);
    hist->SetFillColorAlpha(kCyan-9, 0.35);

    fstream file;

    file.open("counts_doubled.txt", ios::in);

    int value;

    for(int i = 0; i < 9; i++) {
        file >> value;
        hist->SetBinContent(10-i, value);
        hist->SetBinError(10-i, TMath::Sqrt(value));
    }

    hist->Draw("HIST E1");

    for(int i = 0; i <15; i++){
        printf("%d: %f +- %f\n", i, hist->GetBinCenter(i), hist->GetBinError(i));
    }

    c1->Print("histogram_n.pdf");










}