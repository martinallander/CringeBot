#!/usr/bin/env python

#GUI - CringeBOT
#Mathias Kindstedt
#2018-04-13

import sys, random   #Random anvands bara for att testa
from PyQt4 import QtGui, QtCore

#Klass for att skapa IR-rutorna
class IR(QtGui.QWidget):
	def __init__(self, parent = None):
		QtGui.QWidget.__init__(self, parent)
		#Variabel for temperatur, rutor v->h, upp->ner
		self.temp = [10] * 64
		#Initialvarde
		for i in range(64):
			self.temp[i] += i/2

	#inTemp ar en array[64]
	def setTemp(self, inTemp = 0):
		#self.temp = inTemp
		#Nedan bara for test
		for i in range(64):
			self.temp[i] += (random.randint(0, 10) / 10 - 0.10)
		self.update()

	#Ritar ut rutorna
	def paintEvent(self, event):
		painter = QtGui.QPainter(self)
		rect = self.contentsRect() #Storlek pa widgeten
		side = min(rect.height() / 8, rect.width() / 8)
		#Background, svart
		painter.fillRect(0, 0, side*8, side*8, QtGui.QColor(0,0,0))

		#Ritar varje ruta, en ram pa en pixel runt
		for i in range(8):
			for j in range(8):
				square = j + i * 8
				painter.fillRect(side*j + 1, side*i + 1, 
					side - 2, side - 2, self.calcColor(square))

	#Berakna farg baserat pa vilken ruta
	def calcColor(self, sq):
		#Ironbow-palette
		colorTable = [0x00000a,0x000014,0x00001e,0x000025,
		0x00002a,0x00002e,0x000032,0x000036,
		0x00003a,0x00003e,0x000042,0x000046,
		0x00004a,0x00004f,0x000052,0x010055,
		0x010057,0x020059,0x02005c,0x03005e,
		0x040061,0x040063,0x050065,0x060067,
		0x070069,0x08006b,0x09006e,0x0a0070,
		0x0b0073,0x0c0074,0x0d0075,0x0d0076,
		0x0e0077,0x100078,0x120079,0x13007b,
		0x15007c,0x17007d,0x19007e,0x1b0080,
		0x1c0081,0x1e0083,0x200084,0x220085,
		0x240086,0x260087,0x280089,0x2a0089,
		0x2c008a,0x2e008b,0x30008c,0x32008d,
		0x34008e,0x36008e,0x38008f,0x390090,
		0x3b0091,0x3c0092,0x3e0093,0x3f0093,
		0x410094,0x420095,0x440095,0x450096,
		0x470096,0x490096,0x4a0096,0x4c0097,
		0x4e0097,0x4f0097,0x510097,0x520098,
		0x540098,0x560098,0x580099,0x5a0099,
		0x5c0099,0x5d009a,0x5f009a,0x61009b,
		0x63009b,0x64009b,0x66009b,0x68009b,
		0x6a009b,0x6c009c,0x6d009c,0x6f009c,
		0x70009c,0x71009d,0x73009d,0x75009d,
		0x77009d,0x78009d,0x7a009d,0x7c009d,
		0x7e009d,0x7f009d,0x81009d,0x83009d,
		0x84009d,0x86009d,0x87009d,0x89009d,
		0x8a009d,0x8b009d,0x8d009d,0x8f009c,
		0x91009c,0x93009c,0x95009c,0x96009b,
		0x98009b,0x99009b,0x9b009b,0x9c009b,
		0x9d009b,0x9f009b,0xa0009b,0xa2009b,
		0xa3009b,0xa4009b,0xa6009a,0xa7009a,
		0xa8009a,0xa90099,0xaa0099,0xab0099,
		0xad0099,0xae0198,0xaf0198,0xb00198,
		0xb00198,0xb10197,0xb20197,0xb30196,
		0xb40296,0xb50295,0xb60295,0xb70395,
		0xb80395,0xb90495,0xba0495,0xba0494,
		0xbb0593,0xbc0593,0xbd0593,0xbe0692,
		0xbf0692,0xbf0692,0xc00791,0xc00791,
		0xc10890,0xc10990,0xc20a8f,0xc30a8e,
		0xc30b8e,0xc40c8d,0xc50c8c,0xc60d8b,
		0xc60e8a,0xc70f89,0xc81088,0xc91187,
		0xca1286,0xca1385,0xcb1385,0xcb1484,
		0xcc1582,0xcd1681,0xce1780,0xce187e,
		0xcf187c,0xcf197b,0xd01a79,0xd11b78,
		0xd11c76,0xd21c75,0xd21d74,0xd31e72,
		0xd32071,0xd4216f,0xd4226e,0xd5236b,
		0xd52469,0xd62567,0xd72665,0xd82764,
		0xd82862,0xd92a60,0xda2b5e,0xda2c5c,
		0xdb2e5a,0xdb2f57,0xdc2f54,0xdd3051,
		0xdd314e,0xde324a,0xde3347,0xdf3444,
		0xdf3541,0xdf363d,0xe0373a,0xe03837,
		0xe03933,0xe13a30,0xe23b2d,0xe23c2a,
		0xe33d26,0xe33e23,0xe43f20,0xe4411d,
		0xe4421c,0xe5431b,0xe54419,0xe54518,
		0xe64616,0xe74715,0xe74814,0xe74913,
		0xe84a12,0xe84c10,0xe84c0f,0xe94d0e,
		0xe94d0d,0xea4e0c,0xea4f0c,0xeb500b,
		0xeb510a,0xeb520a,0xeb5309,0xec5409,
		0xec5608,0xec5708,0xec5808,0xed5907,
		0xed5a07,0xed5b06,0xee5c06,0xee5c05,
		0xee5d05,0xee5e05,0xef5f04,0xef6004,
		0xef6104,0xef6204,0xf06303,0xf06403,
		0xf06503,0xf16603,0xf16603,0xf16703,
		0xf16803,0xf16902,0xf16a02,0xf16b02,
		0xf16b02,0xf26c01,0xf26d01,0xf26e01,
		0xf36f01,0xf37001,0xf37101,0xf37201,
		0xf47300,0xf47400,0xf47500,0xf47600,
		0xf47700,0xf47800,0xf47a00,0xf57b00,
		0xf57c00,0xf57e00,0xf57f00,0xf68000,
		0xf68100,0xf68200,0xf78300,0xf78400,
		0xf78500,0xf78600,0xf88700,0xf88800,
		0xf88800,0xf88900,0xf88a00,0xf88b00,
		0xf88c00,0xf98d00,0xf98d00,0xf98e00,
		0xf98f00,0xf99000,0xf99100,0xf99200,
		0xf99300,0xfa9400,0xfa9500,0xfa9600,
		0xfb9800,0xfb9900,0xfb9a00,0xfb9c00,
		0xfc9d00,0xfc9f00,0xfca000,0xfca100,
		0xfda200,0xfda300,0xfda400,0xfda600,
		0xfda700,0xfda800,0xfdaa00,0xfdab00,
		0xfdac00,0xfdad00,0xfdae00,0xfeaf00,
		0xfeb000,0xfeb100,0xfeb200,0xfeb300,
		0xfeb400,0xfeb500,0xfeb600,0xfeb800,
		0xfeb900,0xfeb900,0xfeba00,0xfebb00,
		0xfebc00,0xfebd00,0xfebe00,0xfec000,
		0xfec100,0xfec200,0xfec300,0xfec400,
		0xfec500,0xfec600,0xfec700,0xfec800,
		0xfec901,0xfeca01,0xfeca01,0xfecb01,
		0xfecc02,0xfecd02,0xfece03,0xfecf04,
		0xfecf04,0xfed005,0xfed106,0xfed308,
		0xfed409,0xfed50a,0xfed60a,0xfed70b,
		0xfed80c,0xfed90d,0xffda0e,0xffda0e,
		0xffdb10,0xffdc12,0xffdc14,0xffdd16,
		0xffde19,0xffde1b,0xffdf1e,0xffe020,
		0xffe122,0xffe224,0xffe226,0xffe328,
		0xffe42b,0xffe42e,0xffe531,0xffe635,
		0xffe638,0xffe73c,0xffe83f,0xffe943,
		0xffea46,0xffeb49,0xffeb4d,0xffec50,
		0xffed54,0xffee57,0xffee5b,0xffee5f,
		0xffef63,0xffef67,0xfff06a,0xfff06e,
		0xfff172,0xfff177,0xfff17b,0xfff280,
		0xfff285,0xfff28a,0xfff38e,0xfff492,
		0xfff496,0xfff49a,0xfff59e,0xfff5a2,
		0xfff5a6,0xfff6aa,0xfff6af,0xfff7b3,
		0xfff7b6,0xfff8ba,0xfff8bd,0xfff8c1,
		0xfff8c4,0xfff9c7,0xfff9ca,0xfff9cd,
		0xfffad1,0xfffad4,0xfffbd8,0xfffcdb,
		0xfffcdf,0xfffde2,0xfffde5,0xfffde8,
		0xfffeeb,0xfffeee,0xfffef1,0xfffef4,0xfffff6]

		#Varje farg i ironbow motsvarar 0.1 grad mellan 10 och 53.3 grader
		if self.temp[sq] < 10:
			return QtGui.QColor(colorTable[0])
		elif self.temp[sq] > 53.3:
			return QtGui.QColor(colorTable[432])
		else:
			return QtGui.QColor(colorTable[
				int(round(10 * (self.temp[sq] - 10)))])

	def sizeHint(self):
		return QtCore.QSize(480, 480)

#Klass for bilden
class Label(QtGui.QLabel):
	def __init__(self, img):
		super(Label, self).__init__()
		#Laddar bild fran disk
		self.pixmap = QtGui.QPixmap(img)

	#Ritar ut bild och fixar storleksandringar
	def paintEvent(self, event):
		size = self.size()
		painter = QtGui.QPainter(self)
		point = QtCore.QPoint(0,0)
		scaledPix = self.pixmap.scaled(size, QtCore.Qt.KeepAspectRatio, 
			transformMode = QtCore.Qt.SmoothTransformation)
		point.setX((size.width() - scaledPix.width())/2)
		point.setY((size.height() - scaledPix.height())/2)
		painter.drawPixmap(point, scaledPix)

#Klass for hela skarmen 9:16
#A A A A A A A A D D D E E E E E
#A A A A A A A A D D D E E E E E
#A A A A A A A A D D D E E E E E
#A A A A A A A A D D D E E E E E
#A A A A A A A A D D D E E E E E
#A A A A A A A A D D D F F F F F
#A A A A A A A A D D D F F F F F
#B B B B B C C C D D D F F F F F
#B B B B B C C C D D D F F F F F
#A - Karta, B - Status, C - Knappar
#D - Kommandon, E - IR, F - Sensorer
class GUI(QtGui.QMainWindow):
	def __init__(self):
		super(GUI,self).__init__()
		self.initUI()
		self.done = 1 #Variabel for om roboten har lost sin uppgift

	def initUI(self):
		#Huvudfonster, spacing 8 px mellan moduler
		grid = QtGui.QGridLayout()
		grid.setSpacing(8)

		#Fonter for text och knappar, palett for kommandolistan
		font = QtGui.QFont("Arial", 24, QtGui.QFont.Bold)
		fontKey = QtGui.QFont("Calibri", 24, QtGui.QFont.Bold)
		palette = QtGui.QPalette()
		palette.setColor(QtGui.QPalette.Base, QtGui.QColor(0, 0, 0))
		palette.setColor(QtGui.QPalette.Text, QtGui.QColor(0, 255, 0))

		### MAP ###
		### MAP ###
		### MAP ###

		label = Label("map.png")
		grid.addWidget(label, 0, 0, 7, 8)
		
		### STATUS ###
		### STATUS ###
		### STATUS ###

		#VBoxLayout, med statusknapp och label i HBox on top
		status = QtGui.QVBoxLayout()

		#Knapp for att starta, kopplad till funktion start()
		self.startBtn = QtGui.QPushButton("Start", self)
		self.startBtn.setStyleSheet(
			'QPushButton {padding:7px;background-color: #000000;color: #00FF00;}')
		self.startBtn.setFont(font)
		self.startBtn.pressed.connect(self.start)

		#Label som visar statusen pa robot, manuellt, klar etc.
		self.statusLabel = QtGui.QLabel(' V'u"\u00E4"'ntar p'u"\u00E5"' start ', 
			self)
		self.statusLabel.setFont(font)
		self.statusLabel.setStyleSheet(
			"QLabel { background-color : #00FF00; color : #000000;}");

		#Diod for att visa att nodstalld ar hittad
		self.found = QtGui.QRadioButton('N'u"\u00F6"'dst'u"\u00E4"'lld ombord', 
			self)
		self.found.setFont(font)
		self.found.setStyleSheet(
			'QRadioButton {spacing: 10px;}'
			'QRadioButton::indicator {width: 26px;height: 26px;}'
			'QRadioButton::indicator {border-style:solid;border-width:3px;}'
			'QRadioButton::indicator {border-radius:15px;border-color: #000000;}'
			'QRadioButton::indicator {background: #808080;}'
			'QRadioButton::indicator:checked{border:3px solid #000000;background: #00FFFF;}')
		self.found.setAttribute(QtCore.Qt.WA_TransparentForMouseEvents) #kom. nedan
		self.found.setFocusPolicy(QtCore.Qt.NoFocus) #Inte klickbar med mus

		#Knapp for att sla pa/av autonomt laga, kopplad till funktion setStatus()
		self.anatom = QtGui.QCheckBox('Autonomt l'u"\u00E4"'ge', self)
		self.anatom.setFont(font)
		self.anatom.stateChanged.connect(self.setStatus) 
		self.anatom.setStyleSheet(
			'QCheckBox {spacing: 10px;}'
			'QCheckBox::indicator {width: 24px;height: 24px;}'
			'QCheckBox::indicator {border: 3px solid #000000;background: #808080;}'
			'QCheckBox::indicator:checked {border:3px solid #000000;}'
			'QCheckBox::indicator:checked {background: #00FF00;}')
		
		#HBox
		startLayout = QtGui.QHBoxLayout()
		startLayout.addWidget(self.startBtn)
		startLayout.addWidget(self.statusLabel)
		startLayout.setSpacing(20)
		#VBox
		status.addLayout(startLayout)
		status.addWidget(self.found)
		status.addWidget(self.anatom)
		status.setSpacing(12)
		status.setMargin(8)
		status.setAlignment(startLayout, QtCore.Qt.AlignHCenter)
		grid.addLayout(status, 7, 0, 2, 5)

		### STYR ###
		### STYR ###
		### STYR ###

		keys = QtGui.QGridLayout()
		#Skapa knappar
		self.forwardBtn = QtGui.QPushButton(u"\u2191", self)
		self.backwardBtn = QtGui.QPushButton(u"\u2193", self)
		self.rightBtn = QtGui.QPushButton(u"\u2192", self)
		self.leftBtn = QtGui.QPushButton(u"\u2190", self)
		self.rotRightBtn = QtGui.QPushButton(u"\u2197", self)
		self.rotLeftBtn = QtGui.QPushButton(u"\u2196", self)

		#Koppla till varsin on-funktion
		self.forwardBtn.pressed.connect(self.forward)
		self.backwardBtn.pressed.connect(self.backward)
		self.leftBtn.pressed.connect(self.left)
		self.rightBtn.pressed.connect(self.right)
		self.rotLeftBtn.pressed.connect(self.rotLeft)
		self.rotRightBtn.pressed.connect(self.rotRight)

		#Koppla till varsin off-funktion
		self.forwardBtn.released.connect(self.quitForward)
		self.backwardBtn.released.connect(self.quitBackward)
		self.leftBtn.released.connect(self.quitLeft)
		self.rightBtn.released.connect(self.quitRight)
		self.rotLeftBtn.released.connect(self.quitRotLeft)
		self.rotRightBtn.released.connect(self.quitRotRight)

		#Font och form
		self.forwardBtn.setSizePolicy(QtGui.QSizePolicy.Preferred,
			QtGui.QSizePolicy.Expanding) # For att expandera i hojdled
		self.forwardBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
		self.backwardBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
		self.rightBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
		self.leftBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
		self.rotRightBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
		self.rotLeftBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
		self.forwardBtn.setFont(fontKey)
		self.backwardBtn.setFont(fontKey)
		self.leftBtn.setFont(fontKey)
		self.rightBtn.setFont(fontKey)
		self.rotLeftBtn.setFont(fontKey)
		self.rotRightBtn.setFont(fontKey)

		# RL  UPP  RR
		#  L  UPP  R
		#     NER
		keys.addWidget(self.rotLeftBtn, 0, 0, 1, 1)
		keys.addWidget(self.forwardBtn, 0, 1, 2, 1)
		keys.addWidget(self.rotRightBtn, 0, 2, 1, 1)
		keys.addWidget(self.leftBtn, 1, 0, 1, 1)
		keys.addWidget(self.rightBtn, 1, 2, 1, 1)
		keys.addWidget(self.backwardBtn, 2, 1, 1, 1)
		grid.addLayout(keys, 7, 5, 2, 3)

		### KOMMANDO ###
		### KOMMANDO ###
		### KOMMANDO ###

		#textruta som tar upp hela skarmen vertikalt, gar inte att skriva
		self.kommando = QtGui.QTextEdit()
		self.kommando.setReadOnly(True)
		self.kommando.setPalette(palette)
		self.kommando.setFontPointSize(12)
		grid.addWidget(self.kommando, 0, 8, 9, 3)

		### IR ###
		### IR ###
		### IR ###

		#Skapar en IR-panel
		ir = QtGui.QVBoxLayout()
		self.irGrid = IR(self)
		ir.addWidget(self.irGrid)
		grid.addLayout(ir, 0, 11, 5, 5)
		grid.setAlignment(ir, QtCore.Qt.AlignCenter)

		### SENSORER ###
		### SENSORER ###
		### SENSORER ###

		#Skapa nummerpaneler
		sensors = QtGui.QGridLayout()
		self.accx = QtGui.QLCDNumber(self)
		self.accy = QtGui.QLCDNumber(self)
		self.accz = QtGui.QLCDNumber(self)
		self.gyrox = QtGui.QLCDNumber(self)
		self.gyroy = QtGui.QLCDNumber(self)
		self.gyroz = QtGui.QLCDNumber(self)
		self.tofValue = QtGui.QLCDNumber(7, self)
		self.setAcc("LuL", "1337", "PR1DE")
		self.setGyro("'-'", "Olof", "o-d")
		self.setTOF("Alfred")

		#Skapa textpaneler och satt font
		x = QtGui.QLabel('x: ')
		y = QtGui.QLabel('y: ')
		z = QtGui.QLabel('z: ')
		acc = QtGui.QLabel('Acc:')
		gyro = QtGui.QLabel('Gyro:')
		tof = QtGui.QLabel('TOF: ')
		x.setFont(font)
		y.setFont(font)
		z.setFont(font)
		acc.setFont(font)
		gyro.setFont(font)
		tof.setFont(font)
		x.setAlignment(QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter)
		y.setAlignment(QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter)
		z.setAlignment(QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter)
		acc.setAlignment(QtCore.Qt.AlignHCenter | QtCore.Qt.AlignBottom)
		gyro.setAlignment(QtCore.Qt.AlignHCenter | QtCore.Qt.AlignBottom)
		tof.setAlignment(QtCore.Qt.AlignRight | QtCore.Qt.AlignVCenter)

		sensors.addWidget(tof, 0, 0)
		sensors.addWidget(self.tofValue, 0, 1, 1, 4)
		sensors.addWidget(acc, 2, 1, 1, 2)
		sensors.addWidget(gyro, 2, 3, 1, 2)
		sensors.addWidget(x, 3, 0)
		sensors.addWidget(self.accx, 3, 1, 1, 2)
		sensors.addWidget(self.gyrox, 3, 3, 1, 2)
		sensors.addWidget(y, 4, 0)
		sensors.addWidget(self.accy, 4, 1, 1, 2)
		sensors.addWidget(self.gyroy, 4, 3, 1, 2)
		sensors.addWidget(z, 5, 0)
		sensors.addWidget(self.accz, 5, 1, 1, 2)
		sensors.addWidget(self.gyroz, 5, 3, 1, 2)
		grid.addLayout(sensors, 5, 11, 4, 5)

		### SETTINGS ###
		### SETTINGS ###
		### SETTINGS ###

		wid = QtGui.QWidget(self)
		self.setCentralWidget(wid)
		wid.setLayout(grid)

		QtGui.QApplication.setStyle(QtGui.QStyleFactory.create('cleanlooks'))
		self.statusBar().showMessage('Copyright (C) 2018 Mathias Kindstedt')
		self.resize(1600,930)
		self.center() #Centrera fonster
		self.setWindowTitle("CringeBOT")
		self.setWindowIcon(QtGui.QIcon('kappa.png'))
		self.show()

	#Centrera fonster
	def center(self):
		qr = self.frameGeometry()
		cp = QtGui.QDesktopWidget().availableGeometry().center()
		qr.moveCenter(cp)
		self.move(qr.topLeft())

	#Sure you want to quit-fonster
	def closeEvent(self, event):
		reply = QtGui.QMessageBox.question(self, 'Quit?',
			"U wanna quit?", QtGui.QMessageBox.Yes |
			QtGui.QMessageBox.No, QtGui.QMessageBox.Yes)

		if reply == QtGui.QMessageBox.Yes:
			event.accept()
		else:
			event.ignore()

	#Knapptryck
	def keyPressEvent(self, event):
		if event.key() == QtCore.Qt.Key_Escape:
			self.close()
		elif event.key() == QtCore.Qt.Key_Q:
			self.rotLeft()
		elif event.key() == QtCore.Qt.Key_W:
			self.forward()
		elif event.key() == QtCore.Qt.Key_E:
			self.rotRight()
		elif event.key() == QtCore.Qt.Key_A:
			self.left()
		elif event.key() == QtCore.Qt.Key_S:
			self.backward()
		elif event.key() == QtCore.Qt.Key_D:
			self.right()
		elif event.key() == QtCore.Qt.Key_I:
			self.irGrid.setTemp()
		elif event.key() == QtCore.Qt.Key_M:
			self.found.toggle()
			self.setStatus()
		elif event.key() == QtCore.Qt.Key_N:
			if self.found.isChecked() == True:
				self.done = 1
				self.setStatus()

	#Knappslapp
	def keyReleaseEvent(self, event):
		if event.key() == QtCore.Qt.Key_Q:
			self.quitRotLeft()
		elif event.key() == QtCore.Qt.Key_W:
			self.quitForward()
		elif event.key() == QtCore.Qt.Key_E:
			self.quitRotRight()
		elif event.key() == QtCore.Qt.Key_A:
			self.quitLeft()
		elif event.key() == QtCore.Qt.Key_S:
			self.quitBackward()
		elif event.key() == QtCore.Qt.Key_D:
			self.quitRight()

	#Uppdaterar statusfonster
	def setStatus(self):
		self.statusLabel.setStyleSheet(
			"QLabel { background-color : #000000; color : #00FF00;}");
		if self.done == 1:
			self.statusLabel.setStyleSheet(
				"QLabel { background-color : #00FF00; color : #000000;}")
			self.statusLabel.setText(' Uppdrag slutf'u"\u00F6"'rt ')
			self.startBtn.setVisible(True)
		elif self.anatom.checkState() == 0:
			self.statusLabel.setText(' Manuell styrning ')
		elif self.found.isChecked() == True:
			self.statusLabel.setText(' 'u"\u00C5"'terv'u"\u00E4"'nder till start ')
		else:
			self.statusLabel.setText(' S'u"\u00F6"'ker efter n'
				u"\u00F6"'dst'u"\u00E4"'lld ')

	#Vad som hander nar ratt knapp trycks
	def forward(self):
		self.kommando.append(":TriHard:")
		self.forwardBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #00FF00;color: #000000;}')
	def backward(self):
		self.kommando.append(":KappaPride:")
		self.backwardBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #00FF00;color: #000000;}')
	def right(self):
		self.kommando.append(":monkaOMEGA:")
		self.rightBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #00FF00;color: #000000;}')
	def left(self):
		self.kommando.append(":PogChamp:")
		self.leftBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #00FF00;color: #000000;}')
	def rotRight(self):
		self.kommando.append(":greekBrow:")
		self.rotRightBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #00FF00;color: #000000;}')
	def rotLeft(self):
		self.kommando.append(":Kappa:")
		self.rotLeftBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #00FF00;color: #000000;}')

	#Vad som hander nar ratt knapp trycks
	def quitForward(self):
		self.forwardBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
	def quitBackward(self):
		self.backwardBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
	def quitRight(self):
		self.rightBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
	def quitLeft(self):
		self.leftBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
	def quitRotRight(self):
		self.rotRightBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')
	def quitRotLeft(self):
		self.rotLeftBtn.setStyleSheet(
			'QPushButton {padding: 7px;background-color: #000000;color: #00FF00;}')

	#Satter sensorvardena
	def setAcc(self, x, y, z):
		self.accx.display(x)
		self.accy.display(y)
		self.accz.display(z)
	def setGyro(self, x, y, z):
		self.gyrox.display(x)
		self.gyroy.display(y)
		self.gyroz.display(z)
	def setTOF(self, d):
		self.tofValue.display(d)

	#Startkommando, ta bort startknapp
	def start(self):
		self.done = 0;
		self.startBtn.setVisible(False)
		self.found.setChecked(False)
		self.setStatus()


def main():
   app = QtGui.QApplication(sys.argv)
   window = GUI()
   sys.exit(app.exec_())
	
if __name__ == '__main__':
   main()