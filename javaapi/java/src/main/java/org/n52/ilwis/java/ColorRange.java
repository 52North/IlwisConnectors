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

public class ColorRange extends Range {
  private long swigCPtr;

  protected ColorRange(long cPtr, boolean cMemoryOwn) {
    super(ilwisobjectsJNI.ColorRange_SWIGUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(ColorRange obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        ilwisobjectsJNI.delete_ColorRange(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public ColorRange() {
    this(ilwisobjectsJNI.new_ColorRange__SWIG_0(), true);
  }

  public ColorRange(java.math.BigInteger tp, ColorModel.Value clrmodel) {
    this(ilwisobjectsJNI.new_ColorRange__SWIG_1(tp, clrmodel.swigValue()), true);
  }

  public ColorModel.Value defaultColorModel() {
    return ColorModel.Value.swigToEnum(ilwisobjectsJNI.ColorRange_defaultColorModel__SWIG_0(swigCPtr, this));
  }

  public void defaultColorModel(ColorModel.Value m) {
    ilwisobjectsJNI.ColorRange_defaultColorModel__SWIG_1(swigCPtr, this, m.swigValue());
  }

  public static Color toColor(java.math.BigInteger clrint, ColorModel.Value clrModel, String name) {
    return new Color(ilwisobjectsJNI.ColorRange_toColor__SWIG_0(clrint, clrModel.swigValue(), name), true);
  }

  public static Color toColor(java.math.BigInteger clrint, ColorModel.Value clrModel) {
    return new Color(ilwisobjectsJNI.ColorRange_toColor__SWIG_1(clrint, clrModel.swigValue()), true);
  }

  public static Color toColor(vectord v, ColorModel.Value colortype, String name) {
    return new Color(ilwisobjectsJNI.ColorRange_toColor__SWIG_2(vectord.getCPtr(v), v, colortype.swigValue(), name), true);
  }

  public static Color toColor(vectord v, ColorModel.Value colortype) {
    return new Color(ilwisobjectsJNI.ColorRange_toColor__SWIG_3(vectord.getCPtr(v), v, colortype.swigValue()), true);
  }

  public static Color toColor(String v, ColorModel.Value colortype, String name) {
    return new Color(ilwisobjectsJNI.ColorRange_toColor__SWIG_4(v, colortype.swigValue(), name), true);
  }

  public static Color toColor(String v, ColorModel.Value colortype) {
    return new Color(ilwisobjectsJNI.ColorRange_toColor__SWIG_5(v, colortype.swigValue()), true);
  }

}
