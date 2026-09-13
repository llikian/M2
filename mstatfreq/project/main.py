from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

def elfros_leung(sample_img, output_size, neighborhood_size):
    # neighborhood size is not the size of the patches but their "radius":
    # how many pixels there are from the side to the center
    # so a neighborhood_size of 3 means a patch of 7 ( 2n + 1 )
    # this is done so that patches are always odd sizes with an actual center pixel

    np_img = np.array(sample_img)

    seed_size = int(neighborhood_size * 1.5)

    patch_size = neighborhood_size * 2 + 1
    patch_shape = np.zeros((patch_size, patch_size, 2), dtype=np.int32)
    for i in range(patch_size):
        for j in range(patch_size):
            patch_shape[i][j] = (i - neighborhood_size, j - neighborhood_size)

    # a 2D array for a bit mask of whether a pixel is filled
    mask = np.zeros((output_size, output_size), dtype=np.bool)
    # a 2D array for the number of filled neighbors of the pixel
    neighbors = np.zeros((output_size, output_size), dtype=np.int32)

    output = np.full((output_size, output_size, 3), (255, 255, 255), dtype=np.uint8)

    seed_x = np.random.randint(seed_size, sample_img.size[0] - seed_size)
    seed_y = np.random.randint(seed_size, sample_img.size[1] - seed_size)
    seed = np_img[seed_x : seed_x + seed_size, seed_y : seed_y + seed_size, :]

    rand_x = np.random.randint(seed_size, output_size - seed_size)
    rand_y = np.random.randint(seed_size, output_size - seed_size)

    for i in range(rand_x, rand_x + seed_size):
        for j in range(rand_y, rand_y + seed_size):
            output[i][j] = seed[i - rand_x][j - rand_y]
            mask[i][j] = True

            for row in patch_shape:
                for (off_x, off_y) in row:
                    if i + off_x < output_size and j + off_y < output_size and i != j:
                        neighbors[i + off_x][j + off_y] += 1

    while np.min(mask) != False:
        index_max = np.argmax(neighbors) # 1D index, need to convert

    output = Image.fromarray(output)
    output.show()
    

if __name__ == "__main__":
    img = Image.open("data/text0.png")
    elfros_leung(img, 128, int(min(img.size[0] / 8, img.size[1] / 8)))

    # arr = np.zeros((5, 5), dtype=np.int16)
    # print(arr)
    # arr[1:4, 1:4] = 1
    # print(arr)

    pass

