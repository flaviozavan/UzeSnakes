/*
 *  UzeSnakesPatches
 *  Copyright (C) 2010  Flávio Zavan
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//Some were taken from Alec Bourque's Uzebox Default Patches (licensed under the same license) and are indicated

//INST: Synth Piano - Alec's
const char patch00[] PROGMEM ={ 
0,PC_WAVE,6,
0,PC_ENV_SPEED,-5,
0,PATCH_END
};

//INST: Pizzicato - Alec's
const char patch01[] PROGMEM ={ 
0,PC_WAVE,9,
0,PC_ENV_SPEED,-15,
0,PATCH_END
};

//Snake died
const char patch02[] PROGMEM ={
0,PC_NOISE_PARAMS,40,
0,PC_ENV_SPEED,-5, 
250,PC_NOISE_PARAMS,1,
2,PC_NOTE_CUT,0,		 
0,PATCH_END
};

//Ate an item
const char patch03[] PROGMEM ={
0,PC_NOISE_PARAMS,5,
0,PC_ENV_SPEED,-30, 
25,PC_NOISE_PARAMS,3,
2,PC_NOTE_CUT,0,		 
0,PATCH_END
};

//Speed Increased
const char patch04[] PROGMEM ={
0,PC_NOISE_PARAMS,0,
0,PC_ENV_SPEED,-30, 
15,PC_NOISE_PARAMS,3,
2,PC_NOTE_CUT,0,		 
0,PATCH_END
};

//Finished
const char patch05[] PROGMEM ={
0,PC_NOISE_PARAMS,0,
0,PC_ENV_SPEED,-5, 
150,PC_NOISE_PARAMS,3,
2,PC_NOTE_CUT,0,		 
0,PATCH_END
};

//Logo "kling" sound - Alec's
const char patch06[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_PITCH,85,
4,PC_PITCH,90,
0,PC_ENV_SPEED,-8,   
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,30, 
50,PC_NOTE_CUT,0,
0,PATCH_END  
};

const struct PatchStruct patches[] PROGMEM = {
{0,NULL,patch00,0,0},
{0,NULL,patch01,0,0},
{1,NULL,patch02,0,0},
{1,NULL,patch03,0,0},
{1,NULL,patch04,0,0},
{1,NULL,patch05,0,0},
{0,NULL,patch06,0,0},
};
