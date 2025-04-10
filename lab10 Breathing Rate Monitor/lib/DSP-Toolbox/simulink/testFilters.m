clc;clear;close all

fs = 10; 
filterType = 'bandpass';
rippleDb = 1;
filterOrder = 5;
f1 = 12 / 60; % 12 * (1/60)
f2 = 25 / 60; % 25 * (1/60)

[impulseResponse, samples] = ChebyshevFilterBandpass(fs, rippleDb, filterOrder, f1, f2);

N = length(samples);

% Plot the impulse response manually
figure;
plot(samples, impulseResponse);
xlabel('Sample Number');
ylabel('Magnitude');
title('Impulse Response of the IIR Bandpass Filter');
grid on;

% Find the magnitude of the impulse response
CHEBYEimpFFT = fft(impulseResponse);
CHEBYEimpMagnitude = 20 * log10(abs(CHEBYEimpFFT));

% Create frequency vector
CHEBYEfreqHz = (0:N-1) * (fs / N);  % Frequency in Hz
CHEBYEfreqBPM = CHEBYEfreqHz * 60;  % Convert Hz to BPM

figure;
plot(CHEBYEfreqBPM, CHEBYEimpMagnitude);
xlim([0 50])
xlabel('Frequency (BPM)');
ylabel('Magnitude (dB)');
title('Frequency Response of the IIR Bandpass Filter (0–50 BPM)');
grid on;