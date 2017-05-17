compile command are: devenv Project4.sln /rebuild debug
please use run.bat to run my demo.

useful folder position difference between Prompt running and Visual Studio running
----------------------------------------------------------------------------------
         Visual Studio       -> Prompt 
----------------------------   ---------------------
 ServerRepository/repository -> x64/Debug/repository
         ClientRcv           -> x64/ClientRcv
         ServerRcv           -> x64/ServerRcv
           test              -> test
----------------------------------------------------
In my project, I developed a repository server and a client (with WPF),
I will introduce how to use it and shows that I meet all requirements
=============================
         Deme Req#3
=============================
ServerRepository.exe is a Repository program, it can accept packages from client and then
stores the package in "repository". During check in process, files are stored in "ServerRcv" 
and the status is open. After receiving check in donw message from client, all files will be 
moved to "repository" and the status becomes closed. Each package in "repository" have a xml
file, which stores its metadata, information about package name, time stamp, dependent packages, 
contains files. 
==============================
	 Demo Req#4
==============================
After every package check in, I use facilities developed in Project#3 to anaysis its dependency,
and then store these dependency information in package metadata (xml). During every check in process
you can find it in server console

==============================
	 Demo Req#5
==============================
Please use client WPF to check this requirement. Before check in starts, select "check in" tab, 
then click "Select Directory", choose a directory. Then select package files(a pair of .h and .cpp 
files or single .h/.cpp file). Then click "check in", files will be transfer to server, stores 
in ServerRcv (running in Visual Studio) or x64/ServerRcv (running in Prompt). But at this time, 
the status is open, you need click "check in done" button to close thischeck in process. After
then, a new directory will be created in "repository", named from package name and check in time.
package files and metadata (xml) are stored in this directory. Now, please check package you just 
checked in "repository", it will be stored inthe package and named "package name"+"time". During 
check in process, you must choose files belong toone package, but if you wrong click (misselect file) 
or just don't want to continue check in, just click "cancel check in" button to release check in 
process. Metadata has been mentioned in Demo Req#3, you can check them in every package, or you can 
check in a new package to see the new xml file. (not: please don't delete packages in repository when
program is running)
==============================
	 Demo Req#6
==============================
From former manipulate, once you begain check in a file (select a file and click button "check in"),
the check in status is open. You can replace file or cancel check in process any time. But once you
click "check in done" button, the check in process is done, and the status becomes close --- immutable
==============================
	 Demo Req#7
==============================
Extraction process should be demoed under tab "extraction". First you should click "get package list"
button to get all package name stored in repository. I provide two extraction mode, first you can download
a package or packages without dependency, that means you can select any number of packages in list and then
click "download(no dep)" button to download them. The other way is to download a package and its dependent
package or packages. At this time, you only can select one package from list and then click "download(dep)".
Download packages are all stored in ClientRcv (running in Visual Studio) or x64/ClientRcv (running in Prompt)
==============================
	 Demo Req#8
==============================
I use socket communication system to cummunicate from client to server. Every process is demo for this 
requirement.
==============================
	 Demo Req#9
==============================
You can select "Send Message" tab in WPF, and then click "Send Message" to see this HTTP style message I
used in program. I use asynchronous one-way messageing from multi-thread mechanism
==============================
	 Demo Req#10
==============================
streams are used in my project to transfer HTTP style message and files
==============================
	 Demo Req#11
==============================
I have pre-stored three packages in repository, you can check in or download package any time. I provide a 
set of test packages in "test" folder under root directory, you can check in them and compare the package 
difference in "repository" before and after check in,