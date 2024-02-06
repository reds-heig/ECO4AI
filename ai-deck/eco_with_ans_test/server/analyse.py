import csv

def regrouper_par_colonne(nom_fichier):
    # Créer un dictionnaire pour stocker les données groupées
    data = []
    for i in range(9):
        data.append({
            "front_time": 0,
            "front_max_time": 0,
            "front_min_time": 5000000000,
            "end_time": 0,
            "end_max_time": 0,
            "end_min_time": 5000000000,
            "total_time": 0,
            "min_tot": 5000000000,
            "max_tot": 0,
            "count": 0
        })

    # Ouvrir le fichier CSV en mode lecture
    with open(nom_fichier, 'r') as fichier_csv:
        file = fichier_csv.readlines()
        first = True

        # Parcourir chaque ligne du fichier CSV
        for line in file:
            if not first:
                col = line.split(",")
                key = int(col[0], 10)
                data[key]["front_time"] += int(col[1], 10)
                data[key]["end_time"] += int(col[2], 10)
                data[key]["total_time"] += int(col[4], 10)

                if data[key]["total_time"] > data[key]["max_tot"]:
                    data[key]["max_tot"] = data[key]["total_time"]
                if data[key]["total_time"] < data[key]["min_tot"]:
                    data[key]["min_tot"] = data[key]["total_time"]

                if data[key]["front_time"] > data[key]["front_max_time"]:
                    data[key]["front_max_time"] = data[key]["front_time"]
                if data[key]["front_time"] < data[key]["front_min_time"]:
                    data[key]["front_min_time"] = data[key]["front_time"]

                if data[key]["end_time"] > data[key]["end_max_time"]:
                    data[key]["end_max_time"] = data[key]["end_time"]
                if data[key]["end_time"] < data[key]["end_min_time"]:
                    data[key]["end_min_time"] = data[key]["end_time"]

                data[key]["count"] += 1
            else:
                first = False

    for d in data:
        d["front_time"] = d["front_time"] / d["count"]
        d["end_time"] = d["end_time"] / d["count"]
        d["total_time"] = d["total_time"] / d["count"]
    print(data)
    return data

# Utilisation de la fonction pour regrouper les données
nom_fichier = 'data_time_by_layer.csv'  # Remplacez avec le nom de votre fichier CSV
resultat = regrouper_par_colonne(nom_fichier)


header = "Layer,front_time,front_max_time,front_min_time,end_time,end_max_time,end_min_time,total_time,min_tot,max_tot,count"

with open("data_to_graph.csv", 'w') as fichier:
    fichier.write(header + '\n')

    # Afficher les données groupées
    for i, d in enumerate(resultat):
        print(f"CP == {i}")
        fichier.write(f"{i},")
        for cle, valeur in d.items():
            print(f'Clé : {cle}')
            print(f'Valeurs : {valeur}')
            fichier.write(f"{valeur},")
            print('-' * 20)
        fichier.write("\n")
