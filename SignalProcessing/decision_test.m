close all;
clear;
clc;

%% Variables
Ls = 800;           % Length of signal
Lf = 300;           % Length of filter
Fs = 100;           % Sampling frequency
T = 1 / Fs;         % Sampling period
ts = (0:Ls-1) * T;  % Time vector of signal
tf = (0:Lf-1) * T;  % Time vector of filter

Freq1 = 7;
Freq2 = 8;
Amp1 = 0.7;
Amp2 = 0.6;
AmpN = 0.01;
bandw = 1;
freq1 = Freq1;
freq2 = Freq2;

plotCol = 5;
plotInd = 0;

%% Signal generation
S1 = Amp1 * sin(2 * pi * Freq1 * ts);
S2 = Amp2 * sin(2 * pi * Freq2 * ts);
S = S1 + S2;
% X = S + AmpN * randn(size(ts));
X = S;

figure('units', 'normalized', 'outerposition', [0 0 1 1]);
plotInd = plotInd + 1;
subplot(plotCol, 2, plotInd);
plot(ts * 1e3, X);
title(sprintf('Signal : %.1f*sin(2*pi*%d*t) + %.1f*sin(2*pi*%d*ts)', Amp1, Freq1, Amp2, Freq2));
xlabel('t (milliseconds)');
ylabel('X(t)');

%% FFT of received signal
Y = fftshift(fft(X(1:Lf), Lf));
f = linspace(-Fs / 2, Fs / 2 - Fs / Lf, length(Y));
plotInd = plotInd + 1;
subplot(plotCol, 2, plotInd);
plot(f, abs(Y));
title('FFT of signal');
xlabel('f (Hz)');
ylabel('|X(f)|');

%% Sinc filters
A1 = sinc(bandw * (tf - (Lf/Fs/2))) .* cos(2 * pi * freq1 * tf);
A1 = A1 ./ sum(abs(A1) .^ 2);   % Normalization
plotInd = plotInd + 1;
subplot(plotCol, 2, plotInd);
plot(tf * 1e3, A1);
title(sprintf('Sinc1 : %.1fHz bandwidth, centered, shifted %dHz', bandw, freq1));
xlabel('t (milliseconds)');

A2 = sinc(bandw * (tf - (Lf/Fs/2))) .* cos(2 * pi * freq2 * tf);
A2 = A2 ./ sum(abs(A2) .^ 2);   % Normalization
plotInd = plotInd + 1;
subplot(plotCol, 2, plotInd);
plot(tf * 1e3, A2);
title(sprintf('Sinc2 : %.1fHz bandwidth, centered, shifted %dHz', bandw, freq2));
xlabel('t (milliseconds)');

%% Examination of sinc filters using FFT and IFFT
test1FFT = fftshift(fft(A1, Lf));
% test1FVec = -numel(test1FFT)/2:numel(test1FFT)/2-1;
plotInd = plotInd + 1;
subplot(plotCol, 2, plotInd);
% plot(test1FVec, abs(test1FFT));
plot(f, abs(test1FFT));
line([Freq1 Freq1 ; -Freq1 -Freq1]', [get(gca, 'YLim') ; get(gca, 'YLim')]', 'Color', 'red', 'LineStyle', '--');
title('FFT of sinc1');
xlabel('f (Hz)');

test2FFT = fftshift(fft(A2, Lf));
% test2FVec = -numel(test2FFT)/2:numel(test2FFT)/2-1;
plotInd = plotInd + 1;
subplot(plotCol, 2, plotInd);
% plot(test2FVec, abs(test2FFT));
plot(f, abs(test2FFT));
line([Freq2 Freq2 ; -Freq2 -Freq2]', [get(gca, 'YLim') ; get(gca, 'YLim')]', 'Color', 'red', 'LineStyle', '--');
title('FFT of sinc2');
xlabel('f (Hz)');

%% Convolution
B1 = conv(X(1:Lf), A1, 'same');
C1 = convolution_test(X(1:Lf), A1, 'same');
plotInd = plotInd + 1;
subplot(plotCol, 2, plotInd);
plot(tf * 1e3, B1, tf * 1e3, C1, '-.');
legend('matlab function', 'test function', 'Location', 'southeast');
title('Convolution of signal and sinc1');
xlabel('t (milliseconds)');

B2 = conv(X(1:Lf), A2, 'same');
C2 = convolution_test(X(1:Lf), A2, 'same');
plotInd = plotInd + 1;
subplot(plotCol, 2, plotInd);
plot(tf * 1e3, B2, tf * 1e3, C2, '-.');
legend('matlab function', 'test function', 'Location', 'southeast');
title('Convolution of signal and sinc2');
xlabel('t (milliseconds)');

%% Comparison of powers
Pwr_S1 = sum(S1(1:Lf) .^ 2);
Pwr_S2 = sum(S2(1:Lf) .^ 2);
Pwr_B1 = sum(B1 .^ 2);
Pwr_B2 = sum(B2 .^ 2);
Pwr_C1 = sum(C1 .^ 2);
Pwr_C2 = sum(C2 .^ 2);
Pwr = [Pwr_S1 Pwr_B1 Pwr_C1 ; Pwr_S2 Pwr_B2 Pwr_C2];
plotInd = plotInd + 1;
subplot(plotCol, 2, plotInd);
bar(Pwr);
set(gca, 'XTickLabel', {sprintf('%dHz', freq1), sprintf('%dHz', freq2)});
title('Comparison of signal powers');
legend('source', 'matlab function', 'test function', 'Location', 'southeast');

%% Loss powers of signal
LossPwr_B1 = (1 - Pwr_B1 / Pwr_S1) * 100;   % Loss of signal (%)
LossPwr_B2 = (1 - Pwr_B2 / Pwr_S2) * 100;   % Loss of signal (%)
LossPwr_C1 = (1 - Pwr_C1 / Pwr_S1) * 100;   % Loss of signal (%)
LossPwr_C2 = (1 - Pwr_C2 / Pwr_S2) * 100;   % Loss of signal (%)
Loss = [LossPwr_B1 LossPwr_C1 ; LossPwr_B2 LossPwr_C2];
plotInd = plotInd + 1;
subplot(plotCol, 2, plotInd);
bar(Loss);
set(gca, 'XTickLabel', {sprintf('%dHz', freq1), sprintf('%dHz', freq2)});
title('Loss powers of signal (%)');
legend('matlab function', 'test function');
ylabel('percentage');
ylim([0 100]);
