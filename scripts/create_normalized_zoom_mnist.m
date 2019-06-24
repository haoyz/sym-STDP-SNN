function pixel_mean_save = create_normalized_zoom_mnist(filename, normalization_min, normalization_max, normalization)
% filename
% /home/hyz/Downloads/dataset/fashion-mnist/t10k-images-idx3-ubyte
% /home/hyz/Downloads/dataset/fashion-mnist/train-images-idx3-ubyte
% read mnist
fp = fopen(filename, 'r+');
assert(fp ~= -1, ['Could not open ', filename, '']);

magic = fread(fp, 1, 'int32', 0, 'ieee-be');
assert(magic == 2051, ['Bad magic number in ', filename, '']);

numImages = fread(fp, 1, 'int32', 0, 'ieee-be');
numRows = fread(fp, 1, 'int32', 0, 'ieee-be');
numCols = fread(fp, 1, 'int32', 0, 'ieee-be');

images = fread(fp, inf, 'uint8');
fclose(fp);

% DoG filter
images = reshape(images, numCols, numRows, numImages);
image_mean=mean(images(:));
disp(image_mean);

images_processed = zeros(size(images));
pixel_mean_little = 100000;
pixel_mean_large = 1;
pixel_mean_save = zeros(size(numImages));
for i = 1:numImages
    image_pre = images(:,:,i);
    pixel_mean = mean(image_pre(:));
    pixel_mean_save(i) = pixel_mean;
    if pixel_mean_little > pixel_mean
        pixel_mean_little = pixel_mean;
    end
    if pixel_mean_large < pixel_mean
        pixel_mean_large = pixel_mean;
    end
end
% h = histogram(pixel_mean_save)
% 
if normalization == 1
    for i = 1:numImages
        image_pre = images(:,:,i);
        pixel_mean = mean(image_pre(:));

        pixel_mean_processed = (normalization_max - normalization_min)*(pixel_mean - pixel_mean_little)/(pixel_mean_large - pixel_mean_little) + normalization_min;
        filter = [pixel_mean_processed / pixel_mean];
    %     filter = [normalization / pixel_mean];
        image = imfilter(images(:,:,i), filter);

    %     image = 255 * mat2gray(image);
        images_processed(:,:,i) = image;

    end
%     pixel_mean_little = 100000;
%     pixel_mean_large = 1;
%     for i = 1:numImages
%         image_pre = images_processed(:,:,i);
%         pixel_mean = mean(image_pre(:));
%         pixel_mean_save(i) = pixel_mean;
%         if pixel_mean_little > pixel_mean
%             pixel_mean_little = pixel_mean;
%         end
%         if pixel_mean_large < pixel_mean
%             pixel_mean_large = pixel_mean;
%         end
%     end
%     pixel_mean_little
%     pixel_mean_large
end



% write to file
copyfile(filename, [filename '_normalized_to' mat2str(normalization_min) '_' mat2str(normalization_max)]);

fp = fopen([filename '_normalized_to' mat2str(normalization_min) '_' mat2str(normalization_max)], 'r+');
fseek(fp, 16, 'bof');
fwrite(fp, images_processed, 'uint8');
fclose(fp);

end
