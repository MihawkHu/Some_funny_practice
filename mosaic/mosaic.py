from PIL import Image

img_tmp = Image.open("001.jpg")
img = img_tmp

size = img.size;

u_size = 5;   # the size of one masaic unit

lon = size[0] / u_size;
wid = size[1] / u_size;

for i in range(0, lon):
    for j in range(0, wid):
        x = i * u_size + u_size / 2
        y = j * u_size + u_size / 2
        color = img.getpixel((x, y))
        
        x_bor = y_bor = u_size
        for p in range(0, x_bor):
            for q in range(0, y_bor):
                x_tmp = x - u_size / 2 + p;
                y_tmp = y - u_size / 2 + q;
                img.putpixel((x_tmp, y_tmp), color)

img.save("002.jpg", "jpeg")

