function [ output_args ] = plotupd(  )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here


% subplot(2,1,1)
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);
filename1='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.wl.G01';
data1 = load(filename1);

filename2='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.wl.G02';
data2 = load(filename2);


filename3='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.wl.G03';
data3 = load(filename3);


filename4='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.wl.G04';
data4 = load(filename4);


filename5='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.wl.G05';
data5 = load(filename5);


filename6='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.wl.G06';
data6 = load(filename6);


filename7='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.wl.G07';
data7 = load(filename7);


filename8='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.wl.G08';
data8 = load(filename8);


filename9='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.wl.G09';
data9 = load(filename9);


filename10='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.wl.G25';
data10 = load(filename10);

plot(data1(:,1), data1(:,2), 'k-', data2(:,1), data2(:,2), 'b-', data3(:,1), data3(:,2), 'g-', ...
     data4(:,1), data4(:,2), 'r-', data5(:,1), data5(:,2), 'c-'); 




grid on;
axis([9 15 -4 4])

x=9:1:15;     
y=-4:1:4;
set(gca,'xtick',x);
set(gca, 'ytick', y);
legend('G01', 'G02','G03', 'G04','G05');
legend BOXOFF


xlabel('Epoch number (30s sampling rate ) ');
ylabel('WL UPD value [cycles] ')

set(gcf, 'PaperUnits','centimeters','PaperSize', [9 7],'PaperPosition',[0 0 9 7]);
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);

set(get(gca,'XLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','top');
set(get(gca,'YLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','middle');

saveas(gcf,'/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/upd.wl.outcheck.png','png') 


% subplot(2,1,2)
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);
filename1='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.nl.G01';
data1 = load(filename1);

filename2='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.nl.G02';
data2 = load(filename2);


filename3='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.nl.G03';
data3 = load(filename3);


filename4='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.nl.G04';
data4 = load(filename4);


filename5='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.nl.G05';
data5 = load(filename5);


filename6='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.nl.G06';
data6 = load(filename6);


filename7='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.nl.G07';
data7 = load(filename7);


filename8='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.nl.G08';
data8 = load(filename8);


filename9='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.nl.G09';
data9 = load(filename9);


filename10='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/bias.nl.G10';
data10 = load(filename10);

plot(data1(:,1), data1(:,2), 'b-.', data2(:,1), data2(:,2), 'b-', data3(:,1), data3(:,2), 'b--', ...
     data4(:,1), data4(:,2), 'g-.', data5(:,1), data5(:,2), 'g-', data6(:,1), data6(:,2), 'g--', ...
     data7(:,1), data7(:,2), 'r-.', data8(:,1), data8(:,2), 'r-', data9(:,1), data9(:,2), 'r--' ); 

xlabel('Epoch number (30s sampling rate ) ');
ylabel('NL UPD value [cycles] ')

legend('G01', 'G02','G03', 'G04','G05', 'G06','G07', 'G08','G09','Location','BestOutside');

grid on;
axis([9 15 -15 15])

x=9:1:14;     
y=-15:2:15;
set(gca,'xtick',x);
set(gca, 'ytick', y);

set(gcf, 'PaperUnits','centimeters','PaperSize', [18 6],'PaperPosition',[0 0 18 6]);
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);

set(get(gca,'XLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','top');
set(get(gca,'YLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','middle');

saveas(gcf,'/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/upd.nl.outcheck.png','png') 

end





