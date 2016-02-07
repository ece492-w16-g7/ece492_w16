%%
clc; clear;

file = fopen('position_info.csv', 'w');

% References:
% http://www.mathworks.com/help/supportpkg/usbwebcams/ug/acquire-webcam-images-in-a-loop.html
cam = webcam;

cam.Resolution = '640x480';

binary_threshold_value = 250;
n_calibration_frames = 10;

fprintf('Calibrating in:\n');
pause(1);
fprintf('3\n');
pause(1);
fprintf('2\n');
pause(1);
fprintf('1\n');
pause(1);
fprintf('Calibrating\n');

% Calibrate
frame = snapshot(cam);
baseline_frame = uint16(frame(:,:,2));
for c = 1:n_calibration_frames
    frame = snapshot(cam);
    green_channel = uint16(frame(:,:,2));
    baseline_frame = (baseline_frame + green_channel) ./ 2;
    pause(0.1);
end
baseline_frame = uint8(baseline_frame);

fprintf('Finished calibration\n');
pause(1);
imshow(baseline_frame);

i = 0;
while (1)
    frame = snapshot(cam);

    green_channel = frame(:,:,2);
    difference_frame = green_channel - baseline_frame;
    amplified_frame = 3 .* difference_frame;
    %binary_frame = amplified_frame > binary_threshold_value;
    binary_frame = green_channel > binary_threshold_value;
    imshow(binary_frame);

    hold on;

    consec_pixels = 0;
    consec_pixels_thresh = 5;

    biggest_blob_size = 0;
    biggest_blob = [0, 0];

    [r, c] = size(binary_frame);
    for m=1:r
        for n=1:c
            if (binary_frame(m,n) == 1)
                consec_pixels = consec_pixels + 1;
            elseif(consec_pixels > consec_pixels_thresh)
                if (consec_pixels > biggest_blob_size)
                    biggest_blob_size = consec_pixels;
                    biggest_blob = [m, n];
                end
                consec_pixels = 0;
            end
        end
    end

    %http://kr.mathworks.com/matlabcentral/answers/67879-putting-visual-marker-in-an-image
    plot(biggest_blob(2), biggest_blob(1), 'r+', 'MarkerSize', 50);

    fprintf(file, '%d,%d\n', biggest_blob(1), biggest_blob(2));

    clc;
    fprintf('%i\n', i);
    i = i+1;
end

fclose(file);
