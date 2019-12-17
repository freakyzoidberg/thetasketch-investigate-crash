import parquet
import json


#parquet.dump_metadata("events.parquet", True)

with open("events.parquet") as fo:
   for row in parquet.reader(fo, columns=["theta_sketch"]):
      print("".join("{:02x}".format(ord(c)) for c in row[0]))
 
