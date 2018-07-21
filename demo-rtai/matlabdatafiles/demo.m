clear all;
close all;

Filename = 'GMatlabDataFile.mat';

t1 = ReadGMatlabDataFile('t1', Filename);
T1 = ReadGMatlabDataFile('T1', Filename);
Tsleep1 = ReadGMatlabDataFile('Tsleep1', Filename);
y1 = ReadGMatlabDataFile('y1', Filename);
t2 = ReadGMatlabDataFile('t2', Filename);
T2 = ReadGMatlabDataFile('T2', Filename);
y2 = ReadGMatlabDataFile('y2', Filename);

%N = min([size(t) size(texec) size(y1)]);

figure; 
subplot(211), plot(t1,y1); title('Variável y1');
subplot(212), plot(t2,y2); title('Variável y2');

figure; 
subplot(211), plot(t1(2:length(t1)),T1(2:length(t1))); title('Variável T1');
subplot(212), plot(t2(2:length(t2)),T2(2:length(t2))); title('Variável T2');

figure; 
subplot(211), plot(t1(2:length(t1)),Tsleep1(2:length(t1))); title('Variável Tsleep1');
