close all;
clear;
global CL CD S m g rho	
S		=	0.017;			% Reference Area, m^2
AR		=	0.86;			% Wing Aspect Ratio
e		=	0.9;			% Oswald Efficiency Factor;
m		=	0.003;			% Mass, kg
g		=	9.8;			% Gravitational acceleration, m/s^2
rho		=	1.225;			% Air density at Sea Level, kg/m^3	
CLa		=	3.141592 * AR/(1 + sqrt(1 + (AR / 2)^2));
						% Lift-Coefficient Slope, per rad
CDo		=	0.02;			% Zero-Lift Drag Coefficient
epsilon	=	1 / (3.141592 * e * AR);% Induced Drag Factor	
CL		=	sqrt(CDo / epsilon);	% CL for Maximum Lift/Drag Ratio
CD		=	CDo + epsilon * CL^2;	% Corresponding CD
LDmax	=	CL / CD;			% Maximum Lift/Drag Ratio
Gam		=	-atan(1 / LDmax);	% Corresponding Flight Path Angle, rad
V		=	sqrt(2 * m * g /(rho * S * (CL * cos(Gam) - CD * sin(Gam))));
						% Corresponding Velocity, m/s
Alpha	=	CL / CLa;			% Corresponding Angle of Attack, rad

%	a) Variation of Velocity
H		=	2;			% Initial Height, m
R		=	0;			% Initial Range, m
to		=	0;			% Initial Time, sec
tf		=	6;			% Final Time, sec
tspan	=	[to tf];
xo		=	[V;Gam;H;R]; % Nominal v
[ta,xa]	=	ode23('EqMotion',tspan,xo);
xo(1)   =   2; % v = 2
[ta2,xa2] = ode23('EqMotion',tspan,xo);
xo(1)   =   7.5; % v = 7.5
[ta3,xa3] = ode23('EqMotion',tspan,xo);

%	b) Variation of Flight Path Angle
xo		=	[V;Gam;H;R]; % Nominal gamma
[tb,xb]	=	ode23('EqMotion',tspan,xo);
xo(2)   =   -0.5; % gamma = -0.5
[tb2,xb2] = ode23('EqMotion',tspan,xo);
xo(2)   =   0.4; % gamma = 0.4
[tb3,xb3] = ode23('EqMotion',tspan,xo);

figure
subplot(2,1,1)
hold on
plot(xa(:,4),xa(:,3), Color="black")
plot(xa2(:,4),xa2(:,3), Color="red")
plot(xa3(:,4),xa3(:,3), Color="green")
title("Height vs. Range With Velocity Variation")
xlabel("Range (m)")
ylabel("Height (m)")
ylim([0, 4])
legend("v_0 = 3.55 m/s (Nominal)", "v_0 = 2 m/s", "v_0 = 7.5 m/s")

subplot(2,1,2)
hold on
plot(xb(:,4),xb(:,3), Color="black")
plot(xb2(:,4),xb2(:,3), Color="red")
plot(xb3(:,4),xb3(:,3), Color="green")
title("Height vs. Range With Flight Path Angle Variation")
xlabel("Range (m)")
ylabel("Height (m)")
ylim([0, 2.5])
legend("\gamma_0 = -0.18 rad (Nominal)", "\gamma_0 = -0.4 rad", "\gamma_0 = 0.5 rad")

% Parameters
num_simulations = 100;
num_colors = 256;  % Color map resolution
color_map = [linspace(0,1,num_colors)', linspace(1,0,num_colors)', zeros(num_colors,1)];
simulation_data = cell(num_simulations, 1);  % Cell array to store trajectory data
max_range = zeros(num_simulations, 1);  % Array to store max heights of each simulation


% Run simulations
for i = 1:num_simulations
    initial_velocity = 2 + (7.5 - 2) * rand(1);
    initial_gamma = -0.5 + (0.4 + 0.5) * rand(1);
    xo = [initial_velocity, initial_gamma, H, R];
    [ta, xa] = ode23('EqMotion', tspan, xo);  
    simulation_data{i} = [xa(:, [4, 3]), ta];  % Store range and height data
    max_range(i) = xa(find(xa(:,3)<0,1),4); % Setting max range equal to where the height crosses 0
end

% Normalizing max heights for color indexing
normalized_ranges = (max_range - min(max_range)) / (max(max_range) - min(max_range));
color_indices = max(1, min(num_colors, round(normalized_ranges * (num_colors - 1)) + 1));

dt = 0.01; % time step
tPoly = 0:dt:6; % time vector used for polynomial fits

ts = [];
ranges = [];
heights = [];

for i = 1:num_simulations
    ts = cat(1, ts, simulation_data{i}(:, 3));
    ranges = cat(1, ranges, simulation_data{i}(:, 1));
    heights = cat(1, heights, simulation_data{i}(:, 2));
end

% Polynomial fits
pRanges = polyfit(ts, ranges, 6);
pHeights = polyfit(ts, heights, 6);

% Evaluated polynomials
fitRanges = polyval(pRanges, tPoly);
fitHeights = polyval(pHeights, tPoly);



% Plotting
figure;
hold on;
for i = 1:num_simulations
    plot_color = color_map(color_indices(i), :);  % Color based on max height
    plot(simulation_data{i}(:, 1), simulation_data{i}(:, 2), 'Color', plot_color);  % Plot each simulation
end
p = plot(fitRanges, fitHeights, LineWidth=2, Color="black");
legend(p, "6th degree polynomial fit")

title('Height vs. Range');
xlabel('Range (m)');
ylabel('Height (m)');
legend()
colormap(color_map);
x = colorbar;
clim([min(max_range), max(max_range)]) % setting color bar limits to minimum and maximum ranges
ylabel(x, 'Max Range (m)');
ylim([0,4])
grid on;

figure
hold on
plot(ts, ranges, '*')
p = plot(tPoly, fitRanges, LineWidth=2);
title('Range vs. Time');
xlabel("Time (s)")
ylabel("Range (m)")
legend(p, "6th degree polynomial fit")

figure
hold on
plot(ts, heights, '*')
p = plot(tPoly, fitHeights, LineWidth=2, DisplayName="6th degree polynomial fit");
title('Height vs. Time');
xlabel("Time (s)")
ylabel("Range (m)")
legend(p, "6th degree polynomial fit")

% Forward differences
dRanges = diff(fitRanges)/dt; % Time derivative of range (horizontal velocity)
dHeights = diff(fitHeights)/dt; % Time derivative of height (horizontal velocity)

figure
subplot(2, 1, 1)
plot(tPoly(1:end-1), dRanges)
title("Horizontal Velocity vs. Time")
xlabel("Time (s)")
ylabel("Horizontal velocity (m/s)")

subplot(2, 1, 2)
plot(tPoly(1:end-1), dHeights)
title("Vertical Velocity vs. Time")
xlabel("Time (s)")
ylabel("Vertical velocity (m/s)")

