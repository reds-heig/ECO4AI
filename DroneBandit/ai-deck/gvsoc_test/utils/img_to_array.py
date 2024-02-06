from PIL import Image

def image_to_c_array(image_path, array_name):
    image = Image.open(image_path)
    if image.mode != 'L':
        raise ValueError("L'image doit être en niveaux de gris (8 bits)")
    
    width, height = image.size
    pixel_data = image.tobytes()

    c_array = f"const unsigned char {array_name}[] = {{\n"
    
    for i in range(0, len(pixel_data), width):
        row_data = pixel_data[i:i+width]
        row_str = ", ".join(str(byte) for byte in row_data)
        c_array += f"    {row_str},\n"
    
    c_array += "};"
    
    return c_array

if __name__ == "__main__":
    image_path = "test/img/00000.ppm"
    array_name = "mon_image"
    
    c_array = image_to_c_array(image_path, array_name)
    
    with open("image_array.h", "w") as file:
        file.write(c_array)
    
    print(f"Le tableau C a été exporté dans le fichier 'image_array.h'")
