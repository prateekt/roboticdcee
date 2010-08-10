package config;
import java.util.*;
import java.io.*;

public class RobotConfiguration implements Serializable {
	
	/**
	 * DCOP ID of robot
	 */
	private int dcopID;
	
	/**
	 * IP Address of robot
	 */
	private String ipAddr;
	
	/**
	 * Mac Address of robot's laptop
	 */
	private String hwAddr;
	
	/**
	 * List of ids of connected robots to this one.
	 */
	private List<Integer> robotConnections;
	
	/**
	 * Constructor
	 * @param dcopID The id of the robot
	 * @param ipAddr The ip address of the robot's laptop
	 * @param hwAddr The hw address of the robot's laptop
	 */
	public RobotConfiguration(int dcopID, String ipAddr, String hwAddr) {
		this.dcopID = dcopID;
		this.ipAddr = ipAddr;
		this.hwAddr =hwAddr;
		robotConnections = new ArrayList<Integer>();
	}
	
	/**
	 * Adds a connection id the robot
	 * @param connection The connecting robot
	 */
	public void addConnection(int connection) {
		if(!robotConnections.contains(connection)) {
			robotConnections.add(connection);
		}
	}
	
	public void removeConnection(Object connection) {
		if(robotConnections.contains(connection)) {
			robotConnections.remove((Object)connection);
		}
	}
	
	public String displayConnStr() {
		String rtn= "";
		for(int conn : robotConnections) {
			rtn+= conn + " ";
		}
		
		return rtn.trim();				
	}
	
	/**
	 * String to write to file for connection strings.
	 * @return
	 */
	public String fileConnStr() {
		String rtn= dcopID + " ";
		for(int conn : robotConnections) {
			rtn+= conn + " ";
		}
		
		return rtn.trim();		
	}
	
	/**
	 * String to write to file for robot entry.
	 * @return
	 */
	public String fileEntryStr() {
		return dcopID + " " + ipAddr + " " + hwAddr;
	}

	/**
	 * @return the dcopID
	 */
	public int getDcopID() {
		return dcopID;
	}

	/**
	 * @param dcopID the dcopID to set
	 */
	public void setDcopID(int dcopID) {
		this.dcopID = dcopID;
	}

	/**
	 * @return the ipAddr
	 */
	public String getIpAddr() {
		return ipAddr;
	}

	/**
	 * @param ipAddr the ipAddr to set
	 */
	public void setIpAddr(String ipAddr) {
		this.ipAddr = ipAddr;
	}

	/**
	 * @return the hwAddr
	 */
	public String getHwAddr() {
		return hwAddr;
	}

	/**
	 * @param hwAddr the hwAddr to set
	 */
	public void setHwAddr(String hwAddr) {
		this.hwAddr = hwAddr;
	}

	/**
	 * @return the robotConnections
	 */
	public List<Integer> getRobotConnections() {
		return robotConnections;
	}

	/**
	 * @param robotConnections the robotConnections to set
	 */
	public void setRobotConnections(List<Integer> robotConnections) {
		this.robotConnections = robotConnections;
	}
}