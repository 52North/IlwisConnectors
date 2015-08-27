<h1>WPS-Ilwis Bridge</h1>
Wiki page: https://wiki.52north.org/bin/view/Projects/GSoC2015WpsIlwisBridge <br>
Introductory blogpost: http://blog.52north.org/2015/05/27/wps-ilwis-bridge/ <br>
Midterm blogpost: http://blog.52north.org/2015/07/03/wps-ilwis-bridge-midterm-post/ <br>

<h2>IlwisJava</h2>
Maven project.
<h3>Building the library</h3>
<li>Edit <i>ilwisjava/src/main/resources/ilwislocation.config</i> to the ilwisobjects folder.</li>
Default: <code>ilwisDir=F:/documents/ilwis/ilwisObjects/output/win32debug/bin/</code>
<li>Download the testfiles from <a href="https://docs.google.com/uc?id=0B0bWmJJYoWIpR2VuWmRYdUlFSkk&export=download">Here</a> to <i>ilwisjava/src/test/resources</i>.
<li>Use <code>mvn clean install</code> in the <i>ilwisjava</i> folder to export to jar and run the tests.<br></li>
<b>Output:</b>
<li>The jar file is in <i>ilwisjava/target</i>.</li>
<h3>Using the library</h3>
<li>Win: 32 bit Java 8 <br>
Linux: 64 bit Java 8 - tested with <a href="http://www.webupd8.org/2014/03/how-to-install-oracle-java-8-in-debian.html">this.</a> </li>
<li>Linux: libgdal (1.18.0)
<li>Ilwis location have to be in the <b>PATH variable</b>.<br>(Folder with ilwis.exe, development location: <i>D:\ilwisObjects\output\win32debug\bin;</i>).<br>
On Linux, use <b>export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/ilwisobjects-location/libraries/genericrelease</b> </li>
<li>Copy <b>ilwislocation.config</b> to the <i>projectfolder</i>.
<li>Copy 52n-ilwis-java-version-SNAPSHOT.jar to <i>projectfolder/lib</i></li>
<li>Add the jar to build path.</li>
<li>Import: <code>import org.n52.ilwis.java.*;</code></li>
<li>Before use, load Ilwis-Objects with <code>ilwisobjects.initIlwisObjects();</code><br>

<h1>Contact</h1>
Ilwis mailing list: ilwis@52north.org

<h1>License</h1>
This project is published under The Apache Software License, Version 2.0.

