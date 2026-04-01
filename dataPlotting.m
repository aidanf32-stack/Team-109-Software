%% Testing 
clear; clc; close all

sp = serialport("COM4", 115200); % reads data from serial connection
configureTerminator(sp,"LF"); % sets terminator to LF

% assume data comes as time, pressure, Tp1 - 6, Tin, Tout

figure % initializes figure

sp1 = subplot(2,2,1); % 1st subplot --> mean panel temp
hold(sp1, "on");
MPT = plot(sp1, nan, nan);
xlabel(sp1, 'Time (s)');
ylabel(sp1, 'Temperature (°C)');
title(sp1, 'Average Internal Panel Temperature');
hold(sp1, "off");

sp2 = subplot(2,2,2); % 2nd subplot --> internal/external ambient temps
hold(sp2, "on");
To = plot(sp2, nan, nan); % external
Ti = plot(sp2, nan, nan); % internal
xlabel(sp2, 'Time (s)');
ylabel(sp2, 'Temperature (°C)');
title(sp2, 'Internal and External Ambient Temperature');
legend(sp2, 'External', 'Internal');
hold(sp2, "off");

sp3 = subplot(2,2,3); % 3rd subplot --> pressure
hold(sp3, "on");
Pr = plot(sp3, nan, nan);
xlabel(sp3, 'Time (s)');
ylabel(sp3, 'Pressure (Pa)');
title(sp3, 'Internal Ambient Pressure');
hold(sp3, "off");

sp4 = subplot(2,2,4); % 4th subplot --> temp time derivative
hold(sp4, "on");
Tdot = plot(sp4, nan, nan);
xlabel(sp4, 'Time (s)');
ylabel(sp4, 'dT/dt (°C/s)');
title(sp4, 'Time Rate of Change of Internal Ambient Temperature');
hold(sp4, "off");

% placeholder vectors (updated continuously in loop)

data = [];
timestamp = []; % elapsed time (s)
pressure = []; % enclosure ambient temp ([UNIT TBD])
panel1_temp = []; % panel temp (deg C)
panel2_temp = []; % panel temp (deg C)
panel3_temp = []; % panel temp (deg C)
panel4_temp = []; % panel temp (deg C)
panel5_temp = []; % panel temp (deg C)
panel6_temp = []; % panel temp (deg C)
T_intamb = []; % internal ambient temp (deg C)
T_outamb = []; % external ambient temp (deg C)
mean_paneltemp = []; % average panel temp (deg C)
dTdt = []; % temp time derivative (deg C / s)

while true % infinite loop, runs as long as data comes in

    line = readline(sp); % reads a single line from serial connection
    itr = str2double(split(line, ",")); % converts line to numerical values
    data = [data str2double(split(line, ","))]; % converts line to numerical values stored in a matrix

    % update variables with latest iteration
    timestamp(end+1) = itr(1);
    pressure(end+1) = itr(2);
    panel1_temp(end+1) = itr(3);
    panel2_temp(end+1) = itr(4);
    panel3_temp(end+1) = itr(5);
    panel4_temp(end+1) = itr(6);
    panel5_temp(end+1) = itr(7);
    panel6_temp(end+1) = itr(8);
    T_intamb(end+1) = itr(9);
    T_outamb(end+1) = itr(10);
    mean_paneltemp(end+1) = sum(itr(3:8))/6; % mean temp calculation

    % time derivative using a backwards difference approximation
    if length(timestamp) > 1
        dTdt(end+1) = (T_intamb(end) - T_intamb(end-1))/(timestamp(end) - timestamp(end-1));
    % for initial data point w/ no 'backward' instance available
    else
        dTdt = 1;
    end

    % update plots with new data
    set(MPT, "XData", timestamp, "YData", mean_paneltemp); % subplot 1
    set(To, "XData", timestamp, "YData", T_outamb); % subplot 2
    set(Ti, "XData", timestamp, "YData", T_intamb); % subplot 2
    set(Pr, "XData", timestamp, "YData", pressure); % subplot 3
    set(Tdot, "XData", timestamp, "YData", dTdt); % subplot 4

    drawnow limitrate % display

    writematrix(data, "test_3_data.csv"); 

end

%% Post Processing
clear; clc; close all

ppd = readmatrix('3_31_Rising_data.csv');

t = ppd(1,:)./1000;
p = ppd(2,:);
T_p1 = ppd(3,:);
T_p2 = ppd(4,:);
T_p3 = ppd(5,:);
T_p4 = ppd(6,:);
T_p5 = ppd(7,:);
T_p6 = ppd(8,:);
T_p_avg = (T_p1 + T_p2 + T_p3 + T_p4 + T_p5 + T_p6)/6;
T_int = ppd(9,:);
T_ext = ppd(10,:);

subplot(2,2,1);
hold on
plot(t,T_p_avg);
xlabel('Time (s)');
ylabel('Temperature (degrees C)');
title('Average Panel Temperature During Testing');
hold off

subplot(2,2,2);
hold on
plot(t,T_int);
plot(t,T_ext);
xlabel('Time (s)');
ylabel('Temperature (degrees C)');
legend('Internal Ambient', 'External Ambient',Location='southoutside');
title('Ambient Temperature During Testing');
hold off

subplot(2,2,3);
hold on
plot(t,p);
xlabel('Time (s)');
ylabel('Pressure (psi)');
title('Enclosure Ambient Pressure During Testing');
hold off

dTdt = [];
for i = 1:(length(t)-1)
    if i == 1
        dTdt(i) = 1;
    else
        dTdt(i) = (T_int(i+1) - T_int(i))/(t(i+1) - t(i));
    end
end

dTdt(end+1) = dTdt(end);

subplot(2,2,4);
hold on
plot(t,dTdt);
xlabel('Time (s)');
ylabel('dT/dt (degrees C / s)');
title('Temperature Time Derivative During Testing');
hold off


b = [t;dTdt];