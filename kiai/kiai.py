#!/usr/bin/env python
import sys,string,re
from sakusen import *
from sakusencomms import *
from connectDialog import Ui_connectDialog
from settingsDialog import Ui_settingsDialog
from PyQt4 import QtGui,QtCore#,Qt
from modeltest import ModelTest
import sip
Socket_socketsInit()
timeout=timeval(5,0)
#@tCore.pyqtSignature("join(QString)")
class settingsItem:
	def __init__(self,d):
		#d.thisown=False #?
		self.pathstr=d.getSetting()
		self.path=tuple(self.pathstr.split(':'))
		self.value=d.getValue()
		self.name=self.path[-1]
		#if(self.name): self.parent=string.join(self.path[:-2],':')
		if(re.search("\d+ items",self.value)):
			self.children=self.value.split("\n")[1:]
			self.data=""
		else:
			self.children=[]
			self.data=self.value
		#self.d=d
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
	d.thisown=False #this is definitely correct - m now owns d
	s.send(m)
	buf=uint8(BUFFER_LEN)
	while(True):
		l=s.receiveTimeout(buf,BUFFER_LEN,timeout)
		i=IArchive(buf,l)
		#buf.thisown=False #is this right?
		r=Message(i)
		t=r.getType()
		if(t==messageType_notifySetting):
			#r.thisown=False #we are going to use the data from r, so we don't want it to be GCed
			d=r.getNotifySettingData()
			#d.thisown=False #d will be handled when r goes, I think
			return d
		else:
			unexpectedMessage(r)

class settingsTreeModel(QtCore.QAbstractItemModel):
	"""A Model class for viewing the Sakusen settings tree using the Qt Model/View paradigm. Stores the settings internally as a dictionary d whose keys are tuples ('a','b','c') for 'a:b:c' and values are corresponding settingsItems.
	
	These tuples are passed around as QModelIndex.internalIds. But python will garbage-collect them. Current hack: use d[tuple].path rather than tuple - that way the pointer should stay valid as long as the model exists"""
	#current method: l is a list, pointers are indexes into l; l stores objects for the 0th column and strings for the 1st column
	def __init__(self,socket,parent=None):
		QtCore.QAbstractItemModel.__init__(self,parent) #I am a tard
		#print "called m.__init__"
		self.socket=socket
		self.d={('',):settingsItem(getSetting(socket,':'))}
		self.l=[('',),(('',),'')]
		#print "did m.__init__"
	def data(self,index,role):
		#print "called m.data"
		if((not index.isValid()) or role!=QtCore.Qt.DisplayRole): return QtCore.QVariant()
		#print "about to return some data from "+repr(index.internalId())+" column "+repr(index.column())
		if(index.column()==0):
		#	print "going to return name:"+self.d[index.internalId()].name
			return QtCore.QVariant(self.d[self.l[index.internalId()]].name)
		elif(index.column()==1):
			#print "going to return data from:"+repr(self.l[index.internalId()])
			return QtCore.QVariant(self.l[index.internalId()][1])
		else: return QtCore.QVariant()
	def columnCount(self,parent):
		#if(not parent.isValid()): #print "called m.columnCount on invalid index"
		#	return 1
		#else: print "called m.columnCount on node "+repr(parent.internalId())
		#if(parent.isValid()):
		#	if(parent.column()>0): return 1 #finally right?
		return 2# really
	def rowCount(self,parent):
		#if(not parent.isValid()): print "called m.rowCount on invalid index"
		#else: print "called m.rowCount on node "+repr(parent.internalId())
		if(not parent.isValid()): return 1
		if(parent.column()>0): return 0
		l=len(self.d[self.l[parent.internalId()]].children)
		#print "about to return from rowCount"
		return l
	def parent(self,index):
		#print "called m.parent"
		if((not index.isValid()) or index.internalId()==self.l.index(('',)) or index.internalId()==self.l.index((('',),''))): return QtCore.QModelIndex()
		#print "trying to find parent for "+repr(self.l[index.internalId()])
		#print "searching "+repr(self.d[tuple(index.internalId()[:-1])].children)+" for entry"
		#print "returning from m.parent"
		if(index.column()==0): p=self.l[index.internalId()]
		else: p=self.l[index.internalId()][0]
		r=self.createIndex(self.d[p[:-1]].children.index(p[-1]),0,self.l.index(tuple(p[:-1])))
		#print "about to return "+repr(r)+", assertion will be "+repr(r.model()==self)
		return r
	def index(self,row,column,parent):
		#print "called m.index"
		if(not self.hasIndex(row,column,parent)):
		#	print "some craziness ocurring"
			return QtCore.QModelIndex()
		#print "craziness didn't happen"
		#print "did that segfault?"
		if(not parent.isValid()):
			#arrgh arrgh surely this cannot be the right way to do this
			if(row==0 and 0==column):#1 or 2 columns? Arrgh
				#print "about to create an index with "+self.l.index(('',)
				r=self.createIndex(row,column,self.l.index(('',)))
				#print "about to return "+repr(r)+", assertion will be "+repr(r.model()==self)
				return r
			elif(row==0 and column==1):
				return self.createIndex(row,column,self.l.index((('',),'')))
			else: return self.root()
		#print "halfway through m.index, internal pointer is "+parent.internalId()
		#if(parent.column()>0): return QtCore.QModelIndex()
		p=tuple(self.d[self.l[parent.internalId()]].path+(self.d[self.l[parent.internalId()]].children[row],))
		if(not p in self.d):
		#	print "about to add to dictionary: "+repr(p)
			self.d[p]=settingsItem(getSetting(self.socket,string.join(p,':')))
			self.l.append(p)
		#	print "added to dictionary"
		#	print "list is now "+repr(self.l)
		#print "returning from m.index"
		
		if(column>0):
		#	print "arrgh arrgh arrgh"
			d=self.d[p].data
			self.l.append((p,d))
			#print "added a non-zero column, list is now "+repr(self.l)
			return self.createIndex(row,column,self.l.index((p,d)))
		#	return QtCore.QModelIndex()
		r=self.createIndex(row,column,self.l.index(p))
		#print "about to return "+repr(r)+", assertion will be "+repr(r.model()==self)
		return r
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
	mt=ModelTest(m,nwindow) #should only do this when debugging
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
