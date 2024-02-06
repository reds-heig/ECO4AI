import csv


def comparer_colonnes(og, comp):
    
    cpt = [[0, 0], [0, 0]]
    c = 0
    size = min(len(og), len(comp))
    print(f"size = {size}")

    for i in range(0, size):
        print()
        cpt[og[i]][comp[i]] += 1
        if og[i] == comp[i]:
            c += 1

    print(f"\t0\t1\n")
    print(f"0\t{cpt[0][0]}\t{cpt[0][1]}\n")
    print(f"1\t{cpt[1][0]}\t{cpt[1][1]}")
    # print(f"count = {cpt}")
    acc = (cpt[0][0]+cpt[1][1])/(cpt[0][0]+cpt[1][1]+cpt[0][1]+cpt[1][0])
    print(f"accuracy = {acc}")


def lire_fichier(nom_fichier):
    try:
        # Ouvre le fichier en mode lecture
        with open(nom_fichier, 'r') as fichier:
            # Lit toutes les lignes du fichier et les stocke dans un tableau
            lignes = fichier.readlines()
            lignes_entiers = [int(ligne) for ligne in lignes]
        return lignes_entiers
    except FileNotFoundError:
        print("Le fichier", nom_fichier, "n'a pas été trouvé.")
        return []

# Exemple d'utilisation
labels_file = "ruco.txt"
labels = lire_fichier(labels_file)

check_file = "winston.txt"
check = lire_fichier(check_file)

comparer_colonnes(labels, check)


# Exemple d'utilisation
# comparer_colonnes(file)