import ast
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
from osc4py3.as_allthreads import *
from osc4py3 import oscbuildparse
from osc4py3 import oscmethod as osm
import subprocess
from PIL import Image, ImageTk
import cv2
import urllib3
import numpy as np
from math import sqrt
import random
import os

# variable definitions
pTab = [None, None]
pSB = [[None,None],[None,None]]

nbrOfZones = 2
zTab = [None, None, None, None, None]
zSB = [None, None, None, None, None]

theCWP = os.getcwd()



class OSC_CS():
    ''' OSC_SC class create OSC message server and client according to the information in osc_ini_vX.XX.txt file;
    sends and receives OSC messages '''

    def __init__(self):
        self.csList = []
        self.oscMessageList = []
        self.f = open("includes/config_.txt", "r")
        for line in self.f:
            if (line.strip()[0] == '@'):
                self.text_configInfo = line.strip()[1:]
                continue
            if (line.strip()[0] == '#'):
                continue
            if (line.strip() == 'CLIENT_SERVER_LIST'):
                process = 'CLIENT_SERVER_LIST'
                continue
            elif (line.strip() == 'OSC_MESSAGES_LIST'):
                process = 'OSC_MESSAGES_LIST'
                continue
            if (process == 'CLIENT_SERVER_LIST'):
                self.csList.append(ast.literal_eval(line))
            elif (process == 'OSC_MESSAGES_LIST'):
                self.oscMessageList.append(ast.literal_eval(line))
        self.f.close()

        osc_startup()

        for i in self.csList:
            if i['type'] == 'client':
                osc_udp_client(i['ip'], i['port'], i['name'])
            elif i['type'] == 'server':
                try:
                    osc_udp_server(i['ip'], i['port'], i['name'])
                    print(i['ip'], i['port'], i['name'])
                except:
                    messagebox.showinfo("Error",
                                        "please set the SERVER IPs in config file to this machine's IP address or to 127.0.0.1 and try again")
                    exit()

        # OSC receive here
        osc_method("/*", self.msghandler,
                   argscheme=osm.OSCARG_ADDRESS + osm.OSCARG_SRCIDENT + osm.OSCARG_READERNAME + osm.OSCARG_TYPETAGS + osm.OSCARG_DATAUNPACK)

    def friend(self, window):
        self.wself = window

    def run_osc(self):
        osc_process()

        # START HERE
        # 1 COMMUNICATE WITH TM
        self.app = theCWP + '/MEs/TMsimulator.maxpat'
        subprocess.call('open -a' + self.app, shell=True)

    def msghandler(self, address, sender, receiver, typetags, *args):
        # print("received:", address, sender, receiver, typetags, [i for i in args])
        typetags = typetags.replace(",", "")
        '''
        mval = []
        for i in range(len(typetags)):
            if typetags[i] == "s":
                mval.append(str(args[i]))
            elif typetags[i] == "i":
                mval.append(int(args[i]))
            elif typetags[i] == "f":
                mval.append(float(args[i]))
            elif typetags[i] == "b":
                mval.append(bytes(args[i]))
        '''
        # show message
        self.wself.msgMonitor_update(sender, receiver, address, typetags, args)

        # PROCESS INCOMING MESSAGES HERE:
        # - - - - - - - - - - - - - - - - - - - - - - - - - -
        # messages from MEs
        if (address == '/set/ME/loaded'):
            self.send_osc('CMtoME', '/set/ME/initialize', None, [])
        elif (address == '/set/ME/ready'):
            self.wself.MEready()
        elif (address == '/set/player/1/soundbank/1/list' or address == '/set/player/1/soundbank/2/list' or address == '/set/player/2/soundbank/1/list' or address == '/set/player/2/soundbank/2/list'):
            patt = address.split('/')
            id = int(patt[3])-1
            nbr = int(patt[5])-1
            pSB[id][nbr] = args
            if pTab[id] != None:
                pTab[id].loadSoundbank(nbr,args)
        elif (address == '/set/zone/1/soundbank/1/list' or address == '/set/zone/2/soundbank/1/list' or address == '/set/zone/3/soundbank/1/list' or address == '/set/zone/4/soundbank/1/list'  or address == '/set/zone/5/soundbank/1/list'):
            patt = address.split('/')
            id = int(patt[3])-1
            nbr = int(patt[5])-1
            zSB[id] = args
            if zTab[id] != None:
                zTab[id].loadSoundbank(nbr,args)
        elif (address == '/set/xxx'):
            pass
        elif (address == '/set/xxx'):
            pass
        # - - - - - - - - - - - - - - - - - - - - - - - - - -
        # messages from TM
        if (address == '/set/TM/loaded'):
            self.send_osc('CMtoTM', '/set/TM/initialize', None, [])
            #self.wself.TMloaded()
        elif (address == '/set/TM/ready'):
            self.wself.TMready()

    def send_osc(self, dest, pat, tag, val):
        msg = oscbuildparse.OSCMessage(pat, tag, val)
        osc_send(msg, dest)
        self.wself.msgMonitor_update('CM', dest, pat, tag, val)


# - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - -

class Window(tk.Tk):
    """ parent object is the GUI actually"""

    def __init__(self, comm, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)

        self.comm = comm
        self.title("MC Control Module ")
        self.geometry("825x715")
        self.resizable(0, 0)

        # create initial GUI elements
        self.containerFrm = tk.Frame(self)
        self.containerFrm.grid(row=0, column=0, sticky=tk.NSEW, padx=5, pady=5)

        logoFrm = tk.Frame(self.containerFrm, width=200, height=300, bg='red')
        logoFrm.grid(row=0, column=0, sticky=tk.NW)
        logoFrm.grid_propagate(0)
        img = tk.PhotoImage(file='includes/logo1.gif')
        logo = tk.Label(logoFrm, image=img)
        logo.image = img
        logo.grid(row=0, column=0, sticky=tk.NW)

        trafficFrm = tk.Frame(self.containerFrm, width=800, bg='grey', height=50)
        trafficFrm.grid(row=3, column=0, columnspan=4, sticky=tk.EW)
        self.msgMonitor = tk.Listbox(trafficFrm, width=100, height=3)
        self.msgMonitor.grid(row=0, column=0, columnspan=4, sticky=tk.EW)

    def TMloaded(self):
        pass

    def TMready(self):
        self.loadGUIelements_p1()
        # load ME selector
        self.loadSelector()
        # start video
        DrwVid(self.canvas, self)


    def loadGUIelements_p1(self):
        # create video frame
        self.videoFrmWidth = 600
        self.videoFrmHeight = 350
        self.videoFrm = tk.Frame(self.containerFrm, width=self.videoFrmWidth, bg='grey', height=self.videoFrmHeight)
        self.videoFrm.grid(row=0, column=1, rowspan=2, columnspan=3, sticky=tk.NW)
        # create canvas to draw video (and later shapes)
        self.canvas = tk.Canvas(self.videoFrm, width=self.videoFrmWidth, height=self.videoFrmHeight)
        self.canvas.grid()

        self.selectorFrm = tk.LabelFrame(self.containerFrm, text='Select Musical Environment', width=200, height=50)
        self.selectorFrm.grid(row=1, column=0, sticky=tk.NW)
        self.selectorFrm.grid_propagate(0)
        self.selectorFrm.grid_columnconfigure(0, weight=1)

    def loadSelector(self):
        self.me = tk.StringVar()
        meOptions = ('Tonality', 'Techno', 'Fields', 'Drums', 'Particles')
        selectorMenu = tk.OptionMenu(self.selectorFrm, self.me, *meOptions, command=self.loadME)
        selectorMenu.grid(row=1, column=0, padx=5, sticky=tk.EW)

    def loadGUIelements_p2(self):
        self.canvas.bind("<ButtonPress-1>", self.on_B1_press)
        self.canvas.bind("<B1-Motion>", self.on_B1_move)
        self.canvas.bind("<ButtonRelease-1>", self.on_B1_release)

        self.meFrm = tk.Frame(self.containerFrm, width=400, height=250, bg='green')
        self.meFrm.grid(row=2, column=0, columnspan=2, sticky=tk.NW)

        self.idBox = []
        for i in range(nbrOfZones):
            self.idBox.append(i)
        self.idBox.sort(reverse=True)
        self.zoneDrawingEnabled = True

    def loadTrackingControls(self):

        self.trackingFrm = tk.LabelFrame(self.containerFrm, text='Tracking Controls', width=400, height=250)
        self.trackingFrm.grid(row=2, column=2, sticky=tk.EW, padx=(5, 0), pady=5)
        self.trackingFrm.grid_propagate(0)
        self.trackingFrm.grid_columnconfigure(0, weight=1)

        self.trackingModeVar = tk.StringVar()
        oneplayer = tk.Radiobutton(self.trackingFrm, text='one player', variable=self.trackingModeVar, value="1p",
                                       command=self.setTrackingMode)
        oneplayer.grid(row=0, column=0, sticky=tk.W, padx=5)
        twoplayers = tk.Radiobutton(self.trackingFrm, text='two players', variable=self.trackingModeVar, value="2p",
                                      command=self.setTrackingMode)
        twoplayers.grid(row=0, column=1, sticky=tk.W, padx=5)
        zoneOnlyBox = tk.Radiobutton(self.trackingFrm, text='zones only', variable=self.trackingModeVar, value="zo",
                                     command=self.setTrackingMode)
        zoneOnlyBox.grid(row=0, column=2, sticky=tk.W, padx=5)
        self.trackingModeVar.set('1p')  # initialize

        self.colTABFrm = ttk.Notebook(self.trackingFrm, padding=0)
        self.colTABFrm.grid(row=1, columnspan=3, sticky=tk.NSEW)

        self.setTrackingMode()

    def setTrackingMode(self):
        val = self.trackingModeVar.get()
        if val == '1p':
            if pTab[0] == None:
                pTab[0] = TrackingTabs(self.colTABFrm, self, 'player', 0, None)
            if pTab[1] != None:
                pTab[1].forget()
                pTab[1] = None
            for i in range(len(zTab)):
                if zTab[i] != None:
                    zTab[i].drwObj.on_bt_close()
        elif val == '2p':
            if pTab[0] == None:
                pTab[0] = TrackingTabs(self.colTABFrm, self, 'player', 0, None)
            pTab[1] = TrackingTabs(self.colTABFrm, self, 'player', 1, None)
            for i in range(len(zTab)):
                if zTab[i] != None:
                    zTab[i].drwObj.on_bt_close()
        else: # 'zo'
            for i in range(len(pTab)):
                if pTab[i] != None:
                    pTab[i].forget()
                    pTab[i] = None
            if all(v is None for v in zTab):
                self.idBox.sort(reverse=True)
                zid = self.idBox.pop()
                self.drwDefaultZones(zid, 5, 5, 300, 350)
                zid = self.idBox.pop()
                self.drwDefaultZones(zid, 300, 5, 600, 350)





    def drwDefaultZones(self, zid, x1,y1,x2,y2):

        self.z = DrwZone(self.canvas, self, zid, x1, y1)
        self.z.move(x2, y2)
        self.z.release(x2, y2)
        self.z = None

    # handle loading and activating selected ME ----------------

    def loadME(self, *args):
        # loads the selected ME application

        # close the previous application and its GUI
        self.killME()

        # LOAD selected ME here
        self.ME = eval(self.me.get())
        self.MEname = self.me.get()

        # all four below are working:
        # subprocess.call('open -a /Users/ekmelertan/PycharmProjects/CM-ControlModule//MEs/me.app',shell=True)
        # subprocess.call('bash /Users/ekmelertan/PycharmProjects/CM-ControlModule//MEs/runMe.sh',shell=True)
        # subprocess.call('open -a /Applications/Max5/maxMSP.app /Users/ekmelertan/PycharmProjects/CM-ControlModule//MEs/me.maxpat',shell=True)
        #
        # patch = r'/Users/ekmelertan/PycharmProjects/CM-ControlModule/MEs/me.maxpat'
        # appPath = r'/Applications/Max5/maxMSP.app'
        # subprocess.call('%s  %s %s' % ('open -a', appPath, patch))

        #self.app = r'/Users/ekmelertan/PycharmProjects/CM-ControlModule/MEs/' + self.MEname + '.app'
        self.app = theCWP + '/MEs/MEsimulator.maxpat'
        subprocess.call('open -a' + self.app, shell=True)

    def MEloaded(self):
        self.loadGUIelements_p1()
        # load tracking controls
        self.loadTrackingControls()

    def MEready(self):
        self.loadGUIelements_p2()
        self.loadTrackingControls()
        # starts ME Classes i.e. 'Tonality', 'Techno', 'Fields', 'Drums', 'Particles'
        # activated by /set/ready message from loaded ME
        self.meFrmOptions = self.ME(self.meFrm, self)
        self.meFrmOptions.grid(row=0, column=0, sticky=tk.NSEW)
        self.meFrmOptions.tkraise()

    def killME(self):
        try:
            subprocess.call('pkill ' + self.MEname, shell=True)
            self.meFrmOptions.grid_remove()
        except:
            pass

    # handle message monitor ----------------------------------

    def msgMonitor_update(self, sender, receiver, address, typetags, mval):
        # called from OSC class to update message monitor window.
        self.msgMonitor.insert(tk.END, ("from:", sender, "to:", receiver, "msg:", address, typetags, mval))
        self.msgMonitor.yview(tk.END)

    # handle canvas interaction -------------------------------------------------

    def on_B1_press(self, event):
        if self.idBox:
            self.idBox.sort(reverse=True)
            self.zid = self.idBox.pop()
            self.z = DrwZone(self.canvas, self, self.zid, event.x, event.y)


    def on_B1_move(self, event):
        try:
            self.z.move(event.x, event.y)
        except:
            pass

    def on_B1_release(self, event):
        try:
            self.z.release(event.x, event.y)
            self.z = None
        except:
            pass


# CLASS TrackingTabs -------------------------------------------------

class TrackingTabs():
    def __init__(self, parent, master, mode, id, drwObj):
        self.parent = parent
        self.master = master
        self.mode = str(mode)
        self.id = id
        self.drwObj = drwObj

        self.optFrm = tk.Frame(self.parent, width=200, height=200)
        self.optFrm.grid(row=0, column=0, sticky=tk.EW, )
        self.optFrm.grid_propagate(0)
        self.optFrm.grid_columnconfigure(0, weight=1)

        self.parent.add(self.optFrm, text=str(self.mode[0]) + str(self.id))

        self.optVar = tk.IntVar()

        self.optVar.set(1)
        optBox = tk.Checkbutton(self.optFrm, text='Activate/Deactivate ' + str(self.mode) + ' ' + str(self.id),
                                variable=self.optVar, command=self.setOpt)
        optBox.grid(row=0, column=0, sticky=tk.W)
        self.setOpt()
        tk.ttk.Separator(self.optFrm).grid(row=1, columnspan=2, sticky=tk.EW, padx=5)

        if self.mode == 'zone':
            if not all(v is None for v in zSB):
                if zSB[self.id] != None:
                    self.loadSoundbank(0,zSB[self.id])
                else:
                    self.loadSoundbank(0,zSB[0])
        elif self.mode == 'player':
            if pSB[self.id][0] != None:
                self.loadSoundbank(0, pSB[self.id][0])
            if pSB[self.id][1] != None:
                self.loadSoundbank(1, pSB[self.id][1])

    def loadSoundbank(self, nbr, *args):
        self.soundbankNbr = nbr +1
        optSoundbankLabel = tk.Label(self.optFrm, text='Soundbank '+ str(self.soundbankNbr))
        optSoundbankLabel.grid(row=2+2*self.soundbankNbr, column=0, sticky=tk.W, padx=10)
        self.optSoundbankVar = tk.StringVar()
        if len(args) == None: args = ('not', 'initialized', 'yet')
        optSoundbankOptions = list(*args)
        self.optSoundbankVar.set(optSoundbankOptions[0])
        self.optSoundbank = tk.OptionMenu(self.optFrm, self.optSoundbankVar, *optSoundbankOptions,
                                            command=self.setoptSoundbank)
        self.optSoundbank.grid(row=3+2*self.soundbankNbr, column=0, sticky=tk.EW, padx=10)
        #self.optSoundbank.config(state="disabled")


    def forget(self):
        self.optVar.set(0)
        self.setOpt()
        self.parent.forget(self.optFrm)

    def hide(self):
        self.parent.tab(self.optFrm, state='disabled')
        # also send stop tracking message

    def show(self):
        self.parent.tab(self.optFrm, state='normal')
        if self.id == 1:
            self.parent.select(self.optFrm)
            # also send start tracking message

    def setOpt(self):
        val = self.optVar.get()
        self.master.comm.send_osc('CMtoTM', '/set/' + str(self.mode) + '/' + str(self.id) + '/tracking', ',i', val)

    def setoptSoundbank(self, *args):
        val = self.optSoundbankVar.get()
        self.master.comm.send_osc('CMtoME', '/set/' + str(self.mode) + '/' + str(self.id) + '/soundbank' + '/' + str(self.soundbankNbr), ',s', val)


# CLASS DrwZone -------------------------------------------------

class DrwZone():
    def __init__(self, parent, master, id, start_x, start_y):
        self.parent = parent
        self.master = master
        self.start_x = start_x
        self.start_y = start_y
        self.rect = None
        self.state = 1              # active
        self.outlineClr = 'yellow'
        self.id = id

        self.draw()

    def draw(self):
        self.rect = self.parent.create_rectangle(self.start_x, self.start_y, self.start_x + 5, self.start_y + 5,
                                                 outline=self.outlineClr)
        self.parent.tag_bind(self.rect, "<Enter>", self.on_Enter)
        self.parent.tag_bind(self.rect, "<Leave>", self.on_Leave)

    def move(self, curX, curY):
        if (curX < 5):
            curX = 5;
        elif (curX > self.master.videoFrmWidth):
            curX = self.master.videoFrmWidth
        if (curY < 5):
            curY = 5;
        elif (curY > self.master.videoFrmHeight):
            curY = self.master.videoFrmHeight

        self.parent.coords(self.rect, self.start_x, self.start_y, curX, curY)

    def release(self, curX, curY):
        if (sqrt((self.start_x - curX) ** 2 + (self.start_y - curY) ** 2) < 50):
            self.parent.delete(self.rect)
            self.master.idBox.append(self.id)
        else:
            self.parent.coords(self, self.start_x, self.start_y, curX, curY)
            # arange placement of text close button
            if (self.start_x < curX):
                topleft_x = self.start_x
            else:
                topleft_x = curX

            if (self.start_y < curY):
                topleft_y = self.start_y
            else:
                topleft_y = curY
            self.text = self.parent.create_text(topleft_x + 10, topleft_y + 10, font=("Purisa", 18), text=self.id)
            self.bt_close = self.parent.create_oval(topleft_x + 20, topleft_y + 10, topleft_x + 30, topleft_y + 20,
                                                    fill='red')
            self.parent.tag_bind(self.bt_close, "<1>", self.on_bt_close)

            # create TAB for zone
            zTab[self.id] = TrackingTabs(self.master.colTABFrm, self.master, 'zone', self.id, self)
            zTab[self.id].optVar.set(1)
            #send /set/zone/[id]/blob 1 x1 y1 x2 y2


    def on_Enter(self, event):
        self.parent.itemconfig(self.rect, width=5, outline=self.outlineClr)
        self.parent.tag_raise(self.rect)

    def on_Leave(self, event):
        self.parent.itemconfig(self.rect, width=1, outline=self.outlineClr)

    def on_bt_close(self, *event):
        self.parent.delete(self.rect)
        self.parent.delete(self.text)
        self.parent.delete(self.bt_close)
        self.master.idBox.append(self.id)
        zTab[self.id].forget()
        zTab[self.id] = None
        #send /set/zone/[id]/blob 0 message to TM to delete the blob


    def stateUpdate(self, val):
        self.state = val
        if self.state:
            self.outlineClr = 'yellow'
            #send start tracking message
        else:
            self.outlineClr = 'gray'
            #send stop tracking message
        self.parent.itemconfig(self.rect, width=1, outline=self.outlineClr)



# CLASS DrwVid -----------------------

class DrwVid():
    def __init__(self, parent, master):
        self.parent = parent
        self.master = master

        self.cnvsimg = self.parent.create_image(10, 10, anchor=tk.NW)

        # Capture ipCam
        # self.url = 'http://192.168.0.3:8080/shot.jpg'
        # self.http = urllib3.PoolManager()

        # Capture PC's video Cam
        self.cap = cv2.VideoCapture(0)

        self.video_loop()

    def video_loop(self):
        self.vid()
        self.master.after(10, self.video_loop)

    def vid(self):
        # Capture video frames
        retval, self.frame = self.cap.read()
        if (retval):
            self.img = cv2.flip(self.frame, 1)

            # Capture ipCam
            # r = self.http.request('GET', self.url)
            # self.img = cv2.imdecode(np.asarray(bytearray(r.data)), 1)


            self.cv2image = cv2.cvtColor(self.img, cv2.COLOR_BGR2RGBA)
            self.cv2image = cv2.resize(self.cv2image, (0, 0), fx=0.5, fy=0.5)
            self.img = Image.fromarray(self.cv2image)
            self.imgtk = ImageTk.PhotoImage(image=self.img)

            # Capture image in the same folder
            # img = cv2.imread('../includes/test.jpg', 1)
            # self.img = Image.open("../includes/test.jpg")
            # self.imgtk = ImageTk.PhotoImage(self.img)

            self.parent.itemconfigure(self.cnvsimg, image=self.imgtk)


# CLASS MEs --------------------------------------------------

class Tonality(tk.Frame):
    def __init__(self, parent, master):
        tk.Frame.__init__(self, parent)
        self.master = master

        col0Frm = tk.LabelFrame(self, text='Common Controls', width=200, height=250)
        col0Frm.grid(row=0, column=0, sticky=tk.EW, padx=0, pady=5)
        col0Frm.grid_propagate(0)
        col0Frm.grid_columnconfigure(0, weight=1)
        col1Frm = tk.LabelFrame(self, text='Musical Controls', width=200, height=250)
        col1Frm.grid(row=0, column=1, sticky=tk.EW, padx=(5, 0), pady=5)
        col1Frm.grid_propagate(0)
        col1Frm.grid_columnconfigure(0, weight=1)

        # COMMON CONTROLS
        self.setStateTo = tk.StringVar()
        self.setStateTo.set('START')
        commonPlay = tk.Button(col0Frm, textvariable=self.setStateTo, command=self.setState)
        commonPlay.grid(row=0, column=0, sticky=tk.EW, padx=10, pady=(10, 5))
        tk.ttk.Separator(col0Frm).grid(row=1, column=0, sticky=tk.EW, padx=5)
        self.commonVolume = tk.Scale(col0Frm, label='Set Volume', from_=0, to=1, orient=tk.HORIZONTAL, resolution=0.01,
                                     width=15, command=self.setVolume)
        self.commonVolume.set(0.5)
        self.commonVolume.grid(row=2, column=0, sticky=tk.EW, padx=10, pady=5)
        # tk.ttk.Separator(col0Frm).grid(row=3, column=2, sticky=tk.EW, pady=5)
        self.commonSensitivity = tk.Scale(col0Frm, label='Set Sensitivity', from_=0, to=1, orient=tk.HORIZONTAL,
                                          resolution=0.01, width=15, command=self.setSensitivity)
        self.commonSensitivity.set(0.5)
        self.commonSensitivity.grid(row=3, column=0, padx=10, pady=10, sticky=tk.EW)

        # MUSICAL CONTROLS
        scaleLabel = tk.Label(col1Frm, text='Choose Scale')
        scaleLabel.grid(row=0, column=0, sticky=tk.W, pady=(10, 0), padx=10)
        self.scaleVar = tk.StringVar()
        scaleList = ('a', 'b', 'c', 'd', 'e', 'f', 'g')
        self.scaleVar.set(scaleList[0])
        scaleOptions = tk.OptionMenu(col1Frm, self.scaleVar, *scaleList, command=self.setScale)
        scaleOptions.grid(row=1, column=0, sticky=tk.EW, padx=10)
        self.keyVar = tk.StringVar()
        keyList = ('a', 'b', 'c', 'd', 'e', 'f', 'g')
        self.keyVar.set(keyList[0])
        keyLabel = tk.Label(col1Frm, text='Choose Key')
        keyLabel.grid(row=2, column=0, sticky=tk.W, padx=10)
        keyOptions = tk.OptionMenu(col1Frm, self.keyVar, *keyList, command=self.setKey)
        keyOptions.grid(row=3, column=0, sticky=tk.EW, padx=10)
        self.heightVar = tk.IntVar()
        heighBox = tk.Checkbutton(col1Frm, text='Heights On/Off', variable=self.heightVar, command=self.setHeight)
        heighBox.grid(row=4, column=0, sticky=tk.W, padx=10)
        self.evolutionVar = tk.IntVar()
        evolutionBox = tk.Checkbutton(col1Frm, text='Evolution On/Off', variable=self.evolutionVar,
                                      command=self.setEvolution)
        evolutionBox.grid(row=5, column=0, sticky=tk.W, padx=10)
        self.directionReverseVar = tk.IntVar()
        directionReverseBox = tk.Checkbutton(col1Frm, text='Direction Reversed', variable=self.directionReverseVar,
                                             command=self.setDirectionReverse)
        directionReverseBox.grid(row=6, column=0, sticky=tk.W, padx=10)

    def setState(self):
        # send message
        # may be better to update state on confirmation...
        if (self.setStateTo.get() == 'START'):
            self.setStateTo.set('STOP')
            self.master.comm.send_osc('CMtoME', '/set/play/on', None, [])
        else:
            self.setStateTo.set('START')
            self.master.comm.send_osc('CMtoME', '/set/play/off', None, [])

    def setSensitivity(self, *args):
        val = [self.commonSensitivity.get()]
        self.master.comm.send_osc('CMtoME', '/set/sensitivity', None, val)

    def setVolume(self, *args):
        val = [self.commonVolume.get()]
        self.master.comm.send_osc('CMtoME', '/set/volume', ',f', val)

    def setKey(self, *args):
        val = [self.keyVar.get()]
        self.master.comm.send_osc('CMtoME', '/set/key', ',s', val)

    def setScale(self, *args):
        val = [self.scaleVar.get()]
        self.master.comm.send_osc('CMtoME', '/set/scale', None, val)

    def setHeight(self):
        val = self.heightVar.get()
        arg = ['off', 'on']
        self.master.comm.send_osc('CMtoTM', '/set/height', ',s', [arg[val]])

    def setEvolution(self):
        val = self.evolutionVar.get()
        arg = ['off', 'on']
        self.master.comm.send_osc('CMtoTM', '/set/evolution', ',s', [arg[val]])

    def setDirectionReverse(self):
        val = self.directionReverseVar.get()
        arg = ['off', 'on']
        self.master.comm.send_osc('CMtoTM', '/set/directionReverse', ',s', [arg[val]])


class Techno(tk.Frame):
    def __init__(self, parent, master):
        tk.Frame.__init__(self, parent)
        self.master = master

        col0Frm = tk.LabelFrame(self, text='Common Controls', width=200, height=250)
        col0Frm.grid(row=0, column=0, sticky=tk.EW, padx=0, pady=5)
        col0Frm.grid_propagate(0)
        col0Frm.grid_columnconfigure(0, weight=1)
        col1Frm = tk.LabelFrame(self, text='Musical Controls', width=200, height=250)
        col1Frm.grid(row=0, column=1, sticky=tk.EW, padx=(5, 0), pady=5)
        col1Frm.grid_propagate(0)
        col1Frm.grid_columnconfigure(0, weight=1)

        # COMMON CONTROLS
        self.setStateTo = tk.StringVar()
        self.setStateTo.set('START')
        commonPlay = tk.Button(col0Frm, textvariable=self.setStateTo, command=self.setState)
        commonPlay.grid(row=0, column=0, sticky=tk.EW, padx=10, pady=(10, 5))
        tk.ttk.Separator(col0Frm).grid(row=1, column=0, sticky=tk.EW, padx=5)
        self.commonVolume = tk.Scale(col0Frm, label='Set Volume', from_=0, to=1, orient=tk.HORIZONTAL, resolution=0.01,
                                     width=15, command=self.setVolume)
        self.commonVolume.set(0.5)
        self.commonVolume.grid(row=2, column=0, sticky=tk.EW, padx=10, pady=5)
        # tk.ttk.Separator(col0Frm).grid(row=3, column=2, sticky=tk.EW, pady=5)
        self.commonSensitivity = tk.Scale(col0Frm, label='Set Sensitivity', from_=0, to=1, orient=tk.HORIZONTAL,
                                          resolution=0.01, width=15, command=self.setSensitivity)
        self.commonSensitivity.set(0.5)
        self.commonSensitivity.grid(row=3, column=0, padx=10, pady=10, sticky=tk.EW)

        # MUSICAL CONTROLS
        self.evolutionVar = tk.IntVar()
        evolutionBox = tk.Checkbutton(col1Frm, text='Evolution On/Off', variable=self.evolutionVar,
                                      command=self.setEvolution)
        evolutionBox.grid(row=0, column=0, sticky=tk.W, padx=10)

        self.breakVar = tk.IntVar()
        breakBox = tk.Checkbutton(col1Frm, text='Break', variable=self.breakVar,
                                  command=self.setBreak)
        breakBox.grid(row=1, column=0, sticky=tk.W, padx=10)

    def setState(self):
        # send message
        # may be better to update state on confirmation...
        if (self.setStateTo.get() == 'START'):
            self.setStateTo.set('STOP')
            self.master.comm.send_osc('CMtoME', '/set/play/on', None, [])
        else:
            self.setStateTo.set('START')
            self.master.comm.send_osc('CMtoME', '/set/play/off', None, [])

    def setSensitivity(self, *args):
        val = [self.commonSensitivity.get()]
        self.master.comm.send_osc('CMtoME', '/set/sensitivity', None, val)

    def setVolume(self, *args):
        val = [self.commonVolume.get()]
        self.master.comm.send_osc('CMtoME', '/set/volume', ',f', val)

    def setEvolution(self):
        val = self.evolutionVar.get()
        arg = ['off', 'on']
        self.master.comm.send_osc('CMtoTM', '/set/evolution', ',s', [arg[val]])

    def setBreak(self):
        val = self.breakVar.get()
        arg = ['off', 'on']
        self.master.comm.send_osc('CMtoTM', '/set/break', ',s', [arg[val]])


class Fields(tk.Frame):
    def __init__(self, parent, master):
        tk.Frame.__init__(self, parent)

        self.master = master

        col0Frm = tk.LabelFrame(self, text='Common Controls', width=200, height=250)
        col0Frm.grid(row=0, column=0, sticky=tk.EW, padx=0, pady=5)
        col0Frm.grid_propagate(0)
        col0Frm.grid_columnconfigure(0, weight=1)
        col1Frm = tk.LabelFrame(self, text='Musical Controls', width=200, height=250)
        col1Frm.grid(row=0, column=1, sticky=tk.EW, padx=(5, 0), pady=5)
        col1Frm.grid_propagate(0)
        col1Frm.grid_columnconfigure(0, weight=1)

        # COMMON CONTROLS
        self.setStateTo = tk.StringVar()
        self.setStateTo.set('START')
        commonPlay = tk.Button(col0Frm, textvariable=self.setStateTo, command=self.setState)
        commonPlay.grid(row=0, column=0, sticky=tk.EW, padx=10, pady=(10, 5))
        tk.ttk.Separator(col0Frm).grid(row=1, column=0, sticky=tk.EW, padx=5)
        self.commonVolume = tk.Scale(col0Frm, label='Set Volume', from_=0, to=1, orient=tk.HORIZONTAL, resolution=0.01,
                                     width=15, command=self.setVolume)
        self.commonVolume.set(0.5)
        self.commonVolume.grid(row=2, column=0, sticky=tk.EW, padx=10, pady=5)
        # tk.ttk.Separator(col0Frm).grid(row=3, column=2, sticky=tk.EW, pady=5)
        self.commonSensitivity = tk.Scale(col0Frm, label='Set Sensitivity', from_=0, to=1, orient=tk.HORIZONTAL,
                                          resolution=0.01, width=15, command=self.setSensitivity)
        self.commonSensitivity.set(0.5)
        self.commonSensitivity.grid(row=3, column=0, padx=10, pady=10, sticky=tk.EW)

        # MUSICAL CONTROLS
        self.jumpVar = tk.IntVar()
        jumpBox = tk.Checkbutton(col1Frm, text='Jump On/Off', variable=self.jumpVar, command=self.setJump)
        jumpBox.grid(row=4, column=0, sticky=tk.W, padx=10)
        self.kickVar = tk.IntVar()
        kickBox = tk.Checkbutton(col1Frm, text='Kick On/Off', variable=self.kickVar, command=self.setKick)
        kickBox.grid(row=5, column=0, sticky=tk.W, padx=10)
        self.overheadVar = tk.IntVar()
        overheadBox = tk.Checkbutton(col1Frm, text='Overhead On/Off', variable=self.overheadVar,
                                     command=self.setOverhead)
        overheadBox.grid(row=6, column=0, sticky=tk.W, padx=10)
        self.meltdownVar = tk.IntVar()
        meltdownBox = tk.Checkbutton(col1Frm, text='Meltdown On/Off', variable=self.meltdownVar,
                                     command=self.setMeltdown)
        meltdownBox.grid(row=7, column=0, sticky=tk.W, padx=10)

    def setState(self):
        # send message
        # may be better to update state on confirmation...
        if (self.setStateTo.get() == 'START'):
            self.setStateTo.set('STOP')
            self.master.comm.send_osc('CMtoME', '/set/play/on', None, [])
        else:
            self.setStateTo.set('START')
            self.master.comm.send_osc('CMtoME', '/set/play/off', None, [])

    def setSensitivity(self, *args):
        val = [self.commonSensitivity.get()]
        self.master.comm.send_osc('CMtoME', '/set/sensitivity', None, val)

    def setVolume(self, *args):
        val = [self.commonVolume.get()]
        self.master.comm.send_osc('CMtoME', '/set/volume', ',f', val)

    def setJump(self):
        val = self.jumpVar.get()
        self.master.comm.send_osc('CMtoME', '/set/jump', ',i', [val])

    def setKick(self):
        val = self.kickVar.get()
        self.master.comm.send_osc('CMtoME', '/set/kick', ',i', [val])

    def setOverhead(self):
        val = self.overheadVar.get()
        self.master.comm.send_osc('CMtoME', '/set/overhead', ',i', [val])

    def setMeltdown(self):
        val = self.meltdownVar.get()
        self.master.comm.send_osc('CMtoME', '/set/meltdown', ',i', [val])


class Drums(tk.Frame):
    def __init__(self, parent, master):
        tk.Frame.__init__(self, parent)

        self.master = master

        col0Frm = tk.LabelFrame(self, text='Common Controls', width=200, height=250)
        col0Frm.grid(row=0, column=0, sticky=tk.EW, padx=0, pady=5)
        col0Frm.grid_propagate(0)
        col0Frm.grid_columnconfigure(0, weight=1)
        col1Frm = tk.LabelFrame(self, text='Musical Controls', width=200, height=250)
        col1Frm.grid(row=0, column=1, sticky=tk.EW, padx=(5, 0), pady=5)
        col1Frm.grid_propagate(0)
        col1Frm.grid_columnconfigure(0, weight=1)

        # COMMON CONTROLS
        self.setStateTo = tk.StringVar()
        self.setStateTo.set('START')
        commonPlay = tk.Button(col0Frm, textvariable=self.setStateTo, command=self.setState)
        commonPlay.grid(row=0, column=0, sticky=tk.EW, padx=10, pady=(10, 5))
        tk.ttk.Separator(col0Frm).grid(row=1, column=0, sticky=tk.EW, padx=5)
        self.commonVolume = tk.Scale(col0Frm, label='Set Volume', from_=0, to=1, orient=tk.HORIZONTAL, resolution=0.01,
                                     width=15, command=self.setVolume)
        self.commonVolume.set(0.5)
        self.commonVolume.grid(row=2, column=0, sticky=tk.EW, padx=10, pady=5)
        # tk.ttk.Separator(col0Frm).grid(row=3, column=2, sticky=tk.EW, pady=5)
        self.commonSensitivity = tk.Scale(col0Frm, label='Set Sensitivity', from_=0, to=1, orient=tk.HORIZONTAL,
                                          resolution=0.01, width=15, command=self.setSensitivity)
        self.commonSensitivity.set(0.5)
        self.commonSensitivity.grid(row=3, column=0, padx=10, pady=10, sticky=tk.EW)

        # MUSICAL CONTROLS
        self.evolutionVar = tk.IntVar()
        evolutionBox = tk.Checkbutton(col1Frm, text='Evolution On/Off', variable=self.evolutionVar,
                                      command=self.setEvolution)
        evolutionBox.grid(row=0, column=0, sticky=tk.W, padx=10)
        bgMusicLabel = tk.Label(col1Frm, text='Background Music')
        bgMusicLabel.grid(row=1, column=0, sticky=tk.W, padx=10)
        self.bgMusicVar = tk.StringVar()
        bgMusicAlways = tk.Radiobutton(col1Frm, text='Always', variable=self.bgMusicVar, value="Always",
                                       command=self.setBgMusic)
        bgMusicAlways.grid(row=2, column=0, sticky=tk.W, padx=10)
        bgMusicNever = tk.Radiobutton(col1Frm, text='Never', variable=self.bgMusicVar, value="Never",
                                      command=self.setBgMusic)
        bgMusicNever.grid(row=3, column=0, sticky=tk.W, padx=10)
        bgMusicAuto = tk.Radiobutton(col1Frm, text='Auto', variable=self.bgMusicVar, value="Auto",
                                     command=self.setBgMusic)
        bgMusicAuto.grid(row=4, column=0, sticky=tk.W, padx=10)
        self.bgMusicVar.set('Auto')  # initialize

    def setState(self):
        # send message
        # may be better to update state on confirmation...
        if (self.setStateTo.get() == 'START'):
            self.setStateTo.set('STOP')
            self.master.comm.send_osc('CMtoME', '/set/play', ',i', [1])
        else:
            self.setStateTo.set('START')
            self.master.comm.send_osc('CMtoME', '/set/play', ',i', [0])

    def setSensitivity(self, *args):
        val = [self.commonSensitivity.get()]
        self.master.comm.send_osc('CMtoME', '/set/sensitivity', None, val)

    def setVolume(self, *args):
        val = [self.commonVolume.get()]
        self.master.comm.send_osc('CMtoME', '/set/volume', ',f', val)

    def setEvolution(self):
        val = self.evolutionVar.get()
        self.master.comm.send_osc('CMtoME', '/set/evolution', None, [val])

    def setBgMusic(self):
        val = [self.bgMusicVar.get()]
        self.master.comm.send_osc('CMtoME', '/set/bgmusic', ',s', val)


class Particles(tk.Frame):
    def __init__(self, parent, master):
        tk.Frame.__init__(self, parent)

        self.master = master

        col0Frm = tk.LabelFrame(self, text='Common Controls', width=200, height=250)
        col0Frm.grid(row=0, column=0, sticky=tk.EW, padx=0, pady=5)
        col0Frm.grid_propagate(0)
        col0Frm.grid_columnconfigure(0, weight=1)
        col1Frm = tk.LabelFrame(self, text='Musical Controls', width=200, height=250)
        col1Frm.grid(row=0, column=1, sticky=tk.EW, padx=(5, 0), pady=5)
        col1Frm.grid_propagate(0)
        col1Frm.grid_columnconfigure(0, weight=1)

        # COMMON CONTROLS
        self.setStateTo = tk.StringVar()
        self.setStateTo.set('START')
        commonPlay = tk.Button(col0Frm, textvariable=self.setStateTo, command=self.setState)
        commonPlay.grid(row=0, column=0, sticky=tk.EW, padx=10, pady=(10, 5))
        tk.ttk.Separator(col0Frm).grid(row=1, column=0, sticky=tk.EW, padx=5)
        self.commonVolume = tk.Scale(col0Frm, label='Set Volume', from_=0, to=1, orient=tk.HORIZONTAL, resolution=0.01,
                                     width=15, command=self.setVolume)
        self.commonVolume.set(0.5)
        self.commonVolume.grid(row=2, column=0, sticky=tk.EW, padx=10, pady=5)
        # tk.ttk.Separator(col0Frm).grid(row=3, column=2, sticky=tk.EW, pady=5)
        self.commonSensitivity = tk.Scale(col0Frm, label='Set Sensitivity', from_=0, to=1, orient=tk.HORIZONTAL,
                                          resolution=0.01, width=15, command=self.setSensitivity)
        self.commonSensitivity.set(0.5)
        self.commonSensitivity.grid(row=3, column=0, padx=10, pady=10, sticky=tk.EW)

        # MUSICAL CONTROLS
        scaleLabel = tk.Label(col1Frm, text='Choose Scale')
        scaleLabel.grid(row=0, column=0, sticky=tk.W, pady=(10, 0), padx=10)
        self.scaleVar = tk.StringVar()
        scaleList = ('a', 'b', 'c', 'd', 'e', 'f', 'g')
        self.scaleVar.set(scaleList[0])
        scaleOptions = tk.OptionMenu(col1Frm, self.scaleVar, *scaleList, command=self.setScale)
        scaleOptions.grid(row=1, column=0, sticky=tk.EW, padx=10)
        self.keyVar = tk.StringVar()
        keyList = ('a', 'b', 'c', 'd', 'e', 'f', 'g')
        self.keyVar.set(keyList[0])
        keyLabel = tk.Label(col1Frm, text='Choose Key')
        keyLabel.grid(row=2, column=0, sticky=tk.W, padx=10)
        keyOptions = tk.OptionMenu(col1Frm, self.keyVar, *keyList, command=self.setKey)
        keyOptions.grid(row=3, column=0, sticky=tk.EW, padx=10)
        self.heightVar = tk.IntVar()
        heighBox = tk.Checkbutton(col1Frm, text='Heights On/Off', variable=self.heightVar, command=self.setHeight)
        heighBox.grid(row=4, column=0, sticky=tk.W, padx=10)
        self.evolutionVar = tk.IntVar()
        evolutionBox = tk.Checkbutton(col1Frm, text='Evolution On/Off', variable=self.evolutionVar,
                                      command=self.setEvolution)
        evolutionBox.grid(row=5, column=0, sticky=tk.W, padx=10)
        self.directionReverseVar = tk.IntVar()
        directionReverseBox = tk.Checkbutton(col1Frm, text='Direction Reversed', variable=self.directionReverseVar,
                                             command=self.setDirectionReverse)
        directionReverseBox.grid(row=6, column=0, sticky=tk.W, padx=10)

    def setState(self):
        # send message
        # may be better to update state on confirmation...
        if (self.setStateTo.get() == 'START'):
            self.setStateTo.set('STOP')
            self.master.comm.send_osc('CMtoME', '/set/play/on', None, [])
        else:
            self.setStateTo.set('START')
            self.master.comm.send_osc('CMtoME', '/set/play/off', None, [])

    def setSensitivity(self, *args):
        val = [self.commonSensitivity.get()]
        self.master.comm.send_osc('CMtoME', '/set/sensitivity', None, val)

    def setVolume(self, *args):
        val = [self.commonVolume.get()]
        comm.send_osc('CMtoME', '/set/volume', ',f', val)

    def setKey(self, *args):
        val = [self.keyVar.get()]
        self.master.comm.send_osc('CMtoME', '/set/key', ',s', val)

    def setScale(self, *args):
        val = [self.scaleVar.get()]
        self.master.comm.send_osc('CMtoME', '/set/scale', None, val)

    def setHeight(self):
        val = self.heightVar.get()
        if (val):
            self.master.comm.send_osc('CMtoME', '/set/heights/on', None, [])
        else:
            self.master.comm.send_osc('CMtoME', '/set/heights/off', None, [])

    def setEvolution(self):
        val = self.evolutionVar.get()
        if (val):
            self.master.comm.send_osc('CMtoME', '/set/evolution/on', None, [])
        else:
            self.master.comm.send_osc('CMtoME', '/set/evolution/off', None, [])

    def setDirectionReverse(self):
        val = self.directionReverseVar.get()
        if (val):
            self.master.comm.send_osc('CMtoME', '/set/direction/reverse/on', None, [])
        else:
            self.master.comm.send_osc('CMtoME', '/set/direction/reverse/off', None, [])


# <<< MEs --------------------------------------------------

def main():
    comm = OSC_CS()

    # create gui
    window = Window(comm)
    comm.friend(window)
    window.after(0, comm.run_osc)
    window.mainloop()


if __name__ == '__main__':
    main()
