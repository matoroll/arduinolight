from tkinter import *

win = Tk()
win.title("Ambient Light Control Panel")
win.geometry('400x500')




def zamknij(zdarzenie):
    win.quit()

    win.destroy()

def on_closing():
    print("Lorem")
    okno.destroy()


CenterText = Label(win, text="Set connection:", font=(30), height=4, foreground="yellow", background="black")
CenterText.pack(fill=BOTH)

PortText = Label(win, text="Port:", font=(30), foreground="yellow", background="black")
PortText.pack(fill=BOTH)

SpeedText = Label(win, text="Speed:", font=(30), foreground="yellow", background="black")
SpeedText.pack(fill=BOTH)


przycisk = Button(win, text = "Zamknij mnie !", font=("Courier",16,"bold"), background="red")


przycisk.bind("<Button-1>", zamknij)
przycisk.pack(fill=X)


#okno.protocol("WM_DELETE_WINDOW")
win.mainloop()
