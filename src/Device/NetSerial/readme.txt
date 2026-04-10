=== Project ===

== Description ==

== Frequently Asked Questions ==

	= install =
		在用户$(HOME)目录中解压本工程和libs
		
		makefile中使用的库位置采用$(HOME)变量，因此 libs 请解压到$(HOME)中， 如改到其他位置，请自行修改makefile
	= build =
		编译系列server
			make -f Makefile 
		编译client
			make -f Makefile.Client

= What about? =

	Answer to foo bar dilemma.

== Changelog ==

	= 1.0 =
	* A change since the previous version.
	* Another change.
	
	= 1.01 =
	* 2012-12-02 14:53:26
	* 增加start的参数传入并生效.
	* Another change.

== FileList ==
	GeneralAgent.cpp
	GeneralAgentDef.h
	GeneralAgent.h
	GeneralAgentHandler.cpp
	GeneralAgentHandler.h
	GeneralAgentHttpKeepConnector.cpp
	GeneralAgentHttpKeepConnector.h
	GeneralAgentTcpSocketConnector.cpp
	GeneralAgentTcpSocketConnector.h
	GeneralAgentTcpSocket.cpp
	GeneralAgentTcpSocket.h
	GeneralAgentTcpSocketServer.cpp
	GeneralAgentTcpSocketServer.h
	GeneralAgentUdpSocket.cpp
	GeneralAgentUdpSocket.h
	GeneralConsoleOverTcp.cpp
	GeneralConsoleOverTcp.h
	GeneralSocketProcessor.cpp
	GeneralSocketProcessor.h
	main.cpp
	main_multi.cpp
	Makefile
	Makefile.Defines.linux-arm
	Makefile.Defines.linux-hi3515
	Makefile.Defines.linux-x86-32
	Makefile.version
	SocketMsgQue.cpp
	SocketMsgQue.h
	SocketStatusAgent.cpp
	SocketStatusAgent.h

