% main file
% ================================================================
old_img = double(imread('001.jpg')) / 255;
old_img_size = size(old_img);
img = reshape(old_img, old_img_size(1) * old_img_size(2), 3);

k = 20; % the number of classfication

random_idx = randperm(size(img, 1));
init = img(random_idx(1 : k), :);

img_size = size(img);
idx = zeros(img_size(1), 1);
cent = init;

for p = 1 : 15 % iteration times
    printf('iteration %d\n', p);
    for i = 1 : img_size(1)
        d = 1000.00;
        for j = 1 : k
            tmp = 0;
            for l = 1 : img_size(2)
                tmp = tmp + (img(i, l) - cent(j, l)) ^ 2 ;
            end
            if (tmp < d)
                d = tmp;
                idx(i) = j;
            end
        end
    end
    
    num = zeros(k, 1);
    for q = 1 : img_size(1)
        for l = 1 : img_size(2)
            % fprintf('dshguihg %d %d %d\n', idx(q), q, l);
            % fprintf('dshguihg %d %d %d\n', cent(idx(q), l), cent(idx(q), l), img(q, l));
            cent(idx(q), l) = cent(idx(q), l) + img(q, l);
        end
        num(idx(q)) = num(idx(q)) + 1;
    end
    for q = 1 : k
        cent(q, :) = cent(q, :) / num(q);
    end
end

% update idx
for i = 1 : size(img, 1)
    d = 1000.00;
    for j = 1 : k
        tmp = 0;
        for k = 1 : size(img, 2)
            tmp = tmp + (img(i, k) - cent(j, k)) ^ 2 ;
        end
        if (tmp < d)
            d = tmp;
            idx(i) = j;
        end
    end
end

new_img = cent(idx, :);
new_img = reshape(new_img, old_img_size(1), old_img_size(2), 3);


% display the result
subplot(1, 2, 1);
imagesc(old_img);

subplot(1, 2, 2);
imagesc(new_img);






