/*
 *  Copyright (C) 2005-2021 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2010 Terence M. Welsh
 *  Ported to Kodi by Alwin Esch <alwinus@kodi.tv>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

/*
 * Code is based on:
 *   https://github.com/reallyslickscreensavers/reallyslickscreensavers
 * and reworked to GL 4.0.
 */

#include "gizmo.h"
#include "main.h"

Gizmo::Gizmo(CScreensaverMicrocosm* base) : m_base(base)
{
  mComputeComplexity = 1.0f;

  // Most Gizmos will stay partially inside the impCubeVolume of width 1.0 most of
  // the time using this nominal diplacement value
  mMaxDisplacement = 0.5f;

  mScale = 1.0f;

  for(unsigned int i=0; i<GIZMO_NUM_COEFFS; ++i)
  {
    mCoeffRate[i] = rsRandf(0.01f * float(m_base->Settings().dGizmoSpeed)) + 0.005f * float(m_base->Settings().dGizmoSpeed);
    mCoeffPhase[i] = rsRandf(RS_PIx2) - RS_PI;
    // test constants
    //mCoeffRate[i] = 0.02f * float(i) + 0.02f;
    //mCoeffPhase[i] = 0.0f;
  }
}

Gizmo::~Gizmo()
{
  mShapes.clear();
}

void Gizmo::setScale(float s)
{
  mScale = s;
}

float Gizmo::value(float* position)
{
  float value(0.0f);
  for(unsigned int i=0; i<mShapes.size(); ++i)
    value += mShapes[i]->value(position);
  return value;
}

void Gizmo::getShapes(ShapeVector &sv)
{
  for(unsigned int i=0; i<mShapes.size(); ++i)
    sv.push_back(mShapes[i]);
}

void Gizmo::addCrawlPoints(impCrawlPointVector &cpv)
{
  for(unsigned int i=0; i<mShapes.size(); ++i)
    mShapes[i]->addCrawlPoint(cpv);
}

void Gizmo::updateConstants(float frametime)
{
  for(unsigned int i=0; i<GIZMO_NUM_COEFFS; ++i)
  {
    mCoeffPhase[i] += mCoeffRate[i] * frametime;
    if(mCoeffPhase[i] > RS_PI)
      mCoeffPhase[i] -= RS_PIx2;
    mCoeff[i] = cosf(mCoeffPhase[i]);
  }
}

void Gizmo::updateMatrix()
{
  if (m_base->Mode() == 0)
  {
    // Gizmo stays in center, no drifting to sides of impVolume and no rotating
    mMatrix.makeScale(mScale);
  }
  else
  {
    mMatrix.makeRotate(mCoeff[0] * 2.0f, 1.0f, 0.0f, 0.0f);
    mMatrix.rotate(mCoeff[1] * 2.0f, 0.0f, 1.0f, 0.0f);
    mMatrix.rotate(mCoeff[2] * 2.0f, 0.0f, 0.0f, 1.0f);
    mMatrix.scale(mScale);
    mMatrix.translate(mCoeff[3] * mMaxDisplacement, mCoeff[4] * mMaxDisplacement, mCoeff[5] * mMaxDisplacement);
  }
}

void Gizmo::update(float frametime)
{
  updateConstants(frametime);
  updateMatrix();

  for(unsigned int i=0; i<mShapes.size(); ++i)
    mShapes[i]->setMatrix(mMatrix.get());
}
