# Chaotic-Equation-Simulator

## Overview
This project is a Chaos Equation Simulator inspired by the [Chaos Equations - Simple Mathematical Art](https://www.youtube.com/watch?v=fDSIRXmnVvk&ab_channel=CodeParade) Youtube video by [CodeParade](https://www.youtube.com/channel/UCrv269YwJzuZL3dH5PCgxUw).


![](https://github.com/MurilloMMartins/Chaotic-Equation-Simulator/blob/master/showcase.gif)


This program was written in C++, using the GLFW and GLAD libraries for a OpenGL implementation.

## Chaos Equations
In the context of the project, chaos equations (or chaotic equations) are simple polynomial equations that apply their results to themselves, for example, the pattern shown in the gif above had the equations:

$$x' = -x^2 -xy -xt + yt+tt+t\\ y'=x^2-xy+xt+t$$

First we start with $x = y =t$, and then show this point in this $x$ and $y$ position. After this we compute a new $x'$ and $y'$ that will be the position of a new point. Then we repeat this process for all the points we want to show on screen.