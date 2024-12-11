#! /home/leo_zhang/miniconda3/envs/web_auto/bin/python

# import numpy as np
# from PIL import Image
# import scipy.fftpack as fft
# import imagehash

# def modify_dct(image_path, save_path, intensity=0.01):
#     # Step 1: Open image and convert to grayscale
#     image = Image.open(image_path).convert("L")
#     image_array = np.array(image)

#     # Step 2: Compute the DCT of the image
#     dct = fft.dct(fft.dct(image_array.T, norm='ortho').T, norm='ortho')

#     # Step 3: Modify the DCT coefficients (ignore DC coefficient at [0, 0])
#     height, width = dct.shape
#     for i in range(1, min(height, 10)):  # Change only low-frequency coefficients
#         for j in range(1, min(width, 10)):
#             dct[i, j] += intensity  # Slightly increase the coefficient

#     # Step 4: Compute the inverse DCT to get the modified image
#     modified_image_array = fft.idct(fft.idct(dct.T, norm='ortho').T, norm='ortho')
#     modified_image_array = np.clip(modified_image_array, 0, 255).astype(np.uint8)

#     # Step 5: Save the modified image
#     modified_image = Image.fromarray(modified_image_array)
#     modified_image.save(save_path)

# # Paths
# image_path = "test.jpg"
# modified_image_path = "modified_image.jpg"

# # Apply DCT modification
# modify_dct(image_path, modified_image_path, intensity=0.05)

# # Compute and compare pHashes
# original_phash = imagehash.phash(Image.open(image_path))
# modified_phash = imagehash.phash(Image.open(modified_image_path))

# print(f"Original pHash: {original_phash}")
# print(f"Modified pHash: {modified_phash}")
# print(f"Hamming Distance: {original_phash - modified_phash}")


from PIL import Image
import imagehash
import os

# force working directory to be where the file is located
os.chdir(os.path.dirname(__file__))

def rotate_image(image_path, save_path, angle=-5):
    """
    Rotate the image slightly and save the result.
    """
    image = Image.open(image_path)
    
    # Rotate image with slight angle; fill with white background
    rotated_image = image.rotate(angle, resample=Image.BICUBIC, fillcolor=(255, 255, 255))
    rotated_image.save(save_path)

# Paths
source = './images'
output = "./outputs"

for (i,img) in enumerate(os.listdir('./images')):
    rotated_image_path = os.path.join(output, f'{i}_modded.png')
    image_path = os.path.join(source, img)
    # Apply rotation to each image
    rotate_image(image_path, rotated_image_path,)
    # Compute and compare pHashes
    original_phash = imagehash.phash(Image.open(image_path))
    # tested_hash = imagehash.phash(Image.open(test_path))
    rotated_phash = imagehash.phash(Image.open(rotated_image_path))
    print(f"Original pHash: {original_phash}")
    print(f"Rotated pHash: {rotated_phash}")
    print(f"Hamming Distance: {original_phash - rotated_phash}")

    # Visual Similarity
    if original_phash - rotated_phash == 0:
        print("Images are perceptually identical.")
    elif original_phash - rotated_phash < 10:
        print("Images are perceptually similar.")
    else:
        print("Images are perceptually different.")


