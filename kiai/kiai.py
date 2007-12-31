#!/usr/bin/env python
import sys,string,re
from sakusen import *
from sakusencomms import *
from connectDialog import Ui_connectDialog
from settingsDialog import Ui_settingsDialog
from PyQt4 import QtGui,QtCore#,Qt
from modeltest import ModelTest
Socket_socketsInit()
timeout=timeval(5,0)
#@tCore.pyqtSignature("join(QString)")
class settingsItem:
	def __init__(self,d):
		self.pathstr=d.getSetting()
		self.path=tuple(d.getSetting().split(':'))
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
	"""A Model class for viewing the Sakusen settings tree using the Qt Model/View paradigm. Stores the settings internally as a dictionary d whose keys are tuples ('a','b','c') for 'a:b:c' and values are corresponding settingsItems.
	
	These tuples are passed around as QModelIndex.internalPointers. But python will garbage-collect them. Current hack: use d[tuple].path rather than tuple - that way the pointer should stay valid as long as the model exists"""
	def __init__(self,socket,parent=None):
		QtCore.QAbstractItemModel.__init__(self,parent) #I am a tard
		#print "called m.__init__"
		self.socket=socket
		self.d={('',):settingsItem(getSetting(socket,':'))}
		#print "did m.__init__"
	def data(self,index,role):
		#print "called m.data"
		if((not index.isValid()) or role!=QtCore.Qt.DisplayRole): return QtCore.QVariant()
		#print "about to return some data from "+repr(index.internalPointer())+" column "+repr(index.column)
		if(index.column()==0):
		#	print "going to return name:"+self.d[index.internalPointer()].name
			return QtCore.QVariant(self.d[index.internalPointer()].name)
		else:
		#	print "going to return data:"+self.d[index.internalPointer()].data
			return QtCore.QVariant(self.d[index.internalPointer()].data)
	def columnCount(self,parent):
		#if(not parent.isValid()): print "called m.columnCount on invalid index"
		#else: print "called m.columnCount on node "+repr(parent.internalPointer())
		return 2
	def rowCount(self,parent):
		#if(not parent.isValid()): print "called m.rowCount on invalid index"
		#else: print "called m.rowCount on node "+repr(parent.internalPointer())
		if(not parent.isValid()): return 1
		if(parent.column()>0): return 0
		return len(self.d[parent.internalPointer()].children)
	def parent(self,index):
		#print "called m.parent"
		if((not index.isValid()) or index.internalPointer()==('',)): return QtCore.QModelIndex()
		#print "trying to find parent for "+repr(index.internalPointer())
		#print "searching "+repr(self.d[tuple(index.internalPointer()[:-1])].children)+" for entry"
		#print "returning from m.parent"
		return self.createIndex(self.d[tuple(index.internalPointer()[:-1])].children.index(index.internalPointer()[-1]),0,self.d[tuple(index.internalPointer()[:-1])].path)
	def index(self,row,column,parent):
		#print "called m.index"
		#if(not self.hasIndex(row,column,parent)):
		#	print "some craziness ocurring"
		#	return QtCore.QModelIndex()
		#print "craziness didn't happen"
		#print "did that segfault?"
		if(not parent.isValid()):
			#arrgh arrgh surely this cannot be the right way to do this
			if(row==0 and 0<=column and 2>column): return self.createIndex(row,column,self.d[('',)].path)
			else: return self.root()
		#print "halfway through m.index"
		p=tuple(self.d[parent.internalPointer()].path+(self.d[parent.internalPointer()].children[row],))
		if(not p in self.d):
		#	print "about to add to dictionary: "+repr(p)
			self.d[p]=settingsItem(getSetting(self.socket,string.join(p,':')))
		#print "returning from m.index"
		return self.createIndex(row,column,self.d[p].path)
	def root(self):
		#print "called m.root"
		#return self.createIndex(0,0,self.d[('',)].path)
		return QtCore.QModelIndex()
	def flags(self,index):
		#print "called flags"
		if(not index.isValid()): return 0
		return QtCore.Qt.ItemIsSelectable|QtCore.Qt.ItemIsEnabled
	def headerData(self,section,orientation,role):
		#print "called headerData, section %d"%section
		if(role==QtCore.Qt.DisplayRole and orientation==QtCore.Qt.Horizontal):
		#	print "..validly, going to return"
			if(section==0):
				return QtCore.QVariant("Node")
			else:
				return QtCore.QVariant("Data")
		else: 
		#	print "...with invalid role/orientation"
			#if(role==QtCore.Qt.DisplayRole): print "role OK"
			return QtCore.QVariant()
		

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
	m=settingsTreeModel(s,nwindow)
	#mt=ModelTest(m,nwindow)
	ui.settingsTree.setModel(m)
	ui.settingsTree.setRootIndex(m.root())
	#for now, will just go through the entire settings tree
	#def recurse(i,n):
	#	j=settingsItem(getSetting(s,i.text(0)+':'+n))
	#	if(j.data):
	#		l=QtCore.QStringList()
	#		l.append(j.name)
	#		l.append(j.data)
	#		i.addChild(QtGui.QTreeWidgetItem(l))
	#	else:
	#		l=QtCore.QStringList()
	#		l.append(j.pathstr)
	#		l.append('')
	#		w=QtGui.QTreeWidgetItem(l)
	#		i.addChild(w)
	#		for c in j.children:
	#			recurse(w,c)
	#			
	#l=QtCore.QStringList()
	#l.append('')
	#l.append('')
	#i=QtGui.QTreeWidgetItem(l)
	#ui.settingsTree.addTopLevelItem(i)
	#recurse(i,'')
	
	nwindow.show()
	#print "doing fine"
uiold.setupUi(window)
QtCore.QObject.connect(window,QtCore.SIGNAL("accepted()"), connecttoserver)
window.show()
sys.exit(app.exec_())
