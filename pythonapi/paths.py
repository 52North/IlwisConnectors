import os
f = open('ilwislocation.config', 'r')
f.readline()
line = f.readline().strip()
line = line.split('=')[1]
f.close()
os.environ["PATH"] = line + os.pathsep + os.environ["PATH"]

