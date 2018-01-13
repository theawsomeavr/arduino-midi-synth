/*
void drawSprite(unsigned int grid[8], unsigned int sprite[], int spriteHeight, int posx, int posy)  {
 for(int i = 0; i < 8; i++)  {
 if(i - posy >= 0 && i - posy <= spriteHeight -1)  {
 if(posx >= 0)  {
 grid[i] |= sprite[i-posy] >> posx;
 }
 else  {
 grid[i] |= sprite[i-posy] << -posx;
 }
 }
 }
 }
 
 void drawSprite(unsigned int grid[8], byte sprite[], int spriteHeight, int posx, int posy)  {
 for(int i = 0; i < 8; i++)  {
 if(i - posy >= 0 && i - posy <= 7)  {
 if(posx >= 0)  {
 grid[i] |= sprite[i-posy] >> posx;
 }
 else  {
 grid[i] |= sprite[i-posy] << -posx;
 }
 }
 }
 }
 
 
 
 void drawLine(unsigned int grid[8], int fromX, int fromY, int toX, int toY)  {
 int deltaX = fromX - toX + (fromX >= toX ? 1 : -1);  //in hindsight, I can't remember why I put that conditional +-1 part...
 int deltaY = fromY - toY + (fromY >= toY ? 1 : -1);  //But it works, so I'm keeping it
 if(abs(deltaY) <= abs(deltaX))  {  //Is it tall or wide?
 if(fromX < toX)  {               //Forwards or back?
 for(int i = fromX; i <= toX; i++)  {
 int Ycoord = (100*deltaY/deltaX)*(i-fromX)/100+fromY;
 if(i >= 0 && i <= 15 && Ycoord >= 0 && Ycoord <= 7)  {
 grid[Ycoord] |= 0x8000 >> i;
 }
 }
 }
 else  {
 for(int i = toX; i <= fromX; i++)  {
 int Ycoord = (100*deltaY/deltaX)*(i-fromX)/100+fromY;
 if(i >= 0 && i <= 15 && Ycoord >= 0 && Ycoord <= 7)  {
 grid[Ycoord] |= 0x8000 >> i;
 }
 }
 }
 
 }
 else  {
 if(fromY < toY)  {              //Up or down?
 for(int i = fromY; i <= toY; i++)  {
 int Xcoord = (100*deltaX/deltaY)*(i-fromY)/100+fromX;
 if(i >= 0 && i <= 7 && Xcoord >= 0 && Xcoord <= 15)  {
 grid[i] |= 0x8000 >> Xcoord;
 }
 }
 }
 else  {
 for(int i = toY; i <= fromY; i++)  {
 int Xcoord = (100*deltaX/deltaY)*(i-fromY)/100+fromX;
 if(i >= 0 && i <= 7 && Xcoord >= 0 && Xcoord <= 15)  {
 grid[i] |= 0x8000 >> Xcoord;
 }
 }
 }
 
 }
 } */


// Function to write any 4x4 sprite, stored in an unsigned int, on the grid arrays
//origin is the upper left corner of 4x4 box
void drawChar(unsigned int grid[8], unsigned int charSprite, int originX, int originY)  {
  byte bits = 0;

  for(int y = originY+0; y <= originY+3; y++)  {
    if(y >=0 && y <= 7)  {
      if(originX >= 0)  {
        grid[y] |= ((charSprite << bits) & 0xf000) >> originX;
      }
      else  {
        grid[y] |= ((charSprite << bits) & 0xf000) << -originX;
      }
    }
    bits += 4;
  }


}   

void clearGrid(unsigned int grid[8])  {
  for(int i = 0; i < 8; i++)  {
    grid[i] = 0;
  }
}

