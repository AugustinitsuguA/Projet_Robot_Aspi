import numpy as np
# Valeurs de tension
x = [3.05,3,2.75,2.35,2.03,1.78,1.57,1.4,1.27,1.08,0.92,0.8,0.72,0.65,0.53,0.41,0.38,0.3]
# Valeurs de distance
y = [3,3.5,4,5,6,7,8,9,10,12,14,16,18,20,25,30,35,40]

X = np.column_stack([np.ones_like(x),x,x**2,x**3])
print(X)