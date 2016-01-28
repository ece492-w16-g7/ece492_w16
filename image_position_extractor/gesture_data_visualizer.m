%%
clc; clear;

% http://stackoverflow.com/questions/2856417/reading-a-text-file-in-matlab-line-by-line
[numData, ~, ~] = xlsread('position_info.csv');

[n_input, ~] = size(numData);
for i = 1:n_input
    imshow(zeros(480, 640));
    hold on;
    %http://kr.mathworks.com/matlabcentral/answers/67879-putting-visual-marker-in-an-image
    point = numData(i,:);
    plot(point(2), point(1), 'r+', 'MarkerSize', 50);
    pause(0.1);
end