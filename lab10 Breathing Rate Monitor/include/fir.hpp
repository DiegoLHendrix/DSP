namespace fir {
//**********************************************************************
long EqualizerFIR(long xInput, int sampleNumber) {
    int i;
    long yN = 0;  //  Current output
    const int equalizerLength = 4;
    static long xN[equalizerLength] = {0};
    long h[] = {1, 1, -1, -1};

    for (i = equalizerLength - 1; i >= 1; i--) {
        xN[i] = xN[i - 1];
    }

    xN[0] = xInput;

    for (i = 0; i <= equalizerLength - 1; i++) {
        yN += h[i] * xN[i];
    }

    if (sampleNumber < equalizerLength) {
        return 0;
    } else {
        return yN;
    }
}

//*******************************************************************
int NoiseFilter(long xInput, int sampleNumber) {
    // Filter Type: LPF
    const int Fc_bpm = 65;
    // LPF FIR Filter Coefficients MFILT = 61, Fc = 50
    const int HFXPT = 4096, MFILT = 61;
    int h[] = {0, 2, 4, 5, 5, 4, 0, -6, -12, -17, -17, -12, 0, 17,
               35, 47, 47, 32, 0, -43, -87, -117, -119, -82, 0, 122, 270, 424,
               558, 649, 681, 649, 558, 424, 270, 122, 0, -82, -119, -117, -87, -43,
               0, 32, 47, 47, 35, 17, 0, -12, -17, -17, -12, -6, 0, 4,
               5, 5, 4, 2, 0};

    const float INV_HFXPT = 1.0 / HFXPT;
    static long xN[MFILT] = {xInput};
    long hv, accum = 0;

    // Right shift old xv values. Install new x in xv[0];
    for (int i = (MFILT - 1); i > 0; i--) xN[i] = xN[i - 1];
    xN[0] = xInput;

    // h[]*x[] overlap multiply-accumumlate
    for (int i = 0; i < MFILT; i++) {
        hv = h[i];  // create 32 bit space
        accum += hv * xN[MFILT - 1 - i];
    }
    return (accum * INV_HFXPT);
}

}  // namespace fir