import sys
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget,QSlider,
    QHBoxLayout, QVBoxLayout, QPushButton,QGridLayout,
    QGraphicsView, QGraphicsScene, QLineEdit,QLabel
)
from PyQt6.QtCore import (Qt,QTimer)
from PyQt6.QtGui import QBrush, QPen
from PyQt6.QtGui import QColor
from PyQt6.QtNetwork import QTcpSocket, QHostAddress, QAbstractSocket
import pyqtgraph as pg


from style import STYLE
from popup import Popup_donnees, Popup_tourne, Popup_avance_controlee, Popup_plateforme, Popup_direction_aspi

import socket
import time


ESP32_IP = "192.168.4.1"  # l'IP ESP32
PORT = 1234     # port pour les commandes
PORT_ICM = 1235     # port pour les donnees ICM

mode = "test"
nb_marche = 0
nb_tour_av = 0
nb_tour_d = 0
nb_tour_g = 0
etat_sync = 0     # 0 su les roues sont désynchronisée et 1 si elles sont synchronisées

class MainWindow(QMainWindow):
    def __init__(self):

        
        super().__init__()

        # initialisation des listes de données pour le graphique
        self.donnee_x = [0,1]
        self.donnee_y = [0,0]
        self.max_points = 100

        self.setStyleSheet(STYLE)       

        self.setWindowTitle("Interface Graphique")
        self.setGeometry(200, 50, 800, 700)

        # Widget central
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        main_layout = QHBoxLayout()
        central_widget.setLayout(main_layout)

        # création des layout
        left_layout = QVBoxLayout()
        layout_centre = QVBoxLayout()
        right_layout = QVBoxLayout()
        bottom_layout = QGridLayout()
        curseur_layout = QGridLayout()


        # boutons 
        self.b_monter_platforme = QPushButton("monter plateforme")
        self.b_descendre_platforme = QPushButton("descendre plateforme")
        self.b_tourne = QPushButton("tourne")
        self.b_info_etat = QPushButton("info etat")
        self.b_info_etat.setObjectName("test")
        # self.b_afficher_icm = QPushButton("afficher icm")
        # self.b_afficher_icm.setObjectName("affichage")
        # self.b_stop = QPushButton("stopper affichage ICM")
        # self.b_stop.setObjectName("affichage")
        self.b_init = QPushButton("reinit. les valeurs")
        self.b_envoyer = QPushButton("MOTEURS")
        self.b_avance_controlee = QPushButton("avance contrôlée")
        self.b_monte = QPushButton("monter")
        self.b_stop_moteurs = QPushButton("stop moteurs")
        self.b_sync = QPushButton("synchro")
        #self.text1 = QLineEdit()
        #self.text2 = QLineEdit()

        self.b_drone = QPushButton("mouvement DRONE")
        self.b_aspi_drone = QPushButton("aspiration")

        left_layout.addWidget(self.b_info_etat)
        # left_layout.addWidget(self.b_afficher_icm)
        # left_layout.addWidget(self.b_stop)
        left_layout.addWidget(self.b_monte)
        left_layout.addWidget(self.b_tourne)
        left_layout.addWidget(self.b_avance_controlee)
        left_layout.addWidget(self.b_monter_platforme)
        #left_layout.addWidget(self.b_descendre_platforme)
        left_layout.addWidget(self.b_envoyer)
        left_layout.addWidget(self.b_stop_moteurs)
        #left_layout.addWidget(self.b_init)

        right_layout.addWidget(self.b_drone)
        right_layout.addWidget(self.b_aspi_drone)

        #left_layout.addWidget(self.text1)
        #left_layout.addWidget(self.text2)

        left_layout.addStretch()
        right_layout.addStretch()

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
        

        self.s_curseur1.valueChanged.connect(self.valeurChange)
        self.s_curseur2.valueChanged.connect(self.valeurChange)
        

        # zone de graphique
        self.scene = QGraphicsScene()
        self.view = QGraphicsView(self.scene)

        layout_centre.addWidget(self.view, 4)


        # placement des layout
        main_layout.addLayout(left_layout, 1)
        main_layout.addLayout(layout_centre, 5)
        main_layout.addLayout(right_layout, 1)
        layout_centre.addLayout(bottom_layout, 1)
        left_layout.addLayout(curseur_layout, 1)
        left_layout.addWidget(self.b_sync)
        main_layout.setSpacing(10)
        main_layout.setContentsMargins(10, 10, 10, 10)
        layout_centre.setContentsMargins(50, 10, 50, 10)
        

    

        # zone de plot 
      
        self.b_nb_tour = QLabel("nombre de marches montées")
        self.b_nb_tour_val = QLabel(str(nb_marche))
        self.b_nb_tour_d = QLabel("action en cours")
        self.b_nb_tour_d_val = QLabel("")
        self.b_nb_tour_g = QLabel("connection")
        self.b_nb_tour_g_val = QLabel("")
        bottom_layout.addWidget(self.b_nb_tour,0,0)
        bottom_layout.addWidget(self.b_nb_tour_val,0,1)
        bottom_layout.addWidget(self.b_nb_tour_d,1,0)
        bottom_layout.addWidget(self.b_nb_tour_d_val,1,1)
        bottom_layout.addWidget(self.b_nb_tour_g,2,0)
        bottom_layout.addWidget(self.b_nb_tour_g_val,2,1)


        # affecte une fonction à un bouton
        self.b_monter_platforme.clicked.connect(self.monter_platforme)
        self.b_avance_controlee.clicked.connect(self.popup_avance_controlee)
        #self.b_descendre_platforme.clicked.connect(self.descendre_platforme)
        self.b_info_etat.clicked.connect(self.info_etat)
        # self.b_afficher_icm.clicked.connect(self.afficher_icm)
        # self.b_stop.clicked.connect(self.stop_icm)
        self.b_monte.clicked.connect(self.monte)
        self.b_tourne.clicked.connect(self.tourne)
        self.b_envoyer.clicked.connect(self.envoyer_msg)
        self.b_stop_moteurs.clicked.connect(self.stop_moteurs)
        self.b_init.clicked.connect(self.init_valeurs)
        self.b_sync.clicked.connect(self.sync)

        self.b_drone.clicked.connect(self.popup_drone)

        #graphique
        self.plot_widget = pg.PlotWidget()
        self.curve = self.plot_widget.plot()
        layout_centre.addWidget(self.plot_widget)
        self.curve.setData(self.donnee_x, self.donnee_y)

        # socket persistante pour recevoir le flux ICM en continu

        self.link = QTcpSocket(self)
        self.link.readyRead.connect(self.recevoir_flux)
        self.link.errorOccurred.connect(self.on_link_error)
        self.link.connected.connect(lambda: self.b_nb_tour_g_val.setText("Connecté"))
        self.link.disconnected.connect(lambda: self.b_nb_tour_g_val.setText("Déconnecté"))
        self.link.connectToHost(ESP32_IP, PORT)

        

        """
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_tours)
        print("démarrage")
        
        # a tester
        # self.timer.start(4000)  # 1000 ms = 1 seconde
        """

    
    def recevoir_flux(self):
        while self.link.canReadLine():
            data = self.link.readLine().data().decode(errors="ignore").strip()
            if not data:
                continue
            champs = data.split(";")
            type_msg = champs[0]

            if type_msg == "ICM":
                self.maj_graphique(float(champs[1]))
            elif type_msg == "ETAT":
                self.b_nb_tour_val.setText(champs[1])
                self.b_nb_tour_d_val.setText(champs[2])
            else:
                print("Reçu :", data)

    def envoyer_commande(self, *champs):
        ligne = ";".join(str(c) for c in champs) + "\n"
        self.link.write(ligne.encode())


    def maj_graphique(self, x):
        self.donnee_x.append(x)
        self.donnee_y.append(self.donnee_y[-1]+1)

        if len(self.donnee_x) > self.max_points :
            self.donnee_x = self.donnee_x[-self.max_points:]
            self.donnee_y = self.donnee_y[-self.max_points:]       

        self.curve.setData(self.donnee_y, self.donnee_x)

    

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
            

    def monter_platforme(self) :
        self.popup = Popup_plateforme(self.link)
        self.popup.show()
    
    def popup_drone(self) :
        self.popup = Popup_direction_aspi(self.link)
        self.popup.show()

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
        s.sendall(f"etat_base\n".encode())  # envoie commande
        s.close()

    # dit à l'esp32 de réinitialiser les valeurs --------------
    def init_valeurs (self) :  
        self.envoyer_commande("init_valeurs")

    def tourne (self) :  
        self.popup = Popup_tourne(self.link)
        self.popup.show()

    # envoyer un message à l'esp32 ---------------------------
    def envoyer_msg (self) :
        self.envoyer_commande("moteur", self.s_curseur1.value(), self.s_curseur2.value())

    # faire monter le robot en lui envoyant la taille de l'escalier et la vitesse de montée
    def monte (self) :
        self.popup = Popup_donnees(self.link)
        self.popup.show()

    def popup_avance_controlee (self) :
        self.popup = Popup_avance_controlee(self.link)
        self.popup.show()


    def stop_moteurs (self) :
        self.envoyer_commande("moteur", 0, 0)

    # avoir des infos sur l'état du robot --------------------------
    def info_etat (self) :  
        self.envoyer_commande("INFO_ETAT")



app = QApplication(sys.argv)
window = MainWindow()
window.show()
sys.exit(app.exec())