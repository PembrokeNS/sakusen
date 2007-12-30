#!/usr/bin/env python
import sys,string,re
from sakusen import *
from sakusencomms import *
from connectDialog import Ui_connectDialog
from settingsDialog import Ui_settingsDialog
from PyQt4 import QtGui,QtCore
Socket_socketsInit()
timeout=timeval(5,0)
#@tCore.pyqtSignature("join(QString)")
class settingsItem:
	def __init__(self,d):
		self.pathstr=d.getSetting()
		self.path=d.getSetting().split(':')
		self.value=d.getValue()
		self.name=self.path[-1]
		#if(self.name): self.parent=string.join(self.path[:-2],':')
		if(re.search("\d+ items",self.value)):
			self.children=self.value.split("\n")[1:]
			self.data=""
		else:
			self.children=[]
			self.data=self.value
def receiveMessageOfType(socket, mtype):
	while(True):
		buf=uint8(BUFFER_LEN)
		l=s.receiveTimeout(buf,BUFFER_LEN,timeout)
		i=IArchive(buf,l)
		r=Message(i)
		t=r.getType()
		if(t==mtype):
			return r
		else:
			unexpectedMessage(r)
def unexpectedMessage(m):
	print "Got unexpected message of type %d"%m.getType()

def getSetting(s,setting):
	d=GetSettingMessageData(str(setting))
	m=Message(d)
	d.thisown=False
	s.send(m)
	buf=uint8(BUFFER_LEN)
	while(True):
		l=s.receiveTimeout(buf,BUFFER_LEN,timeout)
		i=IArchive(buf,l)
		r=Message(i)
		t=r.getType()
		if(t==messageType_notifySetting):
			return r.getNotifySettingData()
		else:
			unexpectedMessage(r)

class settingsTreeModel(QtCore.QAbstractItemModel):
	def __init__(self,socket):
		self.socket=socket
		d={():settingsItem(getSetting(socket,':'))}
	def data(self,index,role):
		if(index.column==0):
			return self.d[index.internalPointer()].name
		else:
			return self.d[index.internalPointer()].data
	def columnCount(self,parent):
		return 2
	def rowCount(self,parent):
		return len(self.d[parent.internalPointer()].children)
	def parent(self,index):
		return self.createIndex(d[tuple(index.internalPointer().path[:-2])].index(index.internalPointer().path[-2]),0,d[tuple(index.internalPointer().path[:-1])])
	def index(self,row,column,parent):
		p=tuple(d[parent.internalPointer()].path+d[parent.internalPointer()].children[row])
		if(not p in d):
			d[p]=getSetting(self.socket,string.join(p,':'))
		return self.createIndex(row,column,d[p])
	def root(self):
		return self.createIndex(0,0,self.d[()])

def join(addr):
	d=JoinMessageData("")
	m=Message(d)
	d.thisown=False #m now owns d
	s=Socket_newConnectionToAddress(addr)
	s.send(m)
	while(True):
		buf=uint8(BUFFER_LEN)
		l=s.receiveTimeout(buf,BUFFER_LEN,timeout)
		i=IArchive(buf,l)
		r=Message(i)
		t=r.getType()
		if(t==messageType_accept):
#			print "about to return"
			return s
		else:
			unexpectedMessage(r)
app = QtGui.QApplication(sys.argv)
window = QtGui.QDialog()
uiold = Ui_connectDialog()
nwindow=QtGui.QDialog()
def connecttoserver():
	global nwindow
#	print "Connecting to server"
	s=join(str(uiold.address.text()))
	ui=Ui_settingsDialog()
	ui.setupUi(nwindow)
	#settingsTree=QtGui.QListView(nwindow)
	#m=settingsTreeModel(s)
	#settingsTree.setModel(m)
	#settingsTree.setRootIndex(m.root())
	#for now, will just go through the entire settings tree
	def recurse(i,n):
		j=settingsItem(getSetting(s,i.text(0)+':'+n))
		if(j.data):
			l=QtCore.QStringList()
			l.append(j.name)
			l.append(j.data)
			i.addChild(QtGui.QTreeWidgetItem(l))
		else:
			l=QtCore.QStringList()
			l.append(j.pathstr)
			l.append('')
			w=QtGui.QTreeWidgetItem(l)
			i.addChild(w)
			for c in j.children:
				recurse(w,c)
				
	l=QtCore.QStringList()
	l.append('')
	l.append('')
	i=QtGui.QTreeWidgetItem(l)
	ui.settingsTree.addTopLevelItem(i)
	recurse(i,'')
	
	nwindow.show()
#	print "doing fine"
uiold.setupUi(window)
QtCore.QObject.connect(window,QtCore.SIGNAL("accepted()"), connecttoserver)
window.show()
sys.exit(app.exec_())
