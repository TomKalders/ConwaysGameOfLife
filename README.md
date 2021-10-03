# ConwaysGameOfLife
A c++ version of Conway's Game Of Life.

# About
This is Conway's Game Of Life using cellular automata.
The Game of life is based on 3 basic conditions.
for each cell in the grid, you check if that cell is alive.
If the cell is alive, it will die if it either has less than 2 or more than 3 neighbours.
If the cell is dead and has exactly 3 neighbours, the cell will become alive.

For creating and drawing on the screen I used SDL2, as well as for capturing input from the keyboard and mouse.
I made a small framework using the basic game loop. Where I handle input, update the scene and render it to the screen.

# Controls
- Spacebar: start/stop simulating
- Enter: show/hide the grid
- Backspace: clear the grid

# Features I might add later
- On screen/in console settings to choose resolution, the size of the cells, etc..
- Loading/saving data to a file so you can load previously made scenes or save scenes you made

