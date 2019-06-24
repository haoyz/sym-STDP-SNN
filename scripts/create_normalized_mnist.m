function create_normalized_mnist(filename, normalization)
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

images_processed = zeros(size(images));

for i = 1:numImages
    image_pre = images(:,:,i);
    pixel_mean = mean(image_pre(:));
    filter = [normalization / pixel_mean];
    image = imfilter(images(:,:,i), filter);

%     image = 255 * mat2gray(image);
    images_processed(:,:,i) = image;
    
end

% write to file
copyfile(filename, [filename '_normalized_to' mat2str(normalization)]);

fp = fopen([filename '_normalized_to' mat2str(normalization)], 'r+');
fseek(fp, 16, 'bof');
fwrite(fp, images_processed, 'uint8');
fclose(fp);

end
