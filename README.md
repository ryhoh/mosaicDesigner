# mosaicDesigner
A program for making a square bitmap (mosaic) image.

## How it works?
This program make an image from two texts data named design.txt and pattern.txt  

The file "design.txt" indicates design.    
Format:  
16								\# size (width / height)  
2121212121212121	\# 1 means color1  
1212121212121212	\# 2 means color2  
2121212121212121	\# (n) means color(n)  
1212121212121212	\# (max 15 colors + background)  
2121212121212121	  
1212121212121212	  
2121212121212121	  
1212121212121212	  
2121212121212121	  
1212121212121212	  
2121212121212121	  
1212121212121212	  
2121212121212121	  
1212121212121212	  
2121212121212121	  
1212121212121212	  
  
You can also write like this.  
2  
21  
12  
If you choose small data, it will be expanded automatically.
  
The file "pattern.txt" indicates the shape of the image.  
16  								\# size (width / height)  
0000000000000000	\# 0 means background  
0000000010000000	\# 1 means color  
0000010010010000  
0000100100100000  
0001001001000000  
0001001001000000  
0001001001000000  
0000100100100000  
0010010010010100  
0100010010010010  
1000010010010001  
1000100100100001  
1000000100000001  
0110000000000110  
0011000000001100  
0000111111110000  
  
