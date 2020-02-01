import csv
import math
from modelos_altura import *

input = "Altimetria_de_ruta_light.csv"
real_input = "Real_altura.csv"

Km = []
Altura = []

def import_data():
    with open(input, "r") as datos:
        table = csv.DictReader(datos, fieldnames=["Distance (m)", "Elevation (m)"])
        with db_session:
            for row in table:
                if Reference_data.get(distancia=float(row['Distance (m)'])) is None:
                    dato = Reference_data(distancia=float(row['Distance (m)'])/1000, altura=float(row['Elevation (m)']))


def import_real_dist():
    with open(real_input, "r") as datos:
        table = csv.DictReader(datos, fieldnames=["Id", "Distance (Km)"])
        with db_session:
            for row in table:
                if Real_data.get(dist=float(row['Id'])) is None:
                    dato = Real_data(id=int(row['Id']), dist=float(row['Distance (Km)']))
                    print(dato)


def import_diccionario():
    with open(real_input, "r") as datos:
        table = csv.DictReader(datos, fieldnames=["Id", "Distance (Km)"])
        with db_session:
            for row in table:
                if ValoresDeKm.get(km=float(row['Distance (Km)'])) is None:
                    dato = ValoresDeKm(km=float(row['Distance (Km)']))
                    print(dato)


def print_all_data():
    with db_session:
        all_data = select(dato for dato in Reference_data)
        for data in all_data:
            print(data)

        dicc_data = select(dato for dato in ValoresDeKm)
        for data in dicc_data:
            print(data)

        #all_data_real = select(dato for dato in Real_data)
        #for data in all_data_real:
        #    print(data)
'''
def generarArrayDeRealData():
    with db_session:
        all_data_real = select(dato for dato in Real_data)
        for data in all_data_real:
            if not(estaEnArray(data.distancia, Km)):
                Km.append(data.distancia)
                print(Km)
                

def asociarAlturaAKmEnArrays():
    with db_session:
        for i in range(0, len(Km)):     
                min = 10 ** 100
                altura_data = select(dato for dato in Reference_data)
                for alt_data in altura_data:
                    if math.fabs(Km[i] - alt_data.distancia) < min:
                        Altura[i] = alt_data.distancia
                        min = math.fabsmath.fabs(Km[i] - alt_data.distancia)
        print(Km)
        print(Altura)
            
def estaEnArray(elemento, array):
    for elem in array:
        if elemento == elem:
            return True
    return False
'''
def asociarDiccionario():
    with db_session:
        dicc_data = select(dato for dato in ValoresDeKm)
        for dic in dicc_data:
            min = 10**100
            altura_data = select(dato for dato in Reference_data)
            for alt_data in altura_data:
                if math.fabs(alt_data.distancia - dic.km) < min:
                    dic.altura = alt_data.altura
                    min = math.fabs(alt_data.distancia - dic.km)
            print(dic)


def forzarAlturaEnTexto():
    full_data = []
    inputt = "Real_altura.txt"
    text = open(inputt, 'r+')
    for line in text:
        a = line.rstrip("\n").split(",")  # Quitar \n de la linea y dividir por comas
        with db_session:
            dicc_data = select(dato for dato in ValoresDeKm)
            for dic in dicc_data:
                print("Dic Km: " + str(dic.km))
                print("Arr Km: " + a[1])
                if float(a[1]) == dic.km:
                    print("Calza")
                    b = a[0] + "," + a[1] + "," + str(dic.altura) + "\n"
                    full_data.append(b)
                    break
    return full_data


def FullDataToText(Full_data_array):
    out = open("Id-Km-Altura.txt", 'w')
    for line in Full_data_array:
        out.write(line)


def asociarAltura():
    with db_session:
        real_data = select(dato for dato in Real_data)
        for r_data in real_data:
            min = 10**100
            altura_data = select(dato for dato in Reference_data)
            for alt_data in altura_data:
                if math.fabs(alt_data.distancia - r_data.dist) < min:
                    r_data.alt = alt_data.altura
                    min = math.fabs(alt_data.distancia - r_data.dist)
            print(r_data)


def export_data():
    log = open("{} - Clean.txt".format(real_input), "w+")
    log.write("Id,Distancia [Km],Altura [m]\n")
    with db_session:
        all_data = select(dato for dato in Real_data)
        for data in all_data:
            log.write(str(data))
    log.close()


#import_data()
#import_diccionario()
#asociarDiccionario()
FullDataToText(forzarAlturaEnTexto())
#print_all_data()

#import_real_dist()
#generarArrayDeRealData()
#asociarAlturaAKmEnArrays()
#print_all_data()
#asociarAltura()
#print_all_data()
#export_data()
