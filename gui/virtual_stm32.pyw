from serial import Serial
import tkinter as tk
from tkinter import ttk
import time

uart = Serial(port = 'COM4', baudrate = 9600, timeout = 0.2)  # open serial port

time.sleep(1)

avr_rData = [
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ##lcd 00:07
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ##lcd 07:15
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ##lcd 16:23
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ##lcd 24:31
    0,   0,   0,   0,                     ##led 32:05
    0,   0,   0,   0,                     ##swt 36:39
    0,   0,   0,   0,   0,   0,   0,   0, ##btn 40:47
	255
]

avr_tData = [
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ##lcd 00:07
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ##lcd 07:15
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ##lcd 16:23
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ##lcd 24:31
    0,   0,   0,   0,                     ##led 32:05
    0,   0,   0,   0,                     ##swt 36:39
    0,   0,   0,   0,   0,   0,   0,   0, ##btn 40:47
	255
]

def onClose():
  uart.close()
  print('exit')
  root.destroy()

def changeColor(frame, qty, offset, color1, color2):
  for i in range(qty):
    color = (color1 if (avr_rData[offset + i] == 0) else color2)
    b = frame.winfo_children()[i]
    if(len(b.winfo_children()) > 0):
      b = b.winfo_children()[0]
    else:
      b.config(activebackground=color)
    b.config(background=color)
  return

def updateUI():
  global buttonFrame, switchFrame, ledFrame, lcd
  changeColor(buttonFrame, 8, 40, "#F0F0F0", "#A3C3FF")
  changeColor(switchFrame, 4, 36, "#F0F0F0", "#A3C3FF")
  changeColor(ledFrame,    4, 32, "#A6A6A6", "#C3FF53")
  lcd.config(state='normal')
  lcd.delete('1.0', 'end')
  lcd.insert('insert', ''.join(map(chr,avr_rData[0:32])))
  lcd.config(state='disabled')
  return

def exchangeData(index, offset, mode):
  for i in range(len(avr_tData)):
    avr_tData[i] = avr_rData[i]
  if(mode == 'toggle'):
    avr_tData[offset + index] = 0 if avr_tData[offset + index] else 1
  if(mode == 'on'):
    avr_tData[offset + index] = 1
  if(mode == 'off'):
    avr_tData[offset + index] = 0
  msg = bytearray(avr_tData)
  uart.write(msg)

def onSwitch(index):
  exchangeData(index, 36, 'toggle')

def onButtonPress(_, index):
  exchangeData(index, 40, 'on')

def onButtonRelease(_, index):
  exchangeData(index, 40, 'off')

##############################################
##############################################
### ROOT

root = tk.Tk()
root.title('Virtual Input')
root.geometry('300x350')
root.resizable(False, False)
root.protocol("WM_DELETE_WINDOW", onClose)
root.config(bg="#EAEAEA")

##############################################
##############################################
### LCD

lcdFrame = tk.Frame(root)
lcdFrame.pack(side='top', expand=False, fill='both')

lcd = tk.Text(
  lcdFrame,
  height = 2,
  width = 16,
  exportselection=0,
  font=("Consolas", 25),
  relief='solid'
)
lcd.config(state='disabled')
lcd.pack( pady = 10, padx = 10)


##############################################
##############################################
### LED

ledFrame = tk.Frame(root)
ledFrame.pack(side='top', expand = 1, pady = 0, padx = 0)
ledTexts = ['L1', 'L2', 'L3', 'L4']
for i in range(len(ledTexts)):
  f = tk.Frame(ledFrame)
  l = tk.Label( f,
    text = f'       {ledTexts[i]}       ',
    relief='flat',
    justify='center',
    bg="#A6A6A6",
    width=7,
  )
  l.pack(side='top', expand = 1, pady = 0, padx = 0, fill='both')
  f.grid(row=((i//4)%4), column=(i%4), pady = 4, padx = 9)

##############################################
##############################################
### BUTTON

buttonFrame = tk.Frame(root)
buttonFrame.pack(side='top', expand = 1, pady = 0, padx = 0)
buttonTexts = ['B1', 'B2', 'B3', 'B4', 'B5', 'B6', 'B7', 'B8']
for i in range(len(buttonTexts)):
  b = tk.Button(buttonFrame, text = f'\n{buttonTexts[i]}\n', width=7, background="#F0F0F0", activebackground="#F0F0F0")
  b.grid(row=((i//4)%4), column=(i%4), pady = 10 if i < 4 else 4, padx = 7)
  b.bind("<ButtonPress>", (lambda e, u = i: onButtonPress(e, u)))
  b.bind("<ButtonRelease>", (lambda e, u = i: onButtonRelease(e, u)))


##############################################
##############################################
### SWITCH

switchFrame = tk.Frame(root)
switchFrame.pack(side='top', expand = 1, pady = 0, padx = 0)
switchTexts = ['S1', 'S2', 'S3', 'S4']
for i in range(len(switchTexts)):
  s = tk.Button(switchFrame, text = f'\n{switchTexts[i]}\n', width=7, background="#F0F0F0", activebackground="#F0F0F0")
  s.config(command=(lambda u = i: onSwitch(u)))
  s.grid(row=((i//4)%4), column=(i%4), pady = 10, padx = 7)


##############################################
##############################################
### START

align = False

def appLoop():
  global align
  bytesToRead = uart.in_waiting

  if(align == False):
    if(bytesToRead >= 1):
      rcv = uart.read()
      if(rcv == b'\xff'):
        align = True
  else:
    if(bytesToRead > len(avr_rData)):
      rcv = uart.read(len(avr_rData))
      rcv = list(rcv)
      if(rcv[len(rcv)-1] != 255):
        align = False
      else:
        for i in range(len(avr_rData)):
          avr_rData[i] = rcv[i]
        # print(bytearray(avr_rData))
        updateUI()
  root.after(1, appLoop)

appLoop()
root.mainloop()

