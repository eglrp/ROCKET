function [  ] = plotunk(  )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

filename='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/wl.unk';
trop = load(filename);
subplot(1,2,1)
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);
plot(trop(:,1)./30, trop(:,2),'k-.');
hold on;
plot(trop(:,1)./30, trop(:,3),'k-');
hold on;
plot(trop(:,1)./30, trop(:,4),'k--');
hold on
legend('total unknowns', 'fixed ambiguities', 'float unknowns');
grid on;
axis([0 2880 0 800])

x=0:100:800;     
y=0:240:2880;
set(gca,'ytick',x);
set(gca, 'xtick', y);


xlabel('Epoch number (30s sampling rate ) ');
ylabel('Number of parameters in WL UPD filter ')

% set(gcf, 'PaperUnits','centimeters','PaperSize', [9 7],'PaperPosition',[0 0 9 7]);
% set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
% set(gca, 'Fontsize', 6);
% 
% set(get(gca,'XLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','top');
% set(get(gca,'YLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','middle');

%saveas(gcf,strcat(filename,'.png'),'png') 

%%%
%%%
%%%

filename='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/nl.unk';
trop = load(filename);
subplot(1,2,2)
plot(trop(:,1)./30, trop(:,2),'k-.');
hold on;
plot(trop(:,1)./30, trop(:,3),'k-');
hold on;
plot(trop(:,1)./30, trop(:,4),'k--');
hold on
legend('total unknowns', 'fixed ambiguities', 'float unknowns');
grid on;
axis([0 2880 0 800])

x=0:100:800;     
y=0:240:2880;
set(gca,'ytick',x);
set(gca, 'xtick', y);


xlabel('Epoch number (30s sampling rate ) ');
ylabel('Number of parameters in NL UPD filter ')

set(gcf, 'PaperUnits','centimeters','PaperSize', [18 7],'PaperPosition',[0 0 18 7]);
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);

set(get(gca,'XLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','top');
set(get(gca,'YLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','middle');

saveas(gcf,strcat(filename,'.png'),'png') 

end


