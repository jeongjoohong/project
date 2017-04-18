close all;
clear;
clc;

Fs = 400;       % Sampling frequency
T = 1/Fs;       % Sampling period
L = 400;        % Length of signal
t = (0:L-1)*T;  % Time vector

width = 0.5;
freq1 = 50;
freq2 = 120;
amp1 = 0.7;
amp2 = 0.9;

S = amp1*sin(2*pi*freq1*t)+amp2*sin(2*pi*freq2*t);
X = S+randn(size(t));
figure('units','normalized','outerposition',[0 0 1 1]);
subplot(5,2,1);
plot(1000*t,X);
title('Signal with Noise');
xlabel('t (milliseconds)');
ylabel('X(t)');

Y = fftshift(fft(X,L));
fY = (-numel(Y)/2:numel(Y)/2-1);
subplot(5,2,2);
plot(fY, abs(Y));
title('FFT of X(t)');
xlabel('f (Hz)');
ylabel('|X(f)|');

A1 = sinc(width*(t-0.5)) .* cos(2*pi*freq1*t);
subplot(5,2,3);
plot(1000*t,A1);
title(sprintf('Sinc (%.1fHz, centered, shifted %dHz)',width,freq1));
xlabel('t (milliseconds)');
ylabel(sprintf('sinc(%.1f*(t-0.5)) * cos(2*pi*%d*t)',width,freq1));

A2 = sinc(width*(t-0.5)) .* cos(2*pi*freq2*t);
subplot(5,2,4);
plot(1000*t,A2);
title(sprintf('Sinc (%.1fHz, centered, shifted %dHz)',width,freq2));
xlabel('t (milliseconds)');
ylabel(sprintf('sinc(%.1f*(t-0.5)) * cos(2*pi*%d*t)',width,freq2));

B1 = fftshift(fft(A1,L));
fB1 = (-numel(B1)/2:numel(B1)/2-1);
subplot(5,2,5);
plot(fB1, abs(B1));
title('FFT of sinc(t)');
xlabel('f (Hz)');

B2 = fftshift(fft(A2,L));
fB2 = (-numel(B2)/2:numel(B2)/2-1);
subplot(5,2,6);
plot(fB2, abs(B2));
title('FFT of sinc(t)');
xlabel('f (Hz)');

C1 = ifft(ifftshift(B1),L);
subplot(5,2,7);
plot(1000*t,C1);
title('IFFT of sinc(f)');
xlabel('t (milliseconds)');

C2 = ifft(ifftshift(B2),L);
subplot(5,2,8);
plot(1000*t,C2);
title('IFFT of sinc(f)');
xlabel('t (milliseconds)');

D1 = conv(X,C1);
D2 = conv(X,C2);
E1 = [max(abs(D1)) max(abs(D2))];
E2 = [E1 ; E1*0];
subplot(5,1,5);
b = bar(E2, 'FaceColor', [0 0 1]);
title(sprintf('Compare with Convolution %dHz and %dHz',freq1,freq2));
ylabel('max(|conv(X(t), sinc(t))|)');
xlim([0.72 1.28]);
set(gca, 'XTick', []);
if max(abs(D1)) > max(abs(D2))
    b(1).LineWidth = 2;
    b(1).EdgeColor = 'red';
else
    b(2).LineWidth = 2;
    b(2).EdgeColor = 'red';
end
