from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget,QSlider,
    QHBoxLayout, QVBoxLayout, QPushButton,QGridLayout,
    QGraphicsView, QGraphicsScene, QLineEdit,QLabel
)
import socket

class Popup_donnees(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Donnees de l'ecalier")
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
        self.button = QPushButton("Valider")
        self.button.clicked.connect(self.lancer_plat)
        layout.addWidget(self.txtdonee)
        layout.addWidget(self.donnee)
    
        layout.addWidget(self.button)
        self.setLayout(layout)

    def lancer_plat(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        angle = self.donnee.text()
     
        s.sendall(f"platforme_monter;{angle};\n".encode())  # envoie commande
        s.close()