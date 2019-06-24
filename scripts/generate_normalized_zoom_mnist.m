normalization_min = 60;
normalization_max = 90;
pixel_mean_save1 = create_normalized_zoom_mnist('/home/hyz/Downloads/dataset/fashion-mnist/t10k-images-idx3-ubyte',normalization_min, normalization_max, 1);
pixel_mean_save2 = create_normalized_zoom_mnist('/home/hyz/Downloads/dataset/fashion-mnist/train-images-idx3-ubyte',normalization_min, normalization_max, 1);
% pixel_mean_save3 = create_normalized_zoom_mnist('/home/hyz/Downloads/dataset/mnist/t10k-images-idx3-ubyte',normalization_min, normalization_max, 0);
% pixel_mean_save4 = create_normalized_zoom_mnist('/home/hyz/Downloads/dataset/mnist/train-images-idx3-ubyte',normalization_min, normalization_max, 0);
% 
% 
% h1 = histogram(pixel_mean_save1);
% % legend(h1, 'fashion test');
% hold on;
% h2 = histogram(pixel_mean_save2);
% % legend(h2, 'fashion train');
% % hold on;
% h3 = histogram(pixel_mean_save3);
% % legend(h3, 'mnist test');
% % hold on;
% h4 = histogram(pixel_mean_save4);
% legend([h1,h2,h3,h4],'fashion test','fashion train','mnist test','mnist train');
% % hold on;
% h1.Normalization = 'probability';
% h1.BinWidth = 5;
% h2.Normalization = 'probability';
% h2.BinWidth = 5;
% h3.Normalization = 'probability';
% h3.BinWidth = 5;
% h4.Normalization = 'probability';
% h5.BinWidth = 5;