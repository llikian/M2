pkg load image;

im=imread('fraise-foveon.jpg');
figure(1);
imshow(im);
waitforbuttonpress();

R = im(:,:,1);
G = im(:,:,2);
B = im(:,:,3);


function ret = rgb2gray(image)
    ret = 0.298936*image(:,:,1) + 0.587043*image(:,:,2) + 0.114021*image(:,:,3);
end

gray_im = rgb2gray(im);
% imshow(gray_im);
montage({im, gray_im});
waitforbuttonpress();
