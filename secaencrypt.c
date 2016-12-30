/*  secalinux: Emulates a SECA Smartcard

    Copyright (C) 2000  2tor (2tor@zdnetonebox.com)
    Part of original code and algorithms by Fullcrack (fullcrack@teleline.es)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


    This software is for educational purposes only. It should not be used
    to ilegally watch TV channels.

    Version 0.1, December 2000.  
*/

#include "secaencrypt.h"

#define SN(b) (((b&0xf0)>>4)+((b&0xf)<<4))
static unsigned char TD[4] = {1,3,0,2};

static unsigned char T1[]={
  0x2a,0xe1,0x0b,0x13,0x3e,0x6e,0x32,0x48,
  0xd3,0x31,0x08,0x8c,0x8f,0x95,0xbd,0xd0,
  0xe4,0x6d,0x50,0x81,0x20,0x30,0xbb,0x75,
  0xf5,0xd4,0x7c,0x87,0x2c,0x4e,0xe8,0xf4,
  0xbe,0x24,0x9e,0x4d,0x80,0x37,0xd2,0x5f,
  0xdb,0x04,0x7a,0x3f,0x14,0x72,0x67,0x2d,
  0xcd,0x15,0xa6,0x4c,0x2e,0x3b,0x0c,0x41,
  0x62,0xfa,0xee,0x83,0x1e,0xa2,0x01,0x0e,
  0x7f,0x59,0xc9,0xb9,0xc4,0x9d,0x9b,0x1b,
  0x9c,0xca,0xaf,0x3c,0x73,0x1a,0x65,0xb1,
  0x76,0x84,0x39,0x98,0xe9,0x53,0x94,0xba,
  0x1d,0x29,0xcf,0xb4,0x0d,0x05,0x7d,0xd1,
  0xd7,0x0a,0xa0,0x5c,0x91,0x71,0x92,0x88,
  0xab,0x93,0x11,0x8a,0xd6,0x5a,0x77,0xb5,
  0xc3,0x19,0xc1,0xc7,0x8e,0xf9,0xec,0x35,
  0x4b,0xcc,0xd9,0x4a,0x18,0x23,0x9f,0x52,
  0xdd,0xe3,0xad,0x7b,0x47,0x97,0x60,0x10,
  0x43,0xef,0x07,0xa5,0x49,0xc6,0xb3,0x55,
  0x28,0x51,0x5d,0x64,0x66,0xfc,0x44,0x42,
  0xbc,0x26,0x09,0x74,0x6f,0xf7,0x6b,0x4f,
  0x2f,0xf0,0xea,0xb8,0xae,0xf3,0x63,0x6a,
  0x56,0xb2,0x02,0xd8,0x34,0xa4,0x00,0xe6,
  0x58,0xeb,0xa3,0x82,0x85,0x45,0xe0,0x89,
  0x7e,0xfd,0xf2,0x3a,0x36,0x57,0xff,0x06,
  0x69,0x54,0x79,0x9a,0xb6,0x6c,0xdc,0x8b,
  0xa7,0x1f,0x90,0x03,0x17,0x1c,0xed,0xd5,
  0xaa,0x5e,0xfe,0xda,0x78,0xb0,0xbf,0x12,
  0xa8,0x22,0x21,0x3d,0xc2,0xc0,0xb7,0xa9,
  0xe7,0x33,0xfb,0xf1,0x70,0xe5,0x17,0x96,
  0xf8,0x8d,0x46,0xa1,0x86,0xe2,0x40,0x38,
  0xf6,0x68,0x25,0x16,0xac,0x61,0x27,0xcb,
  0x5b,0xc8,0x2b,0x0f,0x99,0xde,0xce,0xc5};

static unsigned char T2[]={
  0xbf,0x11,0x6d,0xfa,0x26,0x7f,0xf3,0xc8,
  0x9e,0xdd,0x3f,0x16,0x97,0xbd,0x08,0x80,
  0x51,0x42,0x93,0x49,0x5b,0x64,0x9b,0x25,
  0xf5,0x0f,0x24,0x34,0x44,0xb8,0xee,0x2e,
  0xda,0x8f,0x31,0xcc,0xc0,0x5e,0x8a,0x61,
  0xa1,0x63,0xc7,0xb2,0x58,0x09,0x4d,0x46,
  0x81,0x82,0x68,0x4b,0xf6,0xbc,0x9d,0x03,
  0xac,0x91,0xe8,0x3d,0x94,0x37,0xa0,0xbb,
  0xce,0xeb,0x98,0xd8,0x38,0x56,0xe9,0x6b,
  0x28,0xfd,0x84,0xc6,0xcd,0x5f,0x6e,0xb6,
  0x32,0xf7,0x0e,0xf1,0xf8,0x54,0xc1,0x53,
  0xf0,0xa7,0x95,0x7b,0x19,0x21,0x23,0x7d,
  0xe1,0xa9,0x75,0x3e,0xd6,0xed,0x8e,0x6f,
  0xdb,0xb7,0x07,0x41,0x05,0x77,0xb4,0x2d,
  0x45,0xdf,0x29,0x22,0x43,0x89,0x83,0xfc,
  0xd5,0xa4,0x88,0xd1,0xf4,0x55,0x4f,0x78,
  0x62,0x1e,0x1d,0xb9,0xe0,0x2f,0x01,0x13,
  0x15,0xe6,0x17,0x6a,0x8d,0x0c,0x96,0x7e,
  0x86,0x27,0xa6,0x0d,0xb5,0x73,0x71,0xaa,
  0x36,0xd0,0x06,0x66,0xdc,0xb1,0x2a,0x5a,
  0x72,0xbe,0x3a,0xc5,0x40,0x65,0x1b,0x02,
  0x10,0x9f,0x3b,0xf9,0x2b,0x18,0x5c,0xd7,
  0x12,0x47,0xef,0x1a,0x87,0xd2,0xc2,0x8b,
  0x99,0x9c,0xd3,0x57,0xe4,0x76,0x67,0xca,
  0x3c,0xfb,0x90,0x20,0x14,0x48,0xc9,0x60,
  0xb0,0x70,0x4e,0xa2,0xad,0x35,0xea,0xc4,
  0x74,0xcb,0x39,0xde,0xe7,0xd4,0xa3,0xa5,
  0x04,0x92,0x8c,0xd9,0x7c,0x1c,0x7a,0xa8,
  0x52,0x79,0xf2,0x33,0xba,0x1f,0x30,0x9a,
  0x00,0x50,0x4c,0xff,0xe5,0xcf,0x59,0xc3,
  0xe3,0x0a,0x85,0xb3,0xae,0xec,0x0b,0xfe,
  0xe2,0xab,0x4a,0xaf,0x69,0x6c,0x2c,0x5d};


static void fase(unsigned char *k,unsigned char *D) {
// Modifica 4 bytes en D con 4 bytes de clave en k

  unsigned char l,dt; // paso 1 
  for(l=0;l<4;++l) D[l]^=k[l];  // paso 2 
  for(l=0;l<4;++l) D[l]=T1[D[l]];
  for(l=6;l>3;--l) { 
    D[(l+2)&3]^=D[(l+1)&3]; 
    dt=(SN(D[(l+1)&3])+D[l&3])&0xff; 
    D[l&3]=T2[dt]; 
  } 
  for(l=3;l>0;--l) {
    D[(l+2)&3]^=D[(l+1)&3]; 
    D[l&3]=T1[(SN(D[(l+1)&3])+D[l&3])&0xff]; 
  } 
  D[2]^=D[1]; 
  D[1]^=D[0]; 
}



void decrypt_seca( unsigned char *k,unsigned char *d ) {
  // k = clave de 128 bits (16 bytes),
  // d = datos de 64 bits (8 bytes)
  unsigned char i,j,l,C,dt,D[4];
  
  // preparacion de la clave
  C=0xff;
  for(j=0;j<4;++j) {
    for(i=0;i<16;++i)	{
      if((i&3)==0) ++C;
      k[i]^=T1[k[(15+i)&0xf]^k[(i+1)&0xf]^C]; 
    } 
  } // manipulacion de los datos
  j=0; // 16 rondas 
  for(i=0;i<16;++i) {
    for(l=0;l<4;++l) D[l]=d[l+4]; // Copia d5..d8 a D1..D4 
    j=(j+12)&0xf; // selecciona clave apropiada
    fase(&k[j],D); // modifica D con clave pertinente 
    // paso 3 
    for(l=0;l<4;++l) { // modifica d bajo
      d[l]^=T2[D[TD[l]]]; // paso 4
    }
    for(l=3;l!=0xff;--l) { // modifica k usado
      k[j+l]^=T1[k[(j+l+1)&0xf]^k[(j+l+15)&0xf]^(15-i)]; 
    }
    if(i<15) { // intercambia d altos y bajos 
      for(l=0;l<4;++l) { 
	dt=d[l];d[l]=d[4+l];d[4+l]=dt; 
      } 
    } 
  } 
}


void encrypt_seca( unsigned char *k,unsigned char *d ) { 
  // Rutina de encriptacion, no modifica la clave 
  // k=clave de 128 bits (16 bytes), d=datos de 64 bits (8 bytes) */
  unsigned char i,j,l,dt,D[4],kk[16];

  // primero hace copia de la clave
  for(i=0;i<16;++i) kk[i]=k[i]; // 16 rondas

  for(j=i=0;i<16;++i,j=(j+4)&0xf) { // paso 1 
    for(l=0;l<4;++l) { 
      kk[j+l]^=T1[kk[(j+l+1)&0xf]^kk[(j+l+15)&0xf]^i]; 
    } 
    if(i>0) { // intercambia d altos y bajos 
      for(l=0;l<4;++l) {
	dt=d[l];d[l]=d[4+l];d[4+l]=dt; }
    } 
    for( l=0; l<4; ++l ) D[l]=d[l+4]; // Copia d5..d8 a D1..D4 
    fase( &kk[j],D ); // modifica D con clave pertinente 
    // paso 2 
    for(l=0;l<4;++l) { // modifica d bajo
      d[l]^=T2[D[TD[l]]]; 
    } 
  } 
}
