import os
from PIL import Image

# Chemin du dossier contenant les images d'origine
dossier_images = "training/images"

# Chemin du dossier où vous souhaitez sauvegarder les images reformatées
dossier_sauvegarde = "training/prepro"

# Vérifier si le dossier de sauvegarde existe, sinon le créer
if not os.path.exists(dossier_sauvegarde):
    os.makedirs(dossier_sauvegarde)

# Parcourir toutes les images du dossier
for nom_fichier in os.listdir(dossier_images):
    chemin_image = os.path.join(dossier_images, nom_fichier)

    # Vérifier si le fichier est une image
    if os.path.isfile(chemin_image) and nom_fichier.lower().endswith((".jpg", ".jpeg", ".png")):
        # Ouvrir l'image
        image = Image.open(chemin_image)

        # Redimensionner en 200x200
        image = image.resize((200, 200))

        # Convertir en noir et blanc
        image = image.convert("L")

        # Enregistrer l'image reformée en format PPM
        nom_fichier_sauvegarde = os.path.splitext(nom_fichier)[0] + ".ppm"
        chemin_sauvegarde = os.path.join(dossier_sauvegarde, nom_fichier_sauvegarde)
        image.save(chemin_sauvegarde, "PPM")

        print(f"Image {nom_fichier} reformée et sauvegardée en format PPM.")

print("Toutes les images ont été reformées et sauvegardées en format PPM.")
