# Projet Robot : Aspirateur Escalier

Code du projet du robot Monte-Escalier



## commandes

* pio device monitor
* pio run -t upload



## arborescence
```
.git
.gitignore
Aspi_Escalier
├───
├───
└───
Drone
├───
├───
└───
gui
├───
├───
├───
└───
```

## infos esp32 Robot monte-escalier

* \[env:esp32-s3-devkitc-1]
* platform = espressif32
* board = esp32-s3-devkitc-1

## Communication ESP-NOW avec le Drone

Le bouton `info etat` conserve la réponse TCP envoyée à l'interface et diffuse
également les mesures de l'IMU en ESP-NOW sur le canal 1. Le message broadcast
est au format `INFO_ETAT;accel_x;accel_y;accel_z;mag_x;mag_y;mag_z;gyro_x;gyro_y;gyro_z`.
L'ESP32 du Drone doit exécuter le firmware du dossier `Drone`; il écoute sur le
même canal et affiche les messages reçus dans le moniteur série.
