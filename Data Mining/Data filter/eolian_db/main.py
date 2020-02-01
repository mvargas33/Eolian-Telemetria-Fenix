import csv
from modelos import *

input = "18_12_2017_light.csv"

def import_data():
    with open(input, "r") as datos:
        table = csv.DictReader(datos, fieldnames=["Id", "Hora", "Velocidad", "SOC", "Corriente", "Voltaje", "Voltaje abierto",
                                                  "Max V", "Min V", "1 V", "1 C", "2 V", "2 C", "3 V", "3 C", "4 V",
                                                  "4 C"])
        with db_session:
            for row in table:
                if DataLight.get(id=int(row['Id'])) is None:
                    dato = DataLight(id=int(row['Id']), time=row['Hora'], speed=float(row['Velocidad']),
                                     soc=float(row['SOC']), current=float(row['Corriente']), voltage=float(row['Voltaje']),
                                     open_voltage=float(row['Voltaje abierto']), max_v=float(row['Max V']),
                                     min_v=float(row['Min V']), mppt1_v=float(row['1 V']), mppt1_c=float(row['1 C']),
                                     mppt2_v=float(row['2 V']), mppt2_c=float(row['2 C']),
                                     mppt3_v=float(row['3 V']), mppt3_c=float(row['3 C']),
                                     mppt4_v=float(row['4 V']), mppt4_c=float(row['4 C']))

def print_all_data():
    with db_session:
        all_data = select(dato for dato in DataLight)
        for data in all_data:
            print(data)

def delete_outrange_data():
    with db_session:
        bad_data_1 = select(dato for dato in DataLight if
                          ((0 < dato.speed and dato.speed < 1e-5) or dato.speed > 90)
                          or ((0 < dato.soc and dato.soc < 1e-5) or dato.soc > 100)
                          or (0 < dato.current and dato.current < 1e-5)
                          or (0 < dato.voltage and dato.voltage < 1e-5)
                          or (0 < dato.open_voltage and dato.open_voltage< 1e-5)
                          or (0 < dato.max_v and dato.max_v < 1e-5) or (0 < dato.min_v and dato.min_v < 1e-5)
                          or (0 < dato.mppt1_v and dato.mppt1_v < 1e-5) or (0 < dato.mppt1_c and dato.mppt1_c < 1e-5)
                          or (0 < dato.mppt2_v and dato.mppt2_v < 1e-5) or (0 < dato.mppt2_c and dato.mppt2_c < 1e-5)
                          )
        bad_data_2 = select(dato for dato in DataLight if
                          (0 < dato.mppt3_v and dato.mppt3_v < 1e-5) or (0 < dato.mppt3_c and dato.mppt3_c < 1e-5)
                          or (0 < dato.mppt4_v and dato.mppt4_v < 1e-5) or (0 < dato.mppt4_c and dato.mppt4_c < 1e-5)
                          )
        for dat in bad_data_1:
            print(dat)
            DataLight[dat.id].delete()

        for dat in bad_data_2:
            print(dat)
            DataLight[dat.id].delete()

def export_clean_data():
    log = open("{} - Clean.txt".format(input), "w+")
    log.write("Id,Hora,Velocidad,SOC,Corriente,Voltaje,Voltaje abierto,Max Volt,Min Volt,"
              "1 Volt IN,1 Current IN,2 Volt IN,2 Current IN,3 Volt IN,3 Current IN, 4 Volt IN, 4 Current IN\n")
    with db_session:
        all_data = select(dato for dato in DataLight)
        for data in all_data:
            log.write(str(data))
    log.close()


import_data()
# print_all_data()
delete_outrange_data()
export_clean_data()
