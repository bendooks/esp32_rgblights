// anim.cc
//
// Copyright 2018 Ben Dooks <ben@fluff.org>

//#include <stdint.h>
#include <stddef.h>
#include <stdint.h>

#define u8_t uint8_t

#include <Arduino.h>
#include <NeoPixelBus.h>
#include "anim.h"

ChaserAnim::ChaserAnim(RgbColor *pix, int nr_pix)
{
  this->nrpix = nr_pix;
  this->pix = pix;
  this->flpix = 0;
}

void ChaserAnim::updateState(void)
{
  int px;
  for (px = 0; px < nrpix; px++) {
    if (px != flpix) {
      pix[px].R = 0;
      pix[px].G = 255;
      pix[px].B = 0;
    } else {
      pix[px].R = 255;
      pix[px].G = 255;
      pix[px].B = 255;
    }
  }

  flpix += 1;
  if (flpix > nrpix)
    flpix = 0;
}

ChaserAnim2::ChaserAnim2(RgbColor *pix, int nr_pix)
{
  this->nrpix = nr_pix;
  this->pix = pix;
  this->flpix = 0;
  a_r = f_r = 0;
  a_g = f_g = 255;
  a_b = f_b = 0;  
}

void ChaserAnim2::updateState(void)
{
  int px;
  for (px = 0; px < nrpix; px++) {
    if (px > flpix) {
      pix[px].R = f_r;
      pix[px].G = f_g;
      pix[px].B = f_b;
    } else if (px <flpix) {
      pix[px].R = a_r;
      pix[px].G = a_g;
      pix[px].B = a_b;
    } else {
      pix[px].R = 255;
      pix[px].G = 255;
      pix[px].B = 255;
    }
  }

  flpix += 1;
  if (flpix > nrpix) {
    flpix = 0;

     f_g = a_g;
     f_r = a_r;
     f_b = a_b;

    switch (nxcol) {
      case 0: a_r = 0; a_g = 255; a_b = 0; nxcol = 1; break;
      case 1: a_r = 255; a_g = 255; a_b = 0; nxcol = 2;break;
      case 2: a_r = 255; a_g = 0; a_b = 0; nxcol = 3;break;
      case 3: a_r = 0; a_g = 0; a_b = 255; nxcol = 4;break;
      case 4: a_r = 0; a_g = 255; a_b = 255; nxcol = 5;break;
      case 5: a_r = 255; a_g = 0; a_b = 255; nxcol = 0;break;
      default: nxcol = 0;  
    }
  }
}

ChaserAnimOverlay::ChaserAnimOverlay(RgbColor *pix, int nr_pix)
{
  this->nrpix = nr_pix;
  this->pix = pix;
  this->flpix = 0;
}

void ChaserAnimOverlay::updateState(void)
{
  int px;
  for (px = 0; px < nrpix; px++) {
    if (px != flpix) {
    } else {
      pix[px].R = 255;
      pix[px].G = 255;
      pix[px].B = 255;
    }
  }

  flpix += 1;
  if (flpix > nrpix)
    flpix = 0;
}

SineWaveAnim::SineWaveAnim(RgbColor *pix, int nr_pix, String shrt, String lng, u8_t r, u8_t g, u8_t b)
{
  this->nrpix = nr_pix;
  this->pix = pix;
  this->angle = 0.0;
  this->r = r;
  this->b = b;
  this->g = g;
  this->longname = lng;
  this->shortname = shrt;
}

void SineWaveAnim::updateState(void)
{
  int px;
  double curang = angle;
  
  for (px = 0; px < nrpix; px++, curang += ((2 * PI) / nrpix)) {
      pix[px].R = r * sin(curang);
      pix[px].G = g * sin(curang);
      pix[px].B = b * sin(curang);
  }

  angle += ((2 * PI) / nrpix);
}
