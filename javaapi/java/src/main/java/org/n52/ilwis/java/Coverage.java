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

public class Coverage extends IlwisObject {
  private long swigCPtr;

  protected Coverage(long cPtr, boolean cMemoryOwn) {
    super(ilwisobjectsJNI.Coverage_SWIGUpcast(cPtr), cMemoryOwn);
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Coverage obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        ilwisobjectsJNI.delete_Coverage(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public CoordinateSystem coordinateSystem() {
    return new CoordinateSystem(ilwisobjectsJNI.Coverage_coordinateSystem(swigCPtr, this), true);
  }

  public Envelope envelope() {
    return new Envelope(ilwisobjectsJNI.Coverage_envelope(swigCPtr, this), true);
  }

  public void setEnvelope(Envelope env) {
    ilwisobjectsJNI.Coverage_setEnvelope(swigCPtr, this, Envelope.getCPtr(env), env);
  }

  public final static class AttributeType {
    public final static Coverage.AttributeType atCOVERAGE = new Coverage.AttributeType("atCOVERAGE");
    public final static Coverage.AttributeType atINDEX = new Coverage.AttributeType("atINDEX");

    public final int swigValue() {
      return swigValue;
    }

    public String toString() {
      return swigName;
    }

    public static AttributeType swigToEnum(int swigValue) {
      if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
        return swigValues[swigValue];
      for (int i = 0; i < swigValues.length; i++)
        if (swigValues[i].swigValue == swigValue)
          return swigValues[i];
      throw new IllegalArgumentException("No enum " + AttributeType.class + " with value " + swigValue);
    }

    private AttributeType(String swigName) {
      this.swigName = swigName;
      this.swigValue = swigNext++;
    }

    private AttributeType(String swigName, int swigValue) {
      this.swigName = swigName;
      this.swigValue = swigValue;
      swigNext = swigValue+1;
    }

    private AttributeType(String swigName, AttributeType swigEnum) {
      this.swigName = swigName;
      this.swigValue = swigEnum.swigValue;
      swigNext = this.swigValue+1;
    }

    private static AttributeType[] swigValues = { atCOVERAGE, atINDEX };
    private static int swigNext = 0;
    private final int swigValue;
    private final String swigName;
  }

}
