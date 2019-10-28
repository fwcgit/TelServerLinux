package com.fu.server;
import java.io.File;
import java.net.URL;
public class ServerLib{

	static{
		//System.loadLibrary("server")
		
		URL url =  ServerLib.class.getResource("/");
		String path = url.getPath();
		System.out.println(path+"----");
		File file = new File(path+"libserver.so");
		System.out.println(file.getAbsolutePath()+"+++");
		System.load(file.getAbsolutePath());
	}

	public native void starpServer(int port);

	public native void closeServer();

	public native long sendData(String session,byte[] data);

	public native long sendCmd(String session,byte cmd);

	public native void closeClient(int session);

	public native String[] getClientList();

	public void newClientConnect(String session)
	{
		System.out.println("jni_newClientConnect"+session);
	}

	public void closeClinetConnect(String  session)
	{
		System.out.println("jni_closeClinetConnect"+session);
	}

	public void receUserData(byte[] data){
		
	}
	
	public  void LOG(int type,String txt)
	{
		System.out.println("log->type:"+type+"----txt"+txt);
	}
}
