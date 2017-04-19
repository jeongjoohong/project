close all;
clear;
clc;

bandw = 0.1;
freq1 = 50;
freq2 = 120;
amp1 = 0.6;
amp2 = 0.8;
plotCol = 7;
plotInd = 0;

Fs = 400;       % Sampling frequency
T = 1/Fs;       % Sampling period
L = 400;        % Length of signal
t = (0:L-1)*T;  % Time vector

S = amp1*sin(2*pi*50*t)+amp2*sin(2*pi*120*t);
X = S+randn(size(t));

figure('units','normalized','outerposition',[0 0 1 1]);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(1000*t,X);
title('Signal with Noise');
xlabel('t (milliseconds)');
ylabel('X(t)');

Y = fftshift(fft(X,L));
fY = (-numel(Y)/2:numel(Y)/2-1);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(fY, abs(Y));
title('FFT of X(t)');
xlabel('f (Hz)');
ylabel('|X(f)|');

A1 = sinc(bandw*(t-0.5)) .* cos(2*pi*freq1*t);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(1000*t,A1);
title(sprintf('Sinc (%.1fHz, centered, shifted %dHz)',bandw,freq1));
xlabel('t (milliseconds)');
ylabel(sprintf('sinc(%.1f*(t-0.5)) * cos(2*pi*%d*t)',bandw,freq1));

A2 = sinc(bandw*(t-0.5)) .* cos(2*pi*freq2*t);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(1000*t,A2);
title(sprintf('Sinc (%.1fHz, centered, shifted %dHz)',bandw,freq2));
xlabel('t (milliseconds)');
ylabel(sprintf('sinc(%.1f*(t-0.5)) * cos(2*pi*%d*t)',bandw,freq2));

B1 = fftshift(fft(A1,L));
fB1 = (-numel(B1)/2:numel(B1)/2-1);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(fB1, abs(B1));
title('FFT of sinc(t)');
xlabel('f (Hz)');

B2 = fftshift(fft(A2,L));
fB2 = (-numel(B2)/2:numel(B2)/2-1);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(fB2, abs(B2));
title('FFT of sinc(t)');
xlabel('f (Hz)');

C1 = ifft(ifftshift(B1),L);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(1000*t,C1);
title('IFFT of sinc(f)');
xlabel('t (milliseconds)');

C2 = ifft(ifftshift(B2),L);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(1000*t,C2);
title('IFFT of sinc(f)');
xlabel('t (milliseconds)');

D1 = conv(X,C1);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(D1);
title('Convolution');

D2 = conv(X,C2);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(D2);
title('Convolution');

D1 = conv(X,C1);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(abs(D1));
title('Absolute of Convolution');

D2 = conv(X,C2);
plotInd = plotInd+1;
subplot(plotCol,2,plotInd);
plot(abs(D2));
title('Absolute of Convolution');

E = [max(abs(D1)) max(abs(D2))];
plotInd = plotInd+1;
subplot(plotCol,1,ceil(plotInd/2));
hold on
for i = 1 : length(E)
    b(i) = bar(i, E(i));
end
hold off
title(sprintf('Maximum Absolute Value of Convolution %dHz, and %dHz',freq1,freq2));
ylabel('max(|conv(X(t), sinc(t))|)');
set(gca, 'XTickLabel',E,'XTick',1:length(E))
if max(abs(D1)) > max(abs(D2))
    b(1).LineWidth = 2;
    b(1).EdgeColor = 'red';
else
    b(2).LineWidth = 2;
    b(2).EdgeColor = 'red';
end
