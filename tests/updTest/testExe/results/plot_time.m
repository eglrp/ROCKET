function [  ] = plottime(  )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

filename='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/time.wl.txt';
time = load(filename);
subplot(1,2,1)
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);
plot(time(:,1), time(:,2),'k-o');
hold on;
plot(time(:,1), time(:,3),'k-s');
hold on;

legend('standard kalman filter', 'improved kalman filter');
legend BOXOFF
grid on;
axis([50 90 0 40])

x=50:20:90;     
y=0:10:40;
set(gca,'xtick',x);
set(gca, 'ytick', y);


xlabel('Number of stations ');
ylabel('CPU time per epoch of the WL UPD filter [Seconds] ')

% set(gcf, 'PaperUnits','centimeters','PaperSize', [9 7],'PaperPosition',[0 0 9 7]);
% set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
% set(gca, 'Fontsize', 6);
% 
% set(get(gca,'XLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','top');
% set(get(gca,'YLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','middle');
% 
% saveas(gcf,strcat(filename,'.png'),'png') 



filename='/Users/shjzhang/Documents/Develop/gpstk/workplace/upd/igs/1657/results/time.nl.txt';
time = load(filename);
subplot(1,2,2)
plot(time(:,1), time(:,2),'k-o');
hold on;
plot(time(:,1), time(:,3),'k-s');
hold on;

legend('standard kalman filter', 'improved kalman filter');
legend BOXOFF
grid on;
axis([50 90 0 40])

x=50:20:90;     
y=0:10:40;
set(gca,'xtick',x);
set(gca, 'ytick', y);

xlabel('Number of stations ');
ylabel('Number of parameters of the NL UPD filter ')

set(gcf, 'PaperUnits','centimeters','PaperSize', [18 7],'PaperPosition',[0 0 18 7]);
set(gca, 'Fontname', 'Arial', 'Fontsize', 6);
set(gca, 'Fontsize', 6);

set(get(gca,'XLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','top');
set(get(gca,'YLabel'),'Fontname', 'Arial','FontSize',7,'Vertical','middle');

saveas(gcf,strcat(filename,'.png'),'png') 

end


