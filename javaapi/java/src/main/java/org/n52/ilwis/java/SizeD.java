/* 
Copyright (C) 2007 - 2015 52°North Initiative for Geospatial Open Source
Software GmbH

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as published
by the Free Software Foundation.

If the program is linked with libraries which are licensed under one of
the following licenses, the combination of the program with the linked
library is not considered a "derivative work" of the program:

      • Apache License, version 2.0
      • Apache Software License, version 1.0
      • GNU Lesser General Public License, version 3
      • Mozilla Public License, versions 1.0, 1.1 and 2.0
      • Common Development and Distribution License (CDDL), version 1.0

Therefore the distribution of the program linked with libraries licensed
under the aforementioned licenses, is permitted by the copyright holders
if the distribution is compliant with both the GNU General Public
License version 2 and the aforementioned licenses.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details. 
*/ 
/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.6
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.n52.ilwis.java;

public class SizeD {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected SizeD(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(SizeD obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        ilwisobjectsJNI.delete_SizeD(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public SizeD(double xSizeT, double ySizeT, double zSizeT) {
    this(ilwisobjectsJNI.new_SizeD__SWIG_0(xSizeT, ySizeT, zSizeT), true);
  }

  public SizeD(double xSizeT, double ySizeT) {
    this(ilwisobjectsJNI.new_SizeD__SWIG_1(xSizeT, ySizeT), true);
  }

  public SizeD(Size size) {
    this(ilwisobjectsJNI.new_SizeD__SWIG_2(Size.getCPtr(size), size), true);
  }

  public SizeD(SizeD size) {
    this(ilwisobjectsJNI.new_SizeD__SWIG_3(SizeD.getCPtr(size), size), true);
  }

  public double xsize() {
    return ilwisobjectsJNI.SizeD_xsize(swigCPtr, this);
  }

  public double ysize() {
    return ilwisobjectsJNI.SizeD_ysize(swigCPtr, this);
  }

  public double zsize() {
    return ilwisobjectsJNI.SizeD_zsize(swigCPtr, this);
  }

  public void setXsize(double x) {
    ilwisobjectsJNI.SizeD_setXsize(swigCPtr, this, x);
  }

  public void setYsize(double y) {
    ilwisobjectsJNI.SizeD_setYsize(swigCPtr, this, y);
  }

  public void setZsize(double z) {
    ilwisobjectsJNI.SizeD_setZsize(swigCPtr, this, z);
  }

  public java.math.BigInteger linearSize() {
    return ilwisobjectsJNI.SizeD_linearSize(swigCPtr, this);
  }

  public SizeD increase(SizeD sz) {
    long cPtr = ilwisobjectsJNI.SizeD_increase(swigCPtr, this, SizeD.getCPtr(sz), sz);
    return (cPtr == 0) ? null : new SizeD(cPtr, false);
  }

  public SizeD decrease(SizeD sz) {
    long cPtr = ilwisobjectsJNI.SizeD_decrease(swigCPtr, this, SizeD.getCPtr(sz), sz);
    return (cPtr == 0) ? null : new SizeD(cPtr, false);
  }

  public SizeD multiply(double f) {
    long cPtr = ilwisobjectsJNI.SizeD_multiply(swigCPtr, this, f);
    return (cPtr == 0) ? null : new SizeD(cPtr, false);
  }

  public boolean equals(SizeD sz) {
    return ilwisobjectsJNI.SizeD_equals(swigCPtr, this, SizeD.getCPtr(sz), sz);
  }

  public boolean notequal(SizeD sz) {
    return ilwisobjectsJNI.SizeD_notequal(swigCPtr, this, SizeD.getCPtr(sz), sz);
  }

  public boolean contains(Coordinate pix) {
    return ilwisobjectsJNI.SizeD_contains__SWIG_0(swigCPtr, this, Coordinate.getCPtr(pix), pix);
  }

  public boolean contains(Pixel pix) {
    return ilwisobjectsJNI.SizeD_contains__SWIG_1(swigCPtr, this, Pixel.getCPtr(pix), pix);
  }

  public boolean contains(PixelD pix) {
    return ilwisobjectsJNI.SizeD_contains__SWIG_2(swigCPtr, this, PixelD.getCPtr(pix), pix);
  }

  public String toString() {
    return ilwisobjectsJNI.SizeD_toString(swigCPtr, this);
  }

  public boolean isValid() {
    return ilwisobjectsJNI.SizeD_isValid(swigCPtr, this);
  }

}
