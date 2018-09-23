# steiner_chain
Interactive visualization of 'Steiners Porism'

Use "Release" instead of "Debug" for best performance

## Controls

hold and drag with left mouse button to move middle circles

press the right mouse button to create circles in circles

press DEL when holding a circle to delete it

press 1-6 to create recursive circles with 1-6 layers

press 0 to clear

## Main ideas

SuperCircle objects have an associated Inversion object which takes it and the middle circle stored in it to concentric circles.
This is how movement is done.

The moveMiddle method works by binary searching the distance for an inversion circle of radius 1 to be from two concentric circles which represent
the SuperCircle and its middle child so that the distance from the middle circle to the outer circle is the desired value. It then performs a 
coupe of transformations to scale and translate to the desired places.

The reason this is done instead of just inverting the midpoint is because the midpoint of a circle is not mapped to the midpoint of its inversion, and 
an exact solution would include 6th powers and such. (wolfram alpha solution of
[a system of equations obtained by these equations](https://en.wikipedia.org/wiki/Inversive_geometry#Transforming_circles_into_circles) )

