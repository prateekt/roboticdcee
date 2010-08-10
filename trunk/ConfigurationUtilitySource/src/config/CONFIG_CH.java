package config;
import java.io.*;
import java.util.*;

import util.StringBuffer2;

public class CONFIG_CH implements Serializable {
	
	private int numAgents;
	private int numRounds;
	private String dcopAlgorithm;
	private double avgSignalStrength;
	private double maxSignalStrength;
	private double iParam;
	private double movementWaveLength;
	private String movementDriverLoc;
	private List<RobotConfiguration> configs;
	
	public CONFIG_CH() {
		configs = new ArrayList<RobotConfiguration>();
		movementDriverLoc = null;
	}
	
	public void writeHFile() {
		try {
			PrintWriter w = new PrintWriter(new FileWriter("config.h"));
			w.print(toHFile());
			w.close();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}

	public void writeCFile() {
		try {
			PrintWriter w = new PrintWriter(new FileWriter("config.c"));
			w.print(toCFile());
			w.close();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}

	
	public String toCFile() {
		StringBuffer2 rtn = new StringBuffer2();
		
		//imports
		rtn.appendN("#include <vector>");
		rtn.appendN("#include \"config.h\"");
		rtn.appendN("#include \"Network.h\"");
		rtn.appendN("");
		
		//start configuration method
		rtn.appendN("//load configuration");
		rtn.appendN("void loadConfiguration() {");
		rtn.appendN("");
		
		//make maps
		rtn.appendN("//init maps");
		rtn.appendN("robotIDToIP = new map<int, string>();");
		rtn.appendN("hwAddrToID = new map<string, int>();");
		rtn.appendN("neighborMap = new map<int, vector<int>*>();");
		rtn.appendN("");
		
		//print ip map
		rtn.appendT("//load ip map", 1);
		for(int x=0; x < configs.size(); x++) {
			RobotConfiguration r = configs.get(x);
			rtn.appendT("robotIDToIP->insert(std::pair<int, string>("+r.getDcopID()+", \""+r.getIpAddr()+"\"));",1);
		}
		rtn.appendT("",1);
		
		//print hw map
		rtn.appendT("//load hardware addr map", 1);
		for(int x=0; x < configs.size(); x++) {
			RobotConfiguration r = configs.get(x);
			rtn.appendT("hwAddrToID->insert(std::pair<string,int>(\""+r.getHwAddr()+"\", "+r.getDcopID()+"));",1);			
		}
		rtn.appendT("",1);
		
		//print connections map
		for(int x=0; x < configs.size(); x++) {
			RobotConfiguration r = configs.get(x);
			List<Integer> connList = r.getRobotConnections();
			rtn.appendT("//init connections map -- robot " + r.getDcopID(), 1);
			rtn.appendT("vector<int>* neighbors" + r.getDcopID() + " = new vector<int>();", 1);
			for(int conn : connList) {					
				rtn.appendT("neighbors"+r.getDcopID()+"->push_back(" +conn+");", 1);
			}
			rtn.appendT("neighborMap->insert(std::pair<int, vector<int>*>("+r.getDcopID()+", neighbors"+r.getDcopID()+"));",1);
			rtn.appendT("",1);
		}
		
		rtn.appendN("}");
		
		return rtn.toString();
	}
	
	public String toHFile() {
		StringBuffer2 rtn = new StringBuffer2();
		rtn.appendN("#ifndef CONFIG_H");
		rtn.appendN("#define CONFIG_H");
		rtn.appendN("");
		rtn.appendN("//DCOP Constants");
		rtn.appendN("#define NUM_AGENTS " +numAgents);
		rtn.appendN("#define NUM_ROUNDS " + numRounds);
		rtn.appendN("#define DCOP_ALGORITHM \""+dcopAlgorithm+"\"");
		rtn.appendN("#define AVG_SIGNAL_STR "+avgSignalStrength);
		rtn.appendN("#define MAX_SIGNAL_STR " + maxSignalStrength);
		rtn.appendN("#define I_PARAM " + iParam);
		rtn.appendN("#define MOVEMENT_WAVE_LENGTH " + movementWaveLength);
		rtn.appendN("#define MOVEMENT_DRIVER_LOC " +"\""+ movementDriverLoc+"\"");

		rtn.appendN("");
		
		//start configuration method
		rtn.appendN("//load configuration");
		rtn.appendN("void loadConfiguration();");
		rtn.appendN("");
		rtn.appendN("#endif");		
		return rtn.toString();
	}
	
	
	/**
	 * @return the numAgents
	 */
	public int getNumAgents() {
		return numAgents;
	}

	/**
	 * @param numAgents the numAgents to set
	 */
	public void setNumAgents(int numAgents) {
		this.numAgents = numAgents;
	}

	/**
	 * @return the numRounds
	 */
	public int getNumRounds() {
		return numRounds;
	}

	/**
	 * @param numRounds the numRounds to set
	 */
	public void setNumRounds(int numRounds) {
		this.numRounds = numRounds;
	}

	/**
	 * @return the dcopAlgorithm
	 */
	public String getDcopAlgorithm() {
		return dcopAlgorithm;
	}

	/**
	 * @param dcopAlgorithm the dcopAlgorithm to set
	 */
	public void setDcopAlgorithm(String dcopAlgorithm) {
		this.dcopAlgorithm = dcopAlgorithm;
	}

	/**
	 * @return the avgSignalStrength
	 */
	public double getAvgSignalStrength() {
		return avgSignalStrength;
	}

	/**
	 * @param avgSignalStrength the avgSignalStrength to set
	 */
	public void setAvgSignalStrength(double avgSignalStrength) {
		this.avgSignalStrength = avgSignalStrength;
	}

	/**
	 * @return the maxSignalStrength
	 */
	public double getMaxSignalStrength() {
		return maxSignalStrength;
	}

	/**
	 * @param maxSignalStrength the maxSignalStrength to set
	 */
	public void setMaxSignalStrength(double maxSignalStrength) {
		this.maxSignalStrength = maxSignalStrength;
	}

	/**
	 * @return the iParam
	 */
	public double getiParam() {
		return iParam;
	}

	/**
	 * @param iParam the iParam to set
	 */
	public void setiParam(double iParam) {
		this.iParam = iParam;
	}

	/**
	 * @return the movementWaveLength
	 */
	public double getMovementWaveLength() {
		return movementWaveLength;
	}

	/**
	 * @param movementWaveLength the movementWaveLength to set
	 */
	public void setMovementWaveLength(double movementWaveLength) {
		this.movementWaveLength = movementWaveLength;
	}

	/**
	 * @return the configs
	 */
	public List<RobotConfiguration> getConfigs() {
		return configs;
	}

	/**
	 * @param configs the configs to set
	 */
	public void setConfigs(List<RobotConfiguration> configs) {
		this.configs = configs;
	}

	/**
	 * @return the movementDriverLoc
	 */
	public String getMovementDriverLoc() {
		return movementDriverLoc;
	}

	/**
	 * @param movementDriverLoc the movementDriverLoc to set
	 */
	public void setMovementDriverLoc(String movementDriverLoc) {
		this.movementDriverLoc = movementDriverLoc;
	}
}
