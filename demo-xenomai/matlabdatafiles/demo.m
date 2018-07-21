clear all;
close all;

Filename = 'gmatlabdatafile.mat';

t1 = ReadGMatlabDataFile('t1', Filename);
T1 = ReadGMatlabDataFile('T1', Filename);
Tsleep1 = ReadGMatlabDataFile('Tsleep1', Filename);
Tsleep2 = ReadGMatlabDataFile('Tsleep2', Filename);
y1 = ReadGMatlabDataFile('y1', Filename);
t2 = ReadGMatlabDataFile('t2', Filename);
T2 = ReadGMatlabDataFile('T2', Filename);
y2 = ReadGMatlabDataFile('y2', Filename);

MatA = ReadGMatlabDataFile('MatA', Filename);
MatB = ReadGMatlabDataFile('MatB', Filename);

%N = min([size(t) size(texec) size(y1)]);

figure; 
subplot(211), plot(t1,y1); title('Variavel y1');
subplot(212), plot(t2,y2); title('Variavel y2');

figure; 
subplot(211), plot(t1(2:length(t1)),T1(2:length(t1))); title('Variavel T1');
subplot(212), plot(t2(2:length(t2)),T2(2:length(t2))); title('Variavel T2');

figure; 
subplot(211), plot(t1(2:length(t1)),Tsleep1(2:length(t1))); title('Variavel Tsleep1');
subplot(212), plot(t1(2:length(t1)),Tsleep2(2:length(t1))); title('Variavel Tsleep2');

figure;
for i=1:length(t1)    x(i) = MatA(1,1,i); end
subplot(311), plot(t1,x); title('Variavel MatA(1,1)');
for i=1:length(t1)    x(i) = MatA(2,1,i); end
subplot(312), plot(t1,x); title('Variavel MatA(2,1)');
for i=1:length(t1)    x(i) = MatA(3,1,i); end
subplot(313), plot(t1,x); title('Variavel MatA(3,1)');

whos
