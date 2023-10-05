void plateau() {
    // Creating canvas.
    TCanvas *c1 = new TCanvas();

    // Creating the graph using the data from the file plateau.txt.
    // Setting titles and marker.
    TGraph *gr = new TGraph("plateau.txt");
    gr->SetTitle("100s interval");
    gr->GetYaxis()->SetRangeUser(0, 3000); // To emphasize the plateau.
    gr->SetMarkerStyle(kFullCircle);
    gr->GetYaxis()->SetTitle("Count");
    gr->GetXaxis()->SetTitle("High Voltage");
    gr->Draw("APL");

    // Creating the ellipse that wraps the plateau region.
    // Making the ellipse transparent and dashed.
    TEllipse *el = new TEllipse(428.367, 954.114, 90, 270);
    el->SetFillStyle(0);
    el->SetLineStyle(2);
    el->Draw();

    // Creating the arrow.
    TArrow *arr = new TArrow(383.668, 1183.54, 369, 1681.96, 0.02, "|>");
    arr->Draw();

    // Creating the text above the arrow.
    TLatex *tx = new TLatex(337.679, 1705.7, "#scale[0.8]{#font[22]{Plateau Region}}");
    tx->Draw();

    
    // Additional arrow and text to point out the operating voltage.
    TArrow *arr2 = new TArrow(440, 0, 455.014, 352.848, 0.02, "|>");
    arr2->Draw();

    TLatex *tx2 = new TLatex(419.341, 447.785, "#scale[0.8]{Operating Voltage: 440 V}");
    tx2->Draw();

    // Saving the canvas.
    c1->Print("plateau.pdf");
}