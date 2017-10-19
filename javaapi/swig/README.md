<h1>WPS-Ilwis Bridge</h1>
Wiki page: https://wiki.52north.org/bin/view/Projects/GSoC2015WpsIlwisBridge <br>
Introductory blogpost: http://blog.52north.org/2015/05/27/wps-ilwis-bridge/ <br>
Midterm blogpost: http://blog.52north.org/2015/07/03/wps-ilwis-bridge-midterm-post/ <br>

<h2>JavaAPI</h2>
<b>Will be moved to IlwisConnectors.</b><br>
Qt project, generates a Java API for <a href="https://github.com/52North/IlwisCore">Ilwis-Objects</a> with <a href="http://www.swig.org/">SWIG</a>.
<h3>Requirements</h3>
<li>Qt 5.2.1 with MinGW 4.8.2 32bit</li>
<li>SWIG 3.0.6</li>
<li>Java 1.8 <a href=http://www.webupd8.org/2014/03/how-to-install-oracle-java-8-in-debian.html>Linux</a></li>
<h3>Build</h3>
<li>Set <b>JAVADIR</b> in javaapi.pro to JDK location.
<li>Set Qt <b>Projects / Build / Build Steps</b> to run SWIG:
<a href="https://drive.google.com/open?id=0B0bWmJJYoWIpdjJsejZLVmthVTg">Windows</a> <a href="">Linux</a></li>
SWIG build parameters: <code>-java -c++ -package org.n52.ilwis.java -outdir ../../java/src/main/java/org/n52/ilwis/java ilwisobjects.i</code><br>
<b>Output:</b>
<li>Java files in the swig outdir <i>(java/src/main/java/org/n52/ilwis/java)</i>.
<li>_ilwisobjects.dll / lib_ilwisobjects.so in the <i>output/extensions/_ilwisobjects</i> folder.

<h1>Contact</h1>
Ilwis mailing list: ilwis@52north.org

<h1>License</h1>
This project is published under The Apache Software License, Version 2.0.

