clc 
clear all
close all
addpath('/MATLAB Drive/Datos');
addpath('/MATLAB Drive/');
%% Importar Archivos
archivos=4;
for i=1: archivos
 datos=readmatrix(strcat('est',num2str(i),'.csv'));
 v=genvarname(['estacion',num2str(i)]);
 eval([v, '=datos;']);
 end

%% Promedio artismetico
Promedio_aritmetico=zeros(archivos,9);
for num =1:4
    eval(['estacion', '=estacion',num2str(num),';']);
    for indice=1:9
        Promedio_aritmetico(num,indice)=mean(estacion(:,indice),'omitnan');
    end
end
disp("--------------------- Promedio aritmetico ---------------------")
tabla1=table(Promedio_aritmetico(:,1),Promedio_aritmetico(:,2),Promedio_aritmetico(:,3),...
    Promedio_aritmetico(:,4),Promedio_aritmetico(:,5),Promedio_aritmetico(:,6),...
    Promedio_aritmetico(:,7),Promedio_aritmetico(:,8),Promedio_aritmetico(:,9),...
    'VariableNames',{'Presion';'Temperatura';'Humedad';'PM 2.5';'PM 10';'CO';'CH4';'CO2';'O3'});
disp(tabla1)

%% Promedio Geometrico
Promedio_geometrico=zeros(archivos,9);
for num =1:4
    eval(['estacion', '=estacion',num2str(num),';']);
    for indice=1:9
        Promedio_geometrico(num,indice)=geomean(estacion(:,indice),'omitnan');
    end
end
disp("--------------------- Promedio geometrico ---------------------")
tabla2=table(Promedio_geometrico(:,1),Promedio_geometrico(:,2),Promedio_geometrico(:,3),...
    Promedio_geometrico(:,4),Promedio_geometrico(:,5),Promedio_geometrico(:,6),...
    Promedio_geometrico(:,7),Promedio_geometrico(:,8),Promedio_geometrico(:,9),...
    'VariableNames',{'Presion';'Temperatura';'Humedad';'PM 2.5';'PM 10';'CO';'CH4';'CO2';'O3'});
disp(tabla2)

%% Promedios de maximos por dia
Promedio_max=zeros(archivos,9);
dia=96; %muesras por dia
for num =1:4
    eval(['estacion', '=estacion',num2str(num),';']);
    for indice=1:9
        Promedio_max(num,indice)=Maxpordia(estacion(:,indice),dia);
    end
end
disp("--------------------- Promedio maximos Por dia  ---------------------")
tabla2=table(Promedio_max(:,1),Promedio_max(:,2),Promedio_max(:,3),...
    Promedio_max(:,4),Promedio_max(:,5),Promedio_max(:,6),...
    Promedio_max(:,7),Promedio_max(:,8),Promedio_max(:,9),...
    'VariableNames',{'Presion';'Temperatura';'Humedad';'PM 2.5';'PM 10';'CO';'CH4';'CO2';'O3'});
disp(tabla2)

%% Graficas de la de las variables
recorte=[1,2,1,35];
m=3;
inicio1="Tiempo transcurrido desde 2021/6/25-10:48:45 a.m (horas)";
inicio2=  "Tiempo transcurrido desde 2021/6/9-0:25:49 a.m (horas)";
inicio3=  "Tiempo transcurrido desde 2021/6/27-3:11:10 p.m (horas)";
inicio4=  'Tiempo transcurrido desde 2021/6/26-4:19:6 p.m (horas)';
% se grafica datos obtenidos de las cuatro estaciones cada hora
figure('Name','temperatura','Color','w')
grid on
hold on
%  ----- GRAFICA DE TEMPERATURA---------
for i=1:4
eval(['estacion', '=estacion',num2str(i),';']);
subplot(4,1,i)
eval(['p',num2str(i),'=plot(promedios(estacion(recorte(i):end,2),3));'])
eval(['xlabel(inicio',num2str(i),')'])
ax = gca;
ax.FontSize = 8;
title('Temperatura vs tiempo','FontSize',13)
xlim([0 350])
ylim([10 30])
ylabel('°C')
legend(['Estación ',num2str(i)])
grid on
end

p1.LineWidth=2;
p3.LineWidth=2;
p2.LineWidth=2;
p4.LineWidth=2;
p1.Color='k';
p2.Color='k';
p3.Color='k';
p4.Color='k';



%  ----- GRAFICAS DE MATERIAL PARTICULADO---------


for i=1:4
titulo=['Material Particulado ', num2str(i)];
figure('Name',titulo)
grid on
hold on
eval(['estacion', '=estacion',num2str(i),';']);
p1=plot(promedios(estacion(recorte(i):end,5),m));
p2=plot(promedios(estacion(recorte(i):end,4),m));
p1.LineWidth=1.5;
p2.LineWidth=2;
p1.LineStyle='--';
p1.Color='#808080';
p2.Color='k';
xlabel('Tiempo transcurrido desde 27/06/2021 (horas)')
ylabel('PM (ug/m3)')
title('Material particulado vs tiempo')
legend('PM 10','PM 2.5' )

end
%  ----- GRAFICA DE HUMEDAD---------
figure('Name','humedad')
grid on
hold on
for i=1:4
eval(['estacion', '=estacion',num2str(i),';']);
subplot(4,1,i)
eval(['p',num2str(i),'=plot(promedios(estacion(recorte(i):end,3),3));'])
eval(['xlabel(inicio',num2str(i),')'])
ax = gca;
ax.FontSize = 8;
title('Humedad relativa vs tiempo','FontSize',13)
xlim([0 350])
ylim([20 80])
ylabel('% RH')
legend(['Estación ',num2str(i)])
grid on
end

p1.LineWidth=2;
p3.LineWidth=2;
p2.LineWidth=2;
p4.LineWidth=2;
p1.Color='k';
p2.Color='k';
p3.Color='k';
p4.Color='k';
%  ----- GRAFICA DE PRESION--------- 
figure('Name','presión')
grid on
hold on
for i=1:4
eval(['estacion', '=estacion',num2str(i),';']);
subplot(4,1,i)
eval(['p',num2str(i),'=plot(promedios(estacion(recorte(i):end,1),3));'])
eval(['xlabel(inicio',num2str(i),')'])
ax = gca;
ax.FontSize = 8;
title('Presión atmosférica vs tiempo','FontSize',13)
xlim([0 350])
%ylim([20 80])
ylabel('mm Hg')
legend(['Estación ',num2str(i)])
grid on
end

p1.LineWidth=2;
p3.LineWidth=2;
p2.LineWidth=2;
p4.LineWidth=2;
p1.Color='k';
p2.Color='k';
p3.Color='k';
p4.Color='k';
%  ----- GRAFICA DE CO--------- 
figure('Name','CO')
grid on
hold on
for i=1:4
eval(['estacion', '=estacion',num2str(i),';']);
subplot(4,1,i)
eval(['p',num2str(i),'=plot(promedios(estacion(recorte(i):end,6),3));'])
eval(['xlabel(inicio',num2str(i),')'])
ax = gca;
ax.FontSize = 8;
title('CO vs tiempo','FontSize',13)
xlim([0 350])
%ylim([20 80])
ylabel('ppm')
legend(['Estación ',num2str(i)])
grid on
end

p1.LineWidth=2;
p3.LineWidth=2;
p2.LineWidth=2;
p4.LineWidth=2;
p1.Color='k';
p2.Color='k';
p3.Color='k';
p4.Color='k';
%  ----- GRAFICA DE CH4--------- 
figure('Name','CH4')
grid on
hold on
for i=1:4
eval(['estacion', '=estacion',num2str(i),';']);
subplot(4,1,i)
eval(['p',num2str(i),'=plot(promedios(estacion(recorte(i):end,7),3));'])
eval(['xlabel(inicio',num2str(i),')'])
ax = gca;
ax.FontSize = 8;
title('CH4 vs tiempo','FontSize',13)
xlim([0 350])
ylim([10 45])
ylabel('ppm')
legend(['Estación ',num2str(i)])
grid on
end

p1.LineWidth=2;
p3.LineWidth=2;
p2.LineWidth=2;
p4.LineWidth=2;
p1.Color='k';
p2.Color='k';
p3.Color='k';
p4.Color='k';

%  ----- GRAFICA DE CO2--------- 
figure('Name','CO2')
grid on
hold on
for i=1:4
eval(['estacion', '=estacion',num2str(i),';']);
subplot(4,1,i)
eval(['p',num2str(i),'=plot(promedios(estacion(recorte(i):end,8),3));'])
eval(['xlabel(inicio',num2str(i),')'])
ax = gca;
ax.FontSize = 8;
title('CO2 vs tiempo','FontSize',13)
xlim([0 350])
%ylim([10 45])
ylabel('ppm CO2')
legend(['Estación ',num2str(i)])
grid on
end

p1.LineWidth=2;
p3.LineWidth=2;
p2.LineWidth=2;
p4.LineWidth=2;
p1.Color='k';
p2.Color='k';
p3.Color='k';
p4.Color='k';

figure('Name','03')
grid on
hold on
for i=1:4
eval(['estacion', '=estacion',num2str(i),';']);
subplot(4,1,i)
eval(['p',num2str(i),'=plot(promedios(estacion(recorte(i):end,9),3));'])
eval(['xlabel(inicio',num2str(i),')'])
ax = gca;
ax.FontSize = 8;
title('O3 vs tiempo','FontSize',13)
xlim([0 350])
%ylim([10 45])
ylabel('ppm CO2')
legend(['Estación ',num2str(i)])
grid on
end

p1.LineWidth=2;
p3.LineWidth=2;
p2.LineWidth=2;
p4.LineWidth=2;
p1.Color='k';
p2.Color='k';
p3.Color='k';
p4.Color='k';
%% funciones
function ProMovil = promedios(Datos,N)
    L=length(Datos);
    salida=0;
    cont=1;
    for i=1:N:L-N
        salida(cont)=mean(Datos(i:i+N),'omitnan');
        cont=cont+1;
    end
    ProMovil=salida;
end 
function ProMax = Maxpordia(Datos,N)
    L=length(Datos);
    salida=0;
    cont=1;
    for i=1:N:L-N
        salida(cont)=max(Datos(i:i+N));
        cont=cont+1;
    end
    ProMax=mean(salida);
end 


