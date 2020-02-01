from pony.orm import *
'''
Primarykey: ID único del objeto
Required: Si el parametro no esta, se cae el programa, es obligatorio
Optional: El parametro puede no estar, por ej, el sig_id en el Coinbase
Set: Lista de objetos, ya que es mas que uno
'''
db = Database()
# Se asocia con una BD Sqlite (Archivo db.sqlite que se creará en la carpeta del proyecto)
db.bind(provider="sqlite", filename="db.sqlite", create_db=True)

class DataLight(db.Entity):
    id = PrimaryKey(int)        # Identificador de dato
    time = Required(str)
    speed = Optional(float)
    soc = Optional(float)
    current = Optional(float)
    voltage = Optional(float)
    open_voltage = Optional(float)
    max_v = Optional(float)
    min_v = Optional(float)
    mppt1_v = Optional(float)
    mppt1_c = Optional(float)
    mppt2_v = Optional(float)
    mppt2_c = Optional(float)
    mppt3_v = Optional(float)
    mppt3_c = Optional(float)
    mppt4_v = Optional(float)
    mppt4_c = Optional(float)

    def __str__(self):
        return "{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}\n"\
            .format(self.id, self.time, self.speed, self.soc, self.current, self.voltage,self.open_voltage, self.max_v,
                    self.min_v, self.mppt1_v, self.mppt1_c, self.mppt2_v, self.mppt2_c, self.mppt3_v, self.mppt3_c,
                    self.mppt4_v, self.mppt4_c)

    '''
    def __str__(self):
        return "DataLight(id={}, time={}, speed={} ,soc={} ,current={} ,voltage={} ,open_voltage={} ,max_v={} ," \
               "min_v={} ,mppt1_v={} ,mppt_c={} ,mppt2_v={} ,mppt2_c={} ,mppt3_v={} ,mppt3_c={} ,mppt4_v={} ," \
               "mpp4_c={})".format(self.id, self.time, self.speed, self.soc, self.current, self.voltage,
                                   self.open_voltage, self.max_v, self.min_v, self.mppt1_v, self.mppt1_c,
                                   self.mppt2_v, self.mppt2_c, self.mppt3_v, self.mppt3_c, self.mppt4_v,
                                   self.mppt4_c)
    '''

# Se rellena la base de datos con las tablas respectivas
db.generate_mapping(create_tables=True)
