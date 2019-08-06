temp = load('kpLookup_azimuth.mat');
kplookup = struct2array(temp);
camH = 1.65;
seqId = 5;
CarId = 31;
startFrame = 190;
endFrame = 230;
totalFrames = endFrame - startFrame + 1;
seq = seqId*ones(1,totalFrames); %Sequences
frm = [startFrame:endFrame]; %Frames
id  = CarId*ones(1,totalFrames); %CarID's
% hourglass network output
Pts = importdata('Multi/keypoints_5_190-230_31.txt');
avgCarHeight = 1.5208;
avgCarWidth = 1.6362;
avgCarLength = 3.8600;
K = [721.53,0,609.55;0,721.53,172.85;0,0,1];
numViews = totalFrames;
%mean wireframe
frame = [2.5447 -3.7577 -1.5125;
-3.0188 -3.8300 -1.5128;
2.2950 3.4357 -1.2746;
-2.8187 3.3599 -1.2430;
1.8002 -5.7926 -0.2371;
-2.2685 -5.8769 -0.2381;
1.5809 5.3567 0.2227;
-2.1554 5.3089 0.2352;
2.6917 -1.6341 0.8413;
-3.1261 -1.7055 0.8464;
1.6331 -0.9118 1.9708;
-2.0508 -0.9471 1.9701;
1.4829 2.6154 2.0137;
-1.9433 2.6160 2.0160];

% 5 eigen vectors
v1 = [-0.151346 0.218373 0.088189 0.178544 0.222476 0.088167 -0.130965 -0.199768 0.073618 0.161234 -0.195187 0.071677 -0.108222 0.338475 0.013481 0.134977 0.343590 0.013548 -0.087232 -0.310159 -0.012570 0.120979 -0.307142 -0.013180 -0.157611 0.089988 -0.049292 0.182542 0.094109 -0.049571 -0.095605 0.049274 -0.114369 0.119684 0.051306 -0.114337 -0.086239 -0.146814 -0.115755 0.112563 -0.146889 -0.115872];
v2 = [-0.098323 -0.065454 0.021927 0.087594 -0.045426 0.015041 0.173314 -0.027648 -0.026248 -0.197289 0.034519 -0.027247 -0.334409 0.314357 -0.024683 0.322223 0.313363 -0.022762 0.230236 0.202303 0.013518 -0.218483 0.230444 0.017632 -0.082111 -0.240761 -0.035543 0.069127 -0.239921 -0.034522 -0.073834 -0.244503 0.002144 0.059896 -0.247034 0.003619 0.074159 -0.014541 0.097339 -0.095463 -0.013529 0.101674];
v3 = [0.144907 0.070979 0.099813 -0.171440 0.121961 0.101635 0.029681 0.074285 0.073524 -0.052592 0.054118 0.080406 0.119029 0.056095 -0.007987 -0.095969 0.039598 -0.007782 -0.049881 0.310210 0.007311 0.027442 0.288766 0.009912 0.120991 0.071623 -0.014953 -0.105925 0.095147 -0.009445 0.001524 -0.045424 -0.086102 0.038969 -0.035048 -0.081712 -0.032435 -0.538018 -0.102501 0.054810 -0.548985 -0.097703];
v4 = [0.303420 0.022644 -0.223071 -0.239204 -0.000371 -0.226790 -0.230123 -0.007481 -0.154541 0.261133 0.027120 -0.155754 -0.102530 0.155292 0.034942 0.086396 0.111854 0.033742 -0.377372 0.040747 -0.055970 0.327202 0.048094 -0.077505 -0.027493 -0.007384 0.099710 0.015499 -0.001164 0.088955 -0.113268 -0.229368 0.202414 0.033187 -0.181117 0.200235 -0.042415 -0.038634 0.173792 0.023206 0.016464 0.160507];
v5 = [0.109164 -0.362282 -0.059697 -0.072880 -0.359107 -0.069168 0.082780 -0.077322 -0.023031 -0.102315 0.046584 -0.014162 -0.176465 0.092178 -0.014550 0.202195 0.095555 -0.011093 -0.032977 -0.110770 -0.015152 0.039475 -0.062289 -0.018643 -0.070913 0.262228 -0.023711 0.076054 0.230184 -0.026444 0.058884 0.324946 0.029187 -0.078275 0.278465 0.029191 0.298251 -0.167472 0.083653 -0.297588 -0.172290 0.090366];
V = [v1;v2;v3;v4;v5];



%lambda values
lambda = [0.25, 0.27 ,0.01, -0.08, -0.05];