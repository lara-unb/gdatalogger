clear all;
close all;

Filename = 'gmatlabdatafile.mat';

t  = ReadGMatlabDataFile('t', Filename);
y1 = ReadGMatlabDataFile('y1', Filename);
y2 = ReadGMatlabDataFile('y2', Filename);
y3 = ReadGMatlabDataFile('y3', Filename);

%N = min([size(t) size(texec) size(y1)]);

figure; 
subplot(311), plot(t,y1); title('Variavel y1');
subplot(312), plot(t,y2); title('Variavel y2');
subplot(313), plot(t,y3); title('Variavel y3');

figure; 
plot(diff(t)); title('Periodo de amostragem');
ylim([0 max(ylim)]);


whos
