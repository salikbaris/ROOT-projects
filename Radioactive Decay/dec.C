void dec() {

    // Initializing the variables.
    double decay, sigma, weight, dec_times_w, w_sum;

    // Reading the data from the file.
    fstream file;

    file.open("decay.txt", ios::in);
    while(1) {
        file >> decay >> sigma;
        weight = 1 / pow(sigma, 2); // Weight of the i th lambda
        dec_times_w += weight * decay;
        w_sum += weight;
        if(file.eof()) break;
    }

    file.close();
    
    // Declaring the weighted average and its uncertainty.
    double decay_wav = dec_times_w / w_sum;
    double sigma_wav = 1 / TMath::Sqrt(w_sum);

    // Printing the lambda and half-life to the terminal.
    printf("Lambda: %f +- %f\n", decay_wav, sigma_wav);
    printf("Half-life: %f +- %f\n", TMath::Log(2)/decay_wav, sigma_wav/pow(decay_wav, 2));

}