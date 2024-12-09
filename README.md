# Projet composteur
Repo du projet composteur


![Architecture Systeme - Composteur](https://github.com/user-attachments/assets/da079198-6b6c-4c16-9085-c946df8af1d3)

<img src="https://github.com/user-attachments/assets/5964c556-d3c6-4e22-9a34-b06894382f2c">

## Fonctions

### Récupération des dernière données par l'IHM

```py
get_last_data(capteur)
```

| **Nom**   | Type|
| -------------   | --------- |
| **Retour** | `list -> [id, float, time]` |

### Récupération de toutes les données par l'IHM

```py
get_all_data(capteur)
```

| **Nom**   | Type|
| -------------   | --------- |
| **Retour** | `list -> [[id, float, time]]` |

### Récupération de toutes les données par l'IHM

```py
get_interval_data(capteur, start_time, end_time)
```

| **Nom**   | Type|
| -------------   | --------- |
| **Retour** | `list -> [[id, float, time]]` |
