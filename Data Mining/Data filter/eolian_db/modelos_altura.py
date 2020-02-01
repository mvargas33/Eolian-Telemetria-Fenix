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

class Real_data(db.Entity):
    id = PrimaryKey(int)
    dist = Required(float)        # Identificador de dato: Distancia REAL recorrida
    alt = Optional(float)           # Comienza con vacio

    def __str__(self):
        return "{},{},{}\n".format(self.id, self.dist, self.alt)


class Reference_data(db.Entity):
    distancia = Required(float)   # Distancia de ruta linkeada a altura
    altura = Required(float)        # Altura linkeada a la distancia


    def __str__(self):
        return "Referencial: {},{}\n".format(self.distancia, self.altura)


class ValoresDeKm(db.Entity):
    km = Required(float)
    altura = Optional(float)


    def __str__(self):
        return "Km y altura: {},{}\n".format(self.km, self.altura)


# Se rellena la base de datos con las tablas respectivas
db.generate_mapping(create_tables=True)
