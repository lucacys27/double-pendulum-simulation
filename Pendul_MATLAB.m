g = 9.81; m1 = 1; m2 = 1; L1 = 1; L2 = 1;

f_pendul = @(t, x) [
    x(3);
    x(4);
    (-g*(2*m1 + m2)*sin(x(1)) - m2*g*sin(x(1) - 2*x(2)) - 2*sin(x(1) - x(2))*m2*(x(4)^2*L2 + x(3)^2*L1*cos(x(1) - x(2)))) / (L1 * (2*m1 + m2 - m2*cos(2*x(1) - 2*x(2))));
    (2*sin(x(1) - x(2))*(x(3)^2*L1*(m1 + m2) + g*(m1 + m2)*cos(x(1)) + x(4)^2*L2*m2*cos(x(1) - x(2)))) / (L2 * (2*m1 + m2 - m2*cos(2*x(1) - 2*x(2))))
];

h = 0.01; 
t = (0 : h : 15)'; 
N = length(t);
M = 50; 

vx0 = zeros(4, M);
vx0(:, 1) = [2*pi/3; 2*pi/3; 0; 0];
for k = 1 : M - 1
    vx0(:, k+1) = vx0(:, k) + [0; 1e-4; 0; 0];
end

X1 = zeros(N, M); Y1 = zeros(N, M);
X2 = zeros(N, M); Y2 = zeros(N, M);

for i = 1 : M
    x_rk4 = zeros(4, N);
    x_rk4(:, 1) = vx0(:, i);
    for k = 1 : N - 1
        xn = x_rk4(:, k); 
        tn = t(k);
        k1 = f_pendul(tn, xn);
        k2 = f_pendul(tn + h/2, xn + h*k1/2);
        k3 = f_pendul(tn + h/2, xn + h*k2/2);
        k4 = f_pendul(tn + h, xn + h*k3);
        x_rk4(:, k+1) = xn + (h/6)*(k1 + 2*k2 + 2*k3 + k4);
    end
    X1(:, i) = L1 * sin(x_rk4(1, :)');
    Y1(:, i) = -L1 * cos(x_rk4(1, :)');
    X2(:, i) = X1(:, i) + L2 * sin(x_rk4(2, :)');
    Y2(:, i) = Y1(:, i) - L2 * cos(x_rk4(2, :)');
end

fig = figure('Name', 'Efectul Fluturelui', 'Color', 'k');
ax = axes('Parent', fig, 'Color', 'k', 'XColor', 'w', 'YColor', 'w');
hold(ax, 'on'); axis(ax, 'equal'); grid(ax, 'on');
xlim(ax, [-(L1+L2)-0.5, (L1+L2)+0.5]); 
ylim(ax, [-(L1+L2)-0.5, (L1+L2)+0.5]);

culori = turbo(M); 
h_brate = gobjects(M, 1);
h_urme = gobjects(M, 1);

for i = 1 : M
    h_urme(i) = plot(ax, X2(1, i), Y2(1, i), '-', 'Color', [culori(i, :), 0.5], 'LineWidth', 1);
    h_brate(i) = plot(ax, [0, X1(1, i), X2(1, i)], [0, Y1(1, i), Y2(1, i)], '-o', ...
        'Color', culori(i, :), 'MarkerSize', 4, 'MarkerFaceColor', culori(i, :), 'LineWidth', 1.2);
end

h_titlu = title(ax, 'Timp: 0.00 s', 'Color', 'w', 'FontSize', 12);

cadre_skip = 2; 
lungime_urma = 80; 

for k = 1 : cadre_skip : N
    if ~ishandle(fig)
        break;
    end
    
    start_idx = max(1, k - lungime_urma);
    for i = 1 : M
        set(h_urme(i), 'XData', X2(start_idx:k, i), 'YData', Y2(start_idx:k, i));
        set(h_brate(i), 'XData', [0, X1(k, i), X2(k, i)], 'YData', [0, Y1(k, i), Y2(k, i)]);
    end
    set(h_titlu, 'String', sprintf('Timp: %.2f s', t(k)));
    drawnow;
end
