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

public class ThematicRange extends ItemRange {
  private long swigCPtr;

  protected ThematicRange(long cPtr, boolean cMemoryOwn) {
    super(ilwisobjectsJNI.ThematicRange_SWIGUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(ThematicRange obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        ilwisobjectsJNI.delete_ThematicRange(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public ThematicRange() {
    this(ilwisobjectsJNI.new_ThematicRange(), true);
  }

  public void add(String name, String id, String descr) {
    ilwisobjectsJNI.ThematicRange_add__SWIG_0(swigCPtr, this, name, id, descr);
  }

  public void add(String name, String id) {
    ilwisobjectsJNI.ThematicRange_add__SWIG_1(swigCPtr, this, name, id);
  }

  public void add(String name) {
    ilwisobjectsJNI.ThematicRange_add__SWIG_2(swigCPtr, this, name);
  }

  public vectorvs listAll() {
    return new vectorvs(ilwisobjectsJNI.ThematicRange_listAll(swigCPtr, this), true);
  }

  public ThematicRange clone() {
    long cPtr = ilwisobjectsJNI.ThematicRange_clone(swigCPtr, this);
    return (cPtr == 0) ? null : new ThematicRange(cPtr, false);
  }

}
