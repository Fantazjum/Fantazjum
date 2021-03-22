#Made with collaboration with Paweł Pastusiak and Łukasz Piekarek
import math
import random
import matplotlib.pyplot as plt
import matplotlib.patches as ptch


class ColorPoint:  # ta klasa przechowuje wartosc i oryginalny kolor dla danej reprezentacji bitowej
    sygnalSin = 0
    sygnalCos = 0
    kolor = ""

    def __init__(self, sin, cos, kolor):
        self.sygnalSin = sin
        self.sygnalCos = cos
        self.kolor = kolor


def catchFile(name):  # tu funkcja otwarcia i przerobienie stringa tak, by mieć rozszerzenie pliku
    file = open(name, "br")
    temp = name.split(".")
    extension = temp[-1]
    return extension, file


def encodeBPSK(bit):  # tu zawieramy przesyłany bit w sygnał
    bit = int(bit)
    if bit:
        signalC = math.cos(0)
        signalS = math.sin(0)
        col = 'ko'
    else:
        signalC = math.cos(math.pi)
        signalS = math.sin(0)
        col = 'yo'
    signal = ColorPoint(signalS, signalC, col)
    return signal


def decodeBPSK(signal):  # transformacja sygnału w bit
    bitLine = math.acos(signal.sygnalCos)
    if bitLine < math.pi * 1 / 2 or bitLine >= math.pi * 3 / 2:
        bit = 1
    else:
        bit = 0
    return bit


def encodeQPSK(bit, bit1):  # tu zawieramy przesyłany bit w sygnał
    bit = int(bit)
    bit1 = int(bit1)
    bit = bit * 2 + bit1
    signalC = math.cos(math.pi * bit / 2 + math.pi / 4)
    signalS = math.sin(math.pi * bit / 2 + math.pi / 4)
    if bit == 0:
        col = 'bo'
    elif bit == 1:
        col = 'ro'
    elif bit == 2:
        col = 'co'
    else:
        col = 'mo'
    signal = ColorPoint(signalS, signalC, col)
    return signal


def decodeQPSK(signal):  # transformacja sygnału w bit
    bitLineC = math.acos(signal.sygnalCos)
    bitLineS = math.asin(signal.sygnalSin)
    if bitLineC < math.pi * 1 / 2 and 0 <= bitLineS < math.pi * 1 / 2:
        bit = 0
        bit1 = 0
    elif bitLineC >= math.pi * 1 / 2 and bitLineS >= 0:
        bit = 0
        bit1 = 1
    elif bitLineC >= math.pi * 1/2 and bitLineS < 0:
        bit = 1
        bit1 = 0
    else:
        bit = 1
        bit1 = 1
    return bit, bit1


def tluczek(byte):  # funkcja rozbijajaca w ciag bitow
    string = ""
    byte = int(byte.hex(), base=16)
    for x in range(8):
        y = byte % 2
        byte = byte // 2
        string = str(y) + string
    string = list(string)
    return string


def tluczekQ(byte):  # funkcja rozbijajaca w ciag bitow
    string = ""
    byte = int(byte.hex(), base=16)
    for x in range(8):
        y = byte % 2
        byte = byte // 2
        string = str(y) + string
    string = [string[i:i+2] for i in range(0, len(string), 2)]
    return string


def disrupt(signal, dist, sev):  # tu będą symulowane zakłócenia
    disrupted = signal
    if disrupted.sygnalCos==0:
        PC = 0
    else:
        PC = 2*math.log10(abs(disrupted.sygnalCos))
    if disrupted.sygnalSin==0:
        PS = 0
    else:
        PS = 2 * math.log10(abs(disrupted.sygnalSin))
    szumPC = math.pow(10, PC - sev/10)        # moc szumu obliczona ze stosunku sygnalu do szumu; moc fali h*f
    szumPS = math.pow(10, PS - sev/10)
    szumC = math.sqrt(szumPC)
    szumS = math.sqrt(szumPS)
    disrupted.sygnalCos += random.normalvariate(0, szumC)
    disrupted.sygnalSin += random.normalvariate(0, szumS)
    signum = 1
    if disrupted.sygnalCos==0:
        PC = 0
    elif disrupted.sygnalCos<0:
        PC = 2*math.log10(abs(disrupted.sygnalCos))
        signum = -1
    else:
        PC = 2*math.log10(disrupted.sygnalCos)
    tlumC = math.pow(10, PC-math.log10(32.44 + 20*math.log10(dist) + 20*math.log10(2000))/10) #tlumienie wolnej przestrzeni
    disrupted.sygnalCos = signum*math.sqrt(tlumC)
    signum = 1
    if disrupted.sygnalSin==0:
        PS = 0
        signum = 0
    elif disrupted.sygnalSin<0:
        PS = 2 * math.log10(abs(disrupted.sygnalSin))
        signum = -1
    else:
        PS = 2 * math.log10(disrupted.sygnalSin)
    tlumS = math.pow(10, PS-math.log10(32.44 + 20*math.log10(dist) + 20*math.log10(2000))/10) #dla uproszczenia przyjmujemy stala czestotliwosc: 2000 MHz
    disrupted.sygnalSin = signum*math.sqrt(tlumS)
    if disrupted.sygnalCos > 1:
        disrupted.sygnalCos = 1.0
    elif disrupted.sygnalCos < -1:
        disrupted.sygnalCos = -1.0
    if disrupted.sygnalSin>1:
        disrupted.sygnalSin=1.0
    elif disrupted.sygnalSin<-1:
        disrupted.sygnalSin=-1.0
    return disrupted


def printMap(patches):  # tu będzie wyświetlany wykres punktów
    plt.legend(handles=patches)
    plt.axhline(y=0, color='k')
    plt.axvline(x=0, color='k')
    plt.title("Rozlozenie bitow w transmisji")
    plt.axis([-1, 1, -1, 1])
    plt.show()
    return


def transferQPSK(inFile, outFile, distance, severity, imagine):  # tu przeprowadzamy modulacje QPSK
    plt.figure(1)
    byte = inFile.read(1)
    while byte:
        byte2 = 0
        bitString = tluczekQ(byte)
        for bit in bitString:
            bitf = bit[0]
            bitl = bit[1]
            signal = encodeQPSK(bitf, bitl)
            signal2 = disrupt(signal, distance, severity)
            bit, bit1 = decodeQPSK(signal2)
            byte2 = 2*byte2 + int(bit)
            byte2 = 2 * byte2 + int(bit1)
            if imagine:
                plt.plot(signal2.sygnalCos, signal2.sygnalSin, signal2.kolor)
        byte2 = byte2.to_bytes(1, 'big')
        outFile.write(byte2)
        byte = inFile.read(1)
    if imagine:
        legend = [ptch.Patch(color="b", label="00"), ptch.Patch(color="r", label="01"),
                  ptch.Patch(color="c", label="10"), ptch.Patch(color="m", label="11")]
        printMap(legend)
    return


def transferBPSK(inFile, outFile, distance, severity, imagine):  # tu przeprowadzamy modulacje BPSK
    plt.figure(1)
    byte = inFile.read(1)
    while byte:
        byte2 = 0
        bitString = tluczek(byte)
        for bit in bitString:
            signal = encodeBPSK(bit)
            signal2 = disrupt(signal, distance, severity)
            bit2 = decodeBPSK(signal2)
            byte2 = 2 * byte2 + int(bit2)
            if imagine:
                plt.plot(signal2.sygnalCos, signal2.sygnalSin, signal2.kolor)
        byte2 = byte2.to_bytes(1, 'big')
        outFile.write(byte2)
        byte = inFile.read(1)
    if imagine:
        legend = [ptch.Patch(color="k", label="1"), ptch.Patch(color="y", label="0")]
        printMap(legend)
    return


def pbmTransferQPSK(inFile, outFile, distance, severity, imagine):  # tu przeprowadzamy modulacje QPSK dla pbm
    plt.figure(1)
    line = inFile.readline()
    outFile.write(line)
    line = inFile.readline()
    while line[0] == '#' or line[0] == '\n':
        outFile.write(line)
        line = inFile.readline()
    outFile.write(line)
    line = inFile.readline()
    while line:
        if line[0] == '#':
            outFile.write(line)
            continue
        string = line.replace(" ", "")
        string = [string[i:i + 2] for i in range(0, len(string), 2)]
        for bit in string:
            bitf = bit[0]
            if bitf == "\n":
                continue
            if len(bit) == 2:
                if bit[1]!="\n":
                    bitl = bit[1]
                    skip = 1
                else:
                    bitl = 0
                    skip = 0
            else:
                skip = 0
                bitl = 0
            signal = encodeQPSK(bitf, bitl)
            signal2 = disrupt(signal, distance, severity)
            bit2, bit3 = decodeQPSK(signal2)
            outFile.write(str(bit2) + " ")
            if skip:                    #tutaj trzeba zalozyc ze odbiorca zna ilosc znakow na linie i nie przyjmuje nadmoariowych danych, tzn ze i stnieje umowa ze ostatnie 0 w ciogu oznacza \n
                outFile.write(str(bit3) + " ")
            if imagine:
                plt.plot(signal2.sygnalCos, signal2.sygnalSin, signal2.kolor)
        outFile.write("\n")
        line = inFile.readline()
    if imagine:
        legend = [ptch.Patch(color="b", label="00"), ptch.Patch(color="r", label="01"),
                  ptch.Patch(color="c", label="10"), ptch.Patch(color="m", label="11")]
        printMap(legend)
    return


def pbmTransferBPSK(inFile, outFile, distance, severity, imagine):  # tu przeprowadzamy modulacje BPSK
    plt.figure(1)
    line = inFile.readline()
    outFile.write(line)
    line = inFile.readline()
    while line[0] == '#' or line[0] == '\n':
        outFile.write(line)
        line = inFile.readline()
    outFile.write(line)
    line = inFile.readline()
    while line:
        if line[0] == '#':
            outFile.write(line)
            line = inFile.readline()
            continue
        bits = line.split()
        for bit in bits:
            signal = encodeBPSK(bit)
            signal2 = disrupt(signal, distance, severity)
            bit2 = decodeBPSK(signal2)
            outFile.write(str(bit2) + " ")
            if imagine:
                plt.plot(signal2.sygnalCos, signal2.sygnalSin, signal2.kolor)
        outFile.write("\n")
        line = inFile.readline()
    if imagine:
        legend = [ptch.Patch(color="k", label="1"), ptch.Patch(color="y", label="0")]
        printMap(legend)
    return


def switchTransfer(argument, inFile, outFile, arg1, arg2, info):
    switcher = {
        1: lambda: transferBPSK(inFile, outFile, arg1, arg2, info),
        2: lambda: transferQPSK(inFile, outFile, arg1, arg2, info)
    }
    return switcher.get(argument, lambda: "brak")()


def pbmSafeTransfer(argument, inFile, outFile, arg1, arg2, info):
    switcher = {
        1: lambda: pbmTransferBPSK(inFile, outFile, arg1, arg2, info),
        2: lambda: pbmTransferQPSK(inFile, outFile, arg1, arg2, info)
    }
    return switcher.get(argument, lambda: "brak")()


def passThrough(tryb):
    fileName = input("Podaj nazwe pliku: ")
    pass1 = float(input("Podaj w jakiej odleglosci od siebie sa nadajnik i odbiornik (w km): "))
    pass2 = float(input("Podaj stosunek sygnalu do szumu (w dB): "))
    quest = input("Czy chcesz wyswietlic graf pokazujacy rozlozenie bitow? (T)")
    if quest == "T" or quest == "t":
        wyswietl = 1
    else:
        wyswietl = 0
    extension, file = catchFile(fileName)
    endFileName = "exit."
    endFileName += extension
    if extension == "pbm":
        print("Wejscie w tryb przekazania formatu pliku pbm...")
        file.close()
        file = open(fileName, "r")
        endFile = open(endFileName, "w")
        pbmSafeTransfer(tryb, file, endFile, pass1, pass2, wyswietl)
    else:
        endFile = open(endFileName, "bw")
        switchTransfer(tryb, file, endFile, pass1, pass2, wyswietl)
    file.close()
    endFile.close()
    return


opcja = 1
while opcja:
    print("-----MENU GŁÓWNE-----")
    print("1. Modulacja BPSK")
    print("2. Modulacja QPSK")
    print("0. Wyjdź z programu")
    opcja = input("Podaj numer opcji: ")
    opcja = int(opcja)
    if (opcja < 0) or (opcja > 2):
        print("Program nie obsługuje podanej opcji")
        continue
    if opcja:
        passThrough(opcja)
exit(0)
