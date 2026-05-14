from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget,QSlider,
    QHBoxLayout, QVBoxLayout, QPushButton,QGridLayout,
    QGraphicsView, QGraphicsScene, QLineEdit,QLabel
)
import socket

import style

ESP32_IP = "192.168.4.1"
PORT = 1234

class Popup_donnees(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Monter")
        layout = QVBoxLayout()
        self.txtdonee = QLabel("nombre de marches :")
        self.donnee = QLineEdit() 
        self.txtvitesse = QLabel("vitesse de montée :")
        self.vitesse = QLineEdit() 
        self.button = QPushButton("Valider")
        self.button.clicked.connect(self.lancer)
        layout.addWidget(self.txtdonee)
        layout.addWidget(self.donnee)
        layout.addWidget(self.txtvitesse)
        layout.addWidget(self.vitesse)
        layout.addWidget(self.button)
        self.setLayout(layout)

    def lancer(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        mot1 = self.vitesse.text()
        taille_esc = self.donnee.text()
        s.sendall(f"monte_escalier;{mot1};{taille_esc};".encode())  # envoie commande
        s.close()

class Popup_tourne(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Tourner")
        layout = QVBoxLayout()
        self.txtdonee = QLabel("nombre de tours de roue :")
        self.donnee = QLineEdit()  
        self.button = QPushButton("Valider")
        self.button.clicked.connect(self.lancer_tourne)
        layout.addWidget(self.txtdonee)
        layout.addWidget(self.donnee)
        layout.addWidget(self.button)
        self.setLayout(layout)

    def lancer_tourne(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        nbTour = self.donnee.text()
        s.sendall(f"tourne;{nbTour};".encode())  # envoie commande
        s.close()

class Popup_avance_controlee(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Avance Contrôlée")
        layout = QVBoxLayout()
        self.txtdonee = QLabel("vitesse 1 G :")
        self.donnee = QLineEdit()  
        self.txtdonee2 = QLabel("vitesse 2 G :")
        self.donnee2 = QLineEdit() 
        self.txtdoneeD = QLabel("vitesse 1 D :")
        self.donneeD = QLineEdit()  
        self.txtdonee2D = QLabel("vitesse 2 D :")
        self.donnee2D = QLineEdit() 
        self.txtdonee3 = QLabel("temps :")
        self.donnee3 = QLineEdit()
        self.button = QPushButton("Valider")
        self.button.clicked.connect(self.lancer_avance_controlee)
        layout.addWidget(self.txtdonee)
        layout.addWidget(self.donnee)
        layout.addWidget(self.txtdonee2)
        layout.addWidget(self.donnee2)
        layout.addWidget(self.txtdoneeD)
        layout.addWidget(self.donneeD)
        layout.addWidget(self.txtdonee2D)
        layout.addWidget(self.donnee2D)
        layout.addWidget(self.txtdonee3)
        layout.addWidget(self.donnee3)

        layout.addWidget(self.button)
        self.setLayout(layout)
        # ajouter temps de l avance contrôlée

    def lancer_avance_controlee(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        vitesse1 = self.donnee.text()
        vitesse2 = self.donnee2.text()
        vitesse1D = self.donneeD.text()
        vitesse2D = self.donnee2D.text()
        temps = self.donnee3.text()  
        s.sendall(f"avance_controlee;{vitesse1};{vitesse2};{vitesse1D};{vitesse2D};{temps};".encode())  # envoie commande
        s.close()

class Popup_plateforme(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Plateforme")
        layout = QVBoxLayout()
        self.txtdonee = QLabel("angle (<90 monte! ; >90 descend) :")
        self.donnee = QLineEdit()  
        self.txtdonee2 = QLabel("duree :")
        self.donnee2 = QLineEdit()  
        self.button = QPushButton("Valider")
        self.button.clicked.connect(self.lancer_plat)
        layout.addWidget(self.txtdonee)
        layout.addWidget(self.donnee)
        layout.addWidget(self.txtdonee2)
        layout.addWidget(self.donnee2)
    
        layout.addWidget(self.button)
        self.setLayout(layout)

    def lancer_plat(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        angle = self.donnee.text()
        duree = self.donnee2.text()
     
        s.sendall(f"platforme_monter;{angle};{duree};\n".encode())  # envoie commande
        s.close()




# popup Robot Aspi -----------------------------------------------------------------------------

class Popup_direction_aspi(QWidget):
    def __init__(self) :
        super().__init__()
        self.setStyleSheet(style.STYLE)
        self.setWindowTitle("Diriger le robot aspirateur")
        layout = QVBoxLayout()
        grid = QGridLayout()
        self.b_gauche = QPushButton("Gauche")
        self.b_gauche.clicked.connect(self.gauche)
        self.b_droite = QPushButton("Droite")
        self.b_droite.clicked.connect(self.droite)
        self.b_avancer = QPushButton("Avancer")
        self.b_avancer.clicked.connect(self.avancer)
        self.b_reculer = QPushButton("Reculer")
        self.b_reculer.clicked.connect(self.reculer)
        self.b_stop = QPushButton("Stop")
        self.b_stop.clicked.connect(self.stop)
        grid.addWidget(self.b_gauche, 1, 0)
        grid.addWidget(self.b_droite, 1, 2)
        grid.addWidget(self.b_avancer, 0, 1)
        grid.addWidget(self.b_reculer, 2, 1)
        grid.addWidget(self.b_stop, 1, 1)
        layout.addLayout(grid)
        self.setLayout(layout)

    def gauche(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"aspi_gauche;\n".encode())  # envoie commande
        s.close()

    def droite(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"aspi_droite;\n".encode())  # envoie commande
        s.close()

    def avancer(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"aspi_avancer;\n".encode())  # envoie commande
        s.close()

    def reculer(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"aspi_reculer;\n".encode())  # envoie commande
        s.close()

    def stop(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"aspi_stop;\n".encode())  # envoie commande
        s.close()