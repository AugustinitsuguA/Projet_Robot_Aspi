import sys
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget,QSlider,
    QHBoxLayout, QVBoxLayout, QPushButton,QGridLayout,
    QGraphicsView, QGraphicsScene, QLineEdit,QLabel
)
from PyQt6.QtCore import (Qt,QTimer)
from PyQt6.QtGui import QBrush, QPen
from PyQt6.QtGui import QColor

import socket
import time


ESP32_IP = "192.168.4.1"  # l'IP ESP32
PORT = 1234

mode = "test"
nb_tour_av = 0
nb_tour_d = 0
nb_tour_g = 0
etat_sync = 0     # 0 su les roues sont désynchronisée et 1 si elles sont synchronisées


class Popup_donnees(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Donnees de l'ecalier")
        layout = QVBoxLayout()
        self.donnee = QLineEdit() 
        self.vitesse = QLineEdit() 
        self.button = QPushButton("Valider")
        self.button.clicked.connect(self.lancer)
        layout.addWidget(self.donnee)
        layout.addWidget(self.vitesse)
        layout.addWidget(self.button)
        self.setLayout(layout)

    def lancer(self):
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        mot1 = self.vitesse.text()
        taille_esc = self.donnee.text()
        s.sendall(f"monte;{mot1};{taille_esc};".encode())  # envoie commande
        s.close()





class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setStyleSheet("""
        QPushButton {
            background-color: #6E651F;
            color: white;
            border-radius: 8px;
            padding: 8px;
        }
        QPushButton:hover {
            background-color: #64825A;
        }
        QMainWindow {
            background-color: #36020F;
        }
        QLabel {
            background-color: #0F614B;
            color: #FCEED2
        }
        QSlider::groove:vertical {
            background: #2c3e50;
            width: 8px;
            border-radius: 4px;               
        }     
        QSlider::handle:vertical {
            background: #27ae60;
            height: 20px;
            margin: -5px;
            border-radius: 10px;
        }

        QSlider::handle:vertical:hover {
            background: #2ecc71;
        }  
        """)        

        self.setWindowTitle("Interface Graphique")
        self.setGeometry(200, 200, 800, 600)

        # Widget central
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        main_layout = QHBoxLayout()
        central_widget.setLayout(main_layout)

        # création des layout
        left_layout = QVBoxLayout()
        right_layout = QVBoxLayout()
        bottom_layout = QGridLayout()
        curseur_layout = QGridLayout()


        # boutons 
        self.b_tourne = QPushButton("tourne")
        self.b_info_etat = QPushButton("info etat")
        self.b_afficher_icm = QPushButton("afficher icm")
        self.b_stop = QPushButton("stopper affichage ICM")
        self.b_init = QPushButton("reinit. les valeurs")
        self.b_envoyer = QPushButton("envoyer valeur")
        self.b_monte = QPushButton("monter")
        self.b_stop_moteurs = QPushButton("stop moteurs")
        self.b_sync = QPushButton("synchro")
        #self.text1 = QLineEdit()
        #self.text2 = QLineEdit()

        left_layout.addWidget(self.b_info_etat)
        left_layout.addWidget(self.b_afficher_icm)
        left_layout.addWidget(self.b_stop)
        left_layout.addWidget(self.b_monte)
        left_layout.addWidget(self.b_tourne)
        left_layout.addWidget(self.b_envoyer)
        left_layout.addWidget(self.b_stop_moteurs)
        left_layout.addWidget(self.b_init)

        #left_layout.addWidget(self.text1)
        #left_layout.addWidget(self.text2)

        left_layout.addStretch()

        # ajout des curseurs
        self.s_curseur1 = QSlider(Qt.Orientation.Vertical)
        self.s_curseur2 = QSlider(Qt.Orientation.Vertical)
        self.s_curseur1.setMinimum(-255)
        self.s_curseur1.setMaximum(255)
        self.s_curseur1.setValue(0)
        self.s_curseur2.setMinimum(-255)
        self.s_curseur2.setMaximum(255)
        self.s_curseur2.setValue(0)
        self.l_curs1 = QLabel()
        self.l_curs2 = QLabel()
        curseur_layout.addWidget(self.s_curseur1,0,0)
        curseur_layout.addWidget(self.s_curseur2,0,1)
        curseur_layout.addWidget(self.l_curs1,1,0)
        curseur_layout.addWidget(self.l_curs2,1,1)
        curseur_layout.addWidget(self.b_sync,2,0)

        self.s_curseur1.valueChanged.connect(self.valeurChange)
        self.s_curseur2.valueChanged.connect(self.valeurChange)
        

        # zone de graphique
        self.scene = QGraphicsScene()
        self.view = QGraphicsView(self.scene)

        right_layout.addWidget(self.view, 4)


        # placement des layout
        main_layout.addLayout(left_layout, 2)
        main_layout.addLayout(right_layout, 2)
        right_layout.addLayout(bottom_layout, 1)
        left_layout.addLayout(curseur_layout, 1)

    

        # zone de plot 
      
        self.b_nb_tour = QLabel("tours de roue av")
        self.b_nb_tour_val = QLabel(str(nb_tour_av))
        self.b_nb_tour_d = QLabel("tours de roue droite")
        self.b_nb_tour_d_val = QLabel(str(nb_tour_d))
        self.b_nb_tour_g = QLabel("tours de roue gauche")
        self.b_nb_tour_g_val = QLabel(str(nb_tour_g))
        bottom_layout.addWidget(self.b_nb_tour,0,0)
        bottom_layout.addWidget(self.b_nb_tour_val,0,1)
        bottom_layout.addWidget(self.b_nb_tour_d,1,0)
        bottom_layout.addWidget(self.b_nb_tour_d_val,1,1)
        bottom_layout.addWidget(self.b_nb_tour_g,2,0)
        bottom_layout.addWidget(self.b_nb_tour_g_val,2,1)


        # affecte une fonction à un bouton
        self.b_info_etat.clicked.connect(self.info_etat)
        self.b_afficher_icm.clicked.connect(self.afficher_icm)
        self.b_stop.clicked.connect(self.stop_icm)
        self.b_monte.clicked.connect(self.monte)
        self.b_tourne.clicked.connect(self.tourne)
        self.b_envoyer.clicked.connect(self.envoyer_msg)
        self.b_stop_moteurs.clicked.connect(self.stop_moteurs)
        self.b_init.clicked.connect(self.init_valeurs)
        
        self.b_sync.clicked.connect(self.sync)

        # si on est dans le mode normal, toutes les secondes on récupère les valeurs de tour de roue
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_tours)
        print("testestest")
        if (mode == "standard"):
            print("oui")
            
            self.timer.start(4000)  # 1000 ms = 1 seconde

    def sync(self) :
        global etat_sync
        self.s_curseur1.setValue(0)
        self.s_curseur2.setValue(0)
        if etat_sync == 0 :
            etat_sync = 1
        else :
            etat_sync = 0

    # synchroniser les deux curseurs
    def valeurChange(self) :
        global etat_sync
        if etat_sync == 0 :  # état désynchro
            self.l_curs1.setText(str(self.s_curseur1.value()))
            self.l_curs2.setText(str(self.s_curseur2.value()))
            self.s_curseur1.setValue(self.s_curseur1.value())
            self.s_curseur2.setValue(self.s_curseur2.value())
        else :   # état synchro
            self.l_curs1.setText(str(self.s_curseur1.value()))
            self.l_curs2.setText(str(self.s_curseur1.value()))
            self.s_curseur1.setValue(self.s_curseur1.value())
            self.s_curseur2.setValue(self.s_curseur1.value())
            

    def update_tours(self):
        try:
            print("update_tours appelé")
            global PORT, ESP32_IP
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect((ESP32_IP, PORT))
            s.sendall("nb_tours\n".encode())
            data = s.recv(1024).decode().strip()
            print("Reçu :", repr(data))
            liste_data = data.split(";")

            self.b_nb_tour_val.setText(liste_data[0])
            self.b_nb_tour_g_val.setText(liste_data[1])
            self.b_nb_tour_d_val.setText(liste_data[2])

            s.close()

        except Exception as e:
            print("ERREUR DANS update_tours :", e)

          

    def mode_normal(self):
        global PORT,ESP32_IP, mode
         # changer de mode du robot sur l'esp32
        if mode == "standard" :
            mode = "test"
            print("test")
            self.timer.stop()
        elif mode == "test" :
            mode = "standard"
            print("standard")
            self.timer.start(1000)

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"etat_base".encode())  # envoie commande
        s.close()

    # dit à l'esp32 de réinitialiser les valeurs --------------
    def init_valeurs (self) :  
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"init_valeurs".encode())  # envoie commande
        s.close()

    def tourne (self) :  
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"change".encode())  # envoie commande
        s.close()

    # envoyer un message à l'esp32 ---------------------------
    def envoyer_msg (self) :

        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        mot1 = self.s_curseur1.value()
        mot2 = self.s_curseur2.value()
        s.sendall(f"m1:{mot1};m2:{mot2};".encode())  # envoie commande
        s.close()

    # faire monter le robot en lui envoyant la taille de l'escalier et la vitesse de montée
    def monte (self) :
        self.popup = Popup_donnees()
        self.popup.show()

    def stop_moteurs (self) :

        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"m1:0;m2:0;\n".encode())  # envoie commande
        self.s_curseur1.setValue(0)
        self.s_curseur2.setValue(0)
        s.close()

    # avoir des infos sur l'état du robot --------------------------

    def info_etat (self) :  
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"info_etat".encode())  # envoie commande
        time.sleep(1.5)
        data = s.recv(1024)
        print("ESP32:", data.decode().strip())

        s.close()



    # afficher les infos ICM dans le but de faire dees graphiques sur MatLab

    def afficher_icm (self) :  
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"afficher_icm".encode())  # envoie commande
        s.close()


    # stopper les valeurs icm ------------------------------------------- 

    def stop_icm (self) :  
        global PORT,ESP32_IP
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP32_IP, PORT))
        s.sendall(f"stop".encode())  # envoie commande
        s.close()


app = QApplication(sys.argv)
window = MainWindow()
window.show()
sys.exit(app.exec())