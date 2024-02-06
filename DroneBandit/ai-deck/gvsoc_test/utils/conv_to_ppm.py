from PIL import Image
import os

def convertir_jpg_vers_ppm(chemin_dossier):
    # Vérifie si le dossier de sortie existe, sinon le crée
    dossier_sortie = "custom_calibration_set/ppm"
    if not os.path.exists(dossier_sortie):
        os.makedirs(dossier_sortie)

    # Obtient la liste des fichiers .jpg dans le dossier spécifié
    fichiers_jpg = [fichier for fichier in os.listdir(chemin_dossier) if fichier.endswith('.jpeg')]

    for fichier_jpg in fichiers_jpg:
        # Chemin complet du fichier d'entrée
        chemin_fichier_jpg = os.path.join(chemin_dossier, fichier_jpg)

        # Ouvre l'image JPEG
        image = Image.open(chemin_fichier_jpg)

        # Chemin complet du fichier de sortie
        fichier_ppm = os.path.splitext(fichier_jpg)[0] + ".ppm"
        chemin_fichier_ppm = os.path.join(dossier_sortie, fichier_ppm)

        # Convertit et enregistre l'image en PPM
        image.save(chemin_fichier_ppm, "PPM")

        print(f"Le fichier {fichier_jpg} a été converti en {fichier_ppm}.")

# Exemple d'utilisation
dossier_images = "custom_calibration_set/jpgeg"
convertir_jpg_vers_ppm(dossier_images)
