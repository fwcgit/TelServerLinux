package com.fu.server;
import java.io.File;
import java.util.Scanner;

public class Main{

	private static Scanner sc;

	public static void main(String[] args){

		ServerLib sl = new ServerLib();
		sl.init(38888);
		File file = new File("./");
		System.out.println(file.getAbsolutePath());
		 sc = new Scanner(System.in);
		while(sc.hasNext()){
			String str = sc.nextLine();
			if(str.equals("q")) {
				System.exit(0);
			}
		}
	}
}

