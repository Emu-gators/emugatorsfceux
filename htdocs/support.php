<?php
	$title = "Support Files";
	include("header.php");
?>

<div class="mainbox">
<h2 class="boxhead">Support Files</h2>
<div class="boxbody">

<ul>
<li><a href="#gfceu">GNOME FCE Ultra</a></li>
<li><a href="#chtconv">NESten&lt;--&gt;FCE Ultra Cheat Converter</a></li>
<li><a href="#netplay">Network Play Server</a></li>

</ul>
</div>
</div>

<div class="mainbox">
<h2 class="boxhead"><a id="gfceu">GNOME FCE Ultra</a></h2>
<div class="boxbody">
<p>
GFCEU is a GTK2 front-end designed to provide a simple, yet powerful interface for FCE Ultra. 
It currently provides an elegant interface for:<br>
<ul>
<li>Sound configuration</li>
<li>Video configuration</li>
<li>Input configuration</li>
<li>Easy network play</li>
</ul>
For more information about gfceu and for download links, click <a href="http://dietschnitzel.com/gfceu/">here</a>.
</p>

</div>
</div>


<div class="mainbox">
<h2 class="boxhead"><a id="chtconv">NESten&lt;--&gt;FCE Ultra Cheat Converter</a></h2>
<div class="boxbody">
<p>
This is a simple cheat converter for converting between NESten and FCE Ultra cheat files.
Only RAM-style(as opposed to Game-Genie) cheats are converted.  Thanks go to Death Adder
for writing the simple documentation included.  DOS executable, with sources included.
</p>

<p class="center"><a href="chtconv.zip">Download</a></p>
</div>
</div>

<div class="mainbox">
<h2 class="boxhead"><a id="netplay">Network Play Server</a></h2>

<div class="boxbody">
<p>
This is the network play server to be used with FCE Ultra 0.98.10 and later.  It is
beta quality, but it should work well, though it probably doesn't scale to
a large number of users as well as it could.
</p>

<p>
It was developed under Linux, but it
should compile and work on any platform that implements the BSD-sockets API.
</p>

<p>
Use <a href="http://www.cygwin.com/">Cygwin</a> to compile the server under MS Windows.
</p>

<p class="center">
Current Release:<br>
<a href="http://prdownloads.sourceforge.net/fceultra/fceu-server-0.0.5.tar.gz?download">0.0.5 Source Code</a><br><br>
Legacy Releases:<br>
<a href="http://prdownloads.sourceforge.net/fceultra/fceu-server-0.0.4.tar.gz?download">0.0.4 Source Code</a><br>
<a href="http://prdownloads.sourceforge.net/fceultra/fceunetserver-0.0.3.tar.bz2">0.0.3 Source Code</a>
</p>
</div>
</div>

<?php include("footer.php"); ?>
