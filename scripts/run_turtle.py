import turtle
import re

with open('./turtle', 'r') as f:
    lines = f.readlines()

t = turtle.Turtle()
t.speed(1)  # Fastest

for line in lines:
    if 'Avance' in line:
        dist = int(re.search(r'\d+', line).group())
        t.forward(dist)
    elif 'Recule' in line:
        dist = int(re.search(r'\d+', line).group())
        t.backward(dist)
    elif 'Tourne gauche' in line:
        angle = int(re.search(r'\d+', line).group())
        t.left(angle)
    elif 'Tourne droite' in line:
        angle = int(re.search(r'\d+', line).group())
        t.right(angle)

turtle.done()