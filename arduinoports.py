import serial
import serial.tools.list_ports
import time
import pyautogui as pya
import numpy as np


def dataSending(r,g,b):
    lista = [0xff,4,r,g,b,numbLed]
    arduino.write(lista)
    print(arduino.readline().decode('utf-8'))

def dataSending2(mode,pattern):
    lista = [0xff,2,mode,pattern]
    arduino.write(lista)
    print(arduino.readline().decode('utf-8'))
    
   
print("laczenie...")
#time.sleep(2)
arduino = serial.Serial(port='COM3', baudrate=9600, timeout=.1)
print("arduino gotowe")
#numbLed = int(input("Podaj ilosc ledow na pasku: "))
numbLed = 70
tryb = int(input("Wybierz tryb: "))


   
if (tryb == 0): #off
    print("off")
    dataSending(0,0,0)

    

if (tryb == 1): #kolor pixela na ktorym jest kursor
    print("Kolor z kursora")
    while True:
        x, y = pya.position()
        r,g,b = pya.pixel(x, y)
        dataSending(r,g,b)
        

   
if (tryb == 2): #srednia z ekranu
    print("Srednia z ekranu")
    while True:
        avg = np.average(np.average(pya.screenshot(), axis=0), axis=0)
        r,g,b = int(avg[0]), int(avg[1]), int(avg[2])
        dataSending(r,g,b)
   
    

if (tryb == 3): #jeden kolor
    print("Statyczny kolor")
    r = int(input("Podaj czerwony: "))
    g = int(input("Podaj zielony: "))
    b = int(input("Podaj niebieski: "))
    dataSending(r,g,b)

if (tryb == 4): #patterny
    print("Patterny")
    #mode = int(input("Podaj mode: "))
    pattern = int(input("Podaj pattern: "))

    #dataSending2(mode,pattern)   
    dataSending2(4,pattern)  
        


