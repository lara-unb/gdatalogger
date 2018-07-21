clear all;
close all;

for n=1:2
    n
    %%% Tarefa com periodicidade 10ms:
    if n==1,
        Filename = 'GMatlabDataFile_10ms.mat'; Tms = 10;
    end
    %%% Tarefa com periodicidade 1ms:
    if n==2,
        Filename = 'GMatlabDataFile_1ms.mat'; Tms = 1;
    end
    %%% Tarefa com periodicidade 100us:
    if n==3,
        Filename = 'GMatlabDataFile_100us.mat'; Tms = 0.1;
    end
    
    t  = ReadGMatlabDataFile('t', Filename);
    y1 = ReadGMatlabDataFile('y1', Filename);
    y2 = ReadGMatlabDataFile('y2', Filename);
    y3 = ReadGMatlabDataFile('y3', Filename);
    
    figure;
    subplot(311), plot(t(2:length(t)),diff(t)); title(sprintf('Tarefa com periodicidade %f ms: Periodo de amostragem medido',Tms));
    subplot(312), plot(t,y2); title('Tarefa com periodicidade 10ms: Variável y2');
    subplot(313), plot(t,y3); title('Tarefa com periodicidade 10ms: Variável y3');
end
