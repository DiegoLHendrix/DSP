function [impulseResponse,samples] = ChebyshevFilterBandpass(samplingFreq,rippleDb,filterOrder,lower_bpf_bsf_cornerFreq,upper_bpf_bsf_cornerFreq)
    N = 512;
    fs = 10;

    nyquistFreq = samplingFreq/2; % BPM folding frequency
    cornerFreq = [lower_bpf_bsf_cornerFreq, upper_bpf_bsf_cornerFreq] / nyquistFreq;

    [numerator, denominator] = cheby1(filterOrder, rippleDb, cornerFreq, 'bandpass');
    [impulseResponse, samples] = impz(numerator, denominator, N);

    % Find the magnitude of the impulse response
    CHEBYimpFFT = fft(impulseResponse);
    CHEBYimpMagnitude = 20 * log10(abs(CHEBYimpFFT));
    
    % Create frequency vector
    CHEBYfreqHz = (0:N-1) * (fs / N);  % Frequency in Hz
    CHEBYfreqBPM = CHEBYfreqHz * 60;  % Convert Hz to BPM
end
