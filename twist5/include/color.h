//     :     :     :     :     :     :     :     :     :     :
//   --------:  ---------:  ---------:  ---------:  ----     :
// ..|      |*..|       |*..|       |*..|       |*..|  |*....:.
//   |      |*  |       |*  |       |*  |       |*  |  |*    :
//   ---  ---*  |  |-----*  |  |-----*  |  |-----*  |  |*    :
// ....|  |*.:..|  |--****..|  |-----*..|  |--****..|  |*....:.
//     |  |* :  |    |*  :  |  |--  |*  |    |*  :  |  |*    :
//     |  |* :  |  |--*  :  |  |*|  |*  |  |--*  :  |  |*    :
// ....|  |*.:..|  |-----:..|  ---  |*..|  |-----:..|  -----.:.
//     |  |* :  |       |*  |       |*  |       |*  |      |*:
//     |  |* :  |       |*  |       |*  |       |*  |      |*:
// ....----*.:..---------*..---------*..---------*..--------*:.
//     :**** :   *********   *********   *********   ********:
//     :     :     :     :     :     :     :     :     :     :
//
// Version 0.82
// Copyright (c) 2002-2007
// Martijn "Amarillion" van Iersel.
//
// This file is part of the Tegel tilemapping add-on for allegro
//
//    Tegel is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General 
//    Public License as published by the Free Software 
//    Foundation; either version 2.1 of the License, or
//    (at your option) any later version.
//
//    Tegel is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with Tegel; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//	
// official website:
// http://tegel.sourceforge.net/
//
// I hope you like this program. Please send 
// questions, comments, flames and feature 
// requests to amarillion@yahoo.com.
//
#ifndef COLOR_H
#define COLOR_H

#define RED al_map_rgb(255, 0, 0)
#define LIGHT_BLUE al_map_rgb(128, 128, 255)
#define BLUE al_map_rgb(0, 0, 255)
#define DARK_BLUE al_map_rgb(0, 0, 192)
#define GREEN al_map_rgb(0, 255, 0)
#define DARK_GREEN al_map_rgb(0, 192, 0)
#define YELLOW al_map_rgb(255, 255, 0)
#define BLACK al_map_rgb(0, 0, 0)
#define WHITE al_map_rgb(255, 255, 255)
#define LIGHT_GREY al_map_rgb(192, 192, 192)
#define GREY al_map_rgb(128, 128, 128)
#define DARK_GREY al_map_rgb(64, 64, 64)
#define CYAN al_map_rgb(0, 255, 255)
#define MAGENTA al_map_rgb(255, 0, 255)
// TODO: replace use of MAGIC_PINK with TRANSPARENT
#define MAGIC_PINK al_map_rgba(0, 0, 0, 0)
#define TRANSPARENT al_map_rgba(0, 0, 0, 0)
#define ORANGE al_map_rgb(255, 192, 0)
#define BROWN al_map_rgb(192, 168, 128)

struct ALLEGRO_COLOR;

// color generation functions
ALLEGRO_COLOR getRainbowColor(int index, int span);

#endif
