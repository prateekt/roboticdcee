package util;

import java.util.*;
import java.io.*;

import config.CONFIG_CH;

public class Serializer {
	
	public static String SERIAL_FILE = "CFG.obj";
	
	public static CONFIG_CH load() {
		CONFIG_CH rtn = null;
		try {
			// Read from disk using FileInputStream
			FileInputStream f_in = new FileInputStream(SERIAL_FILE);
	
			// Read object using ObjectInputStream
			ObjectInputStream obj_in = new ObjectInputStream (f_in);
	
			// Read an object
			Object obj = obj_in.readObject();
	
			if (obj instanceof CONFIG_CH) {
				rtn= (CONFIG_CH) obj;
			}
		}
		catch(Exception e) {}
		
		return rtn;
	}
	
	public static void save(CONFIG_CH c) {
		try {
						
			// Write to disk with FileOutputStream
			FileOutputStream f_out = new FileOutputStream(SERIAL_FILE);

			// Write object with ObjectOutputStream
			ObjectOutputStream obj_out = new ObjectOutputStream (f_out);

			// Write object out to disk
			obj_out.writeObject (c);

			//close streams
			obj_out.close();
			f_out.close();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
}
