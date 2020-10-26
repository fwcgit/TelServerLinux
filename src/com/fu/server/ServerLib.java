package com.fu.server;
import java.io.File;
import java.net.URL;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;
public class ServerLib{
	public static final long CHECK_HEARTBEAT_TIME = 1000 * 15;
	private Thread heartbeatCheckThread,authCheckThread;
	private boolean isRun = false;
	private boolean isInit = false;
	private byte[] lock = new byte[0]; // 特殊的instance变量    	
	
	private Map<String,Client> authClient = new LinkedHashMap<String,Client>(); 
	private Map<Integer,Client> allClient = new LinkedHashMap<Integer,Client>(); 
	
	private Runnable heartbeatRunnable = new Runnable() {
		
		@Override
		public void run() {
			while(isRun) {
				synchronized (lock) {
					Set<String> set = authClient.keySet();
					Iterator<String> iterator = set.iterator();
					while(iterator.hasNext()) {
						String key = iterator.next();
						Client client = authClient.get(key);
						if(null != client) {
							System.out.println("heartbeat:"+client.heartbeatCnt);
							if(client.heartbeatCnt > 3) {
								System.out.println(client.key+"offline");
								closeClient(client.fd);
								allClient.remove(client.fd);
								authClient.remove(key);
								updateFdsArray();
							}else {
								client.heartbeatCnt++;
							}
						}
					}
				}
				try {
					Thread.sleep(1000 * 15);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	};
	
	private  Runnable authRunnable = new Runnable() {
		
		@Override
		public void run() {
			while(isRun) {
				synchronized (lock) {
					Set<Integer> set = allClient.keySet();
					Iterator<Integer> iterator = set.iterator();
					try {
						while(iterator.hasNext()) {
							int fd = iterator.next();
							Client client = allClient.get(fd);
							if(null != client && !client.isAuth) {
								if(System.currentTimeMillis() - client.ctime >= (1000 * 10)) {
									closeClient(fd);
									allClient.remove(fd);
									updateFdsArray();
									System.out.println("auth timeout:"+client.fd);
								}
							}	
						}
					} catch (Exception e) {
						// TODO: handle exception
					}
				}
				try {
					Thread.sleep(1000 * 10);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	};

	static{
		//System.loadLibrary("server")
		
		URL url =  ServerLib.class.getResource("/");
		String path = url.getPath();
		System.out.println(path+"----");
		File file = new File(path+"libserver.so");
		System.out.println(file.getAbsolutePath()+"+++");
		System.load(file.getAbsolutePath());
	}

	public void init(int port) {
		if(isInit) return;
		isInit = true;
		isRun = true;
		heartbeatCheckThread = new Thread(heartbeatRunnable);
		heartbeatCheckThread.start();
		authCheckThread = new Thread(authRunnable);
		authCheckThread.start();
		starpServer(port);
	}
	

	public native void starpServer(int port);

	public native void closeServer();

	public native long sendData(int fd,byte[] data);

	public native long sendCmd(int fd,byte cmd);

	public native void closeClient(int fd);

	public native void refreshFds(int[] fds,int len);
	
	public void removeAllClient(int fd) {
	}
	
	public void clientDisConnect(int fd) {
		synchronized (lock) {
			Client client = allClient.get(fd);
			if(null != client) {
				if(null != client.key && client.key != "") {
					authClient.remove(client.key);
				}
				allClient.remove(fd);
				updateFdsArray();
			}
		}
	}
	
	public void updateFdsArray() {
		synchronized (lock) {
			Set<Integer> set = allClient.keySet();
			Iterator<Integer> iterator = set.iterator();
			System.out.println("------------------------");
			System.out.println("allClient.size():"+set.size());
			System.out.println("allsize:"+set.size());
			System.out.println("------------------------");
			if(set.size() > 0) {
				int fds[] = new int[set.size()];
				int i = 0;
				while(iterator.hasNext()) {
					fds[i++] = iterator.next();
				}
				refreshFds(fds, fds.length);
			}else
			{
				refreshFds(new int[] {}, 0);
			}
		}
	}
	
	public void newUserConnect(int fd) {
		synchronized (lock) {
			System.out.println("newUserConnect:"+fd);
			if(allClient.containsKey(fd)) {
				closeClient(fd);
				allClient.remove(fd);
			}
			Client client = new Client();
			client.fd = fd;
			client.isAuth = false;
			client.ctime = System.currentTimeMillis();
			allClient.put(fd, client);
			
			updateFdsArray();
		}
	}

	public void acceptNewUser(int fd,String key) {
		synchronized (lock) {
			System.out.println("-----------------acceptNewUser:"+fd+" key:"+key+"-----");
			Client client = null;
			if(authClient.containsKey(key)) {
				System.out.println("-----------------containsKey:"+fd+" key:"+key);
				client = authClient.get(key);
				System.out.println("-----------client---fd:"+client.fd+"  key:"+client.key);
				closeClient(client.fd);
				allClient.remove(client.fd);
				authClient.remove(key);
				updateFdsArray();
			}else {
				client = new Client();
				client.isAuth = true;
				client.key = key;
				client.fd = fd;
				client.heartbeatCnt = 0;
				authClient.put(key, client);
				
				Client fdClient = allClient.get(fd);
				if(null != fdClient) {
					fdClient.isAuth = true;
					fdClient.key = key;
				}
			}
		}

	}

	public void userHeartbeat(int fd,String key) {
		synchronized (lock) {
			System.out.println("userHeartbeat:"+fd+" key:"+key);
			Client client = authClient.get(key);
			if(null != client) {
				System.out.println(client.heartbeatCnt+":"+client.key + "client userHeartbeat:"+fd+" key:"+key);
				client.heartbeatCnt = 0;
			}
		}
	}

	public void receUserData(byte[] data){
		
	}
	
	public  void LOG(int type,String txt)
	{
		System.out.println("log->type:"+type+"----txt"+txt);
	}
}
