package util;
import java.io.*;
import java.util.*;

public class SysUtil {
	
	public static void transferFile(String orig, String newFile) {
		List<String> lines = readFile(orig);
		String content = "";
		for(String line : lines) {
			content += line + "\n";
		}
		writeFile(newFile, content);
	}
	
	public static List<String> readFile(String file) {
		List<String> rtn = new ArrayList<String>();
		try {
			BufferedReader r = new BufferedReader(new FileReader(file));
			String line="";
			while((line=r.readLine())!=null) {
				rtn.add(line);
			}
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		
		return rtn;
	}
	
	public static void writeFile(String file, String content) {
		try {
			PrintWriter out = new PrintWriter(new FileWriter(file));
			out.print(content);
			out.close();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	public static String callSysCmd(String cmd) {
		StringBuffer rtn = new StringBuffer();	
		String s="";
		try {
			Process p = Runtime.getRuntime().exec(cmd);	            
			BufferedReader stdInput = new BufferedReader(new InputStreamReader(p.getInputStream()));
			BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getErrorStream()));

	        // read the output from the command
	        System.out.println("Here is the standard output of the command:\n");
	        while ((s = stdInput.readLine()) != null) {
	            rtn.append(s + "\n");
	        }
	        
	        // read any errors from the attempted command
	        System.out.println("Here is the standard error of the command (if any):\n");
	        while ((s = stdError.readLine()) != null) {
	        	rtn.append(s + "\n");
	        }	            
        }
        catch (IOException e) {
            e.printStackTrace();
        }
        
        return rtn.toString();
	}
}
