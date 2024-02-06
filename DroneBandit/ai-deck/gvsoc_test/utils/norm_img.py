from PIL import Image
import os

# Chemin du dossier contenant les images d'entrée
input_folder = "test/img"

# Chemin du dossier de sortie pour les images modifiées
output_folder = "test/img_norm"

# Vérifier si le dossier de sortie existe, sinon le créer
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# Liste des extensions d'images prises en charge
supported_extensions = [".ppm", ".jpeg", ".png"]

# Parcourir les fichiers du dossier d'entrée
for filename in os.listdir(input_folder):
    if any(filename.lower().endswith(ext) for ext in supported_extensions):
        input_path = os.path.join(input_folder, filename)
        output_path = os.path.join(output_folder, filename)

        # Ouvrir l'image avec PIL
        image = Image.open(input_path)

        # Appliquer la soustraction de 128 à chaque pixel
        modified_image = image.point(lambda p: p - 128)

        # Enregistrer l'image modifiée dans le dossier de sortie
        modified_image.save(output_path)

        print(f"Image {filename} traitée et enregistrée.")

print("Traitement terminé.")
