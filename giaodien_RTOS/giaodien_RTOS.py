from PyQt5.QtWidgets import *
from PyQt5 import uic
import serial
import sys
import serial.tools.list_ports
import time

global ser


class UI(QMainWindow):
    def __init__(self):
        super(UI, self).__init__()

        uic.loadUi("giaodien_RTOS.ui", self)
        self.button = self.findChild(QPushButton, "pushButton")
        self.dulieu = self.findChild(QLineEdit,"lineEdit")
        self.text = self. findChild(QTextBrowser,"textBrowser")
        self.command = self.findChild(QCheckBox, "box_command")
        self.thoigian = self.findChild(QCheckBox, "box_thoigian")
        self.nhietdo = self.findChild(QCheckBox, "box_nhietdo")
        self.button_COM = self.findChild(QPushButton, "pushButton_2")
        self.box_COM = self.findChild(QComboBox, "comboBox")

        self.command.setCheckState(2)
        self.button.clicked.connect(self.clicker)
        self.command.stateChanged.connect(self.set_command)
        self.button_COM.clicked.connect(self.load_COM)
        self.show()


    def load_COM(self):
        i = 0
        ports = list(serial.tools.list_ports.comports())
        self.box_COM.clear()
        for p in ports:
            self.box_COM.addItem("")
            self.box_COM.setItemText(i, str(p))
            i +=1
    def set_command(self):
        self.dulieu.setText("") 
        if(self.command.checkState()):
            self.dulieu.setEnabled(1)
        else:
            self.dulieu.setEnabled(0)
    def clicker(self):
        global ser
        try:
            ser = serial.Serial(self.box_COM.currentText()[0:5],9600)
            a = ""
            print("dc")
            self.text.clear()
            if(self.thoigian.checkState()):
                a = "&"
                self.text.append("thoi gian")
            if(self.nhietdo.checkState()):
                a += "@"
                self.text.append("nhiet do")
            a += self.dulieu.displayText()
            self.text.append(self.dulieu.displayText())
            ser.write(a.encode("ascii"))
            #self.text.setText(a)
            print("ok")
            time.sleep(0.1)
            ser.close()
        except:
            pass
app = QApplication(sys.argv)
UIWindow = UI()
app.exec_()