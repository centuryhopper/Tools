from PIL import Image
import imagehash

def add_transparent_pixel(image_path, save_path, position=(0, 0)):
    """
    Adds a transparent pixel to an image at the specified position.

    Parameters:
    - image_path: Path to the input image.
    - save_path: Path to save the modified image.
    - position: Tuple (x, y) specifying where to make the pixel transparent.
    """
    # Open the image
    image = Image.open(image_path).convert("RGBA")  # Ensure image has an alpha channel
    pixels = image.load()

    # Get position
    x, y = position
    if x < image.width and y < image.height:
        # Set the pixel to fully transparent
        pixels[x, y] = (0, 0, 0, 0)  # Fully transparent pixel (R=0, G=0, B=0, A=0)

    # Save the modified image
    image.save(save_path)

# Paths
image_path = "test.jpg"  # Use an image format that supports transparency (e.g., PNG)
modified_image_path = "result.png"

# Add a transparent pixel at (10, 10)
add_transparent_pixel(image_path, modified_image_path, position=(10, 10))

print("Transparent pixel added and image saved.")


# Compute and compare pHashes
original_phash = imagehash.phash(Image.open(image_path))
tested_hash = imagehash.phash(Image.open(modified_image_path))
# rotated_phash = imagehash.phash(Image.open(rotated_image_path))

print(f"Original pHash: {original_phash}")
# print(f"Rotated pHash: {rotated_phash}")
print(f"Hamming Distance: {original_phash - tested_hash}")

# Visual Similarity
if original_phash - tested_hash == 0:
    print("Images are perceptually identical.")
elif original_phash - tested_hash < 10:
    print("Images are perceptually similar.")
else:
    print("Images are perceptually different.")