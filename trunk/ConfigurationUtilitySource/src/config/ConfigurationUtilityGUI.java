package config;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

import javax.swing.*;
import javax.swing.border.TitledBorder;
import javax.swing.table.DefaultTableModel;

import util.Serializer;
import util.StringBuffer2;
import util.SysUtil;

import java.util.*;
import java.util.*;

public class ConfigurationUtilityGUI implements ActionListener {
	
	private GUI gui;
	private java.util.List<RobotConfiguration> configs;
	private int currentDCOPID;
	
	public static void main(String[] args) {
		
		//check configuration
		//if first time load, default settings.
		CONFIG_CH config = Serializer.load();
		if(config==null) {
			config = new CONFIG_CH_DEFAULT();
			Serializer.save(config);
		}
		
		//create and show gui
		ConfigurationUtilityGUI c = new ConfigurationUtilityGUI();
		GUI g = new GUI(c);
		c.setGui(g);
		g.setVisible(true);
	}
	
	public ConfigurationUtilityGUI() {
		configs = new ArrayList<RobotConfiguration>();
		currentDCOPID = 0;
	}
	
	public String validateDCOPSettings() {
		
		//get settings
		String algo = (String) gui.getAlgoBox().getSelectedItem();
		String numRoundsStr = gui.getNumRoundsField().getText();
		String maxSignalStr = gui.getMaxSignalStrField().getText();
		String avgSignalStr = gui.getAvgSignalStrengthField().getText();
		String iParamStr = gui.getIParamField().getText();
		String movementStr = gui.getMovementField().getText();
		
		//valdidate
		StringBuffer2 errors = new StringBuffer2();
		try {
			Integer.parseInt(numRoundsStr);
		}
		catch(Exception e) {
			errors.appendN("Number of rounds must be a valid integer.");
		}
		try {
			Double.parseDouble(maxSignalStr);
		}
		catch(Exception e) {
			errors.appendN("Max Signal Strength must be a valid double.");
		}
		try {
			Double.parseDouble(avgSignalStr);
		}
		catch(Exception e) {
			errors.appendN("Average Signal Strength must be a valid double.");
		}
		try {
			Double.parseDouble(iParamStr);
		}
		catch(Exception e) {
			errors.appendN("I Parameter must be a valid double.");
		}
		try {
			Double.parseDouble(movementStr);
		}
		catch(Exception e) {
			errors.appendN("Movement Amount must be a valid double.");
		}
		
		return errors.toString();
	}
			
	public DefaultTableModel createModel() {
		
		//column names
		String[] columnNames = {"Robot ID", "IP Address", "MAC Address", "Connections"};
		
		//objects
		Object[][] data = new Object[configs.size()][4];
		for(int x=0; x < data.length; x++) {
			RobotConfiguration config = configs.get(x);
			data[x][0] = config.getDcopID();
			data[x][1] = config.getIpAddr();
			data[x][2] = config.getHwAddr();
			data[x][3] = config.displayConnStr();
		}
	    DefaultTableModel dataModel = new DefaultTableModel(data,columnNames); 
	    return dataModel;
	}

	public void actionPerformed(ActionEvent e) {
		Object o =e.getSource();
		if(o==gui.getAddRobotButton()) {
			addRobotPressed();
		}
		if(o==gui.getAddNetworkConnButton()) {
			addLinkPressed();
		}
		if(o==gui.getChainGraphButton()) {
			makeChainPressed();
		}
		if(o==gui.getFullGraphButton()) {
			makeFullPressed();
		}
		if(o==gui.getRemoveRobotButton()) {
			removeRobotPressed();
		}
		if(o==gui.getRemoveLinkButton()) {
			removeLinkPressed();
		}
		if(o==gui.getSaveConfigButton()) {
			saveConfigPressed();
		}
		if(o==gui.getReloadConfigButton()) {
			reloadConfigPressed(false);
		}
		if(o==gui.getDefaultConfigButton()) {
			reloadConfigPressed(true);
		}
	}
	
	public void updateView() {
		
		//Control button enablement
		gui.getAddNetworkConnButton().setEnabled(true);
		gui.getChainGraphButton().setEnabled(true);
		gui.getFullGraphButton().setEnabled(true);
		gui.getRemoveRobotButton().setEnabled(true);
		gui.getRemoveLinkButton().setEnabled(true);
		if(configs.size() < 2) {
			gui.getAddNetworkConnButton().setEnabled(false);
			gui.getChainGraphButton().setEnabled(false);
			gui.getFullGraphButton().setEnabled(false);
			gui.getRemoveLinkButton().setEnabled(false);
		}
		if(configs.size() < 1) {
			gui.getRemoveRobotButton().setEnabled(false);
		}
		
		//populate robot combo boxes
			
		//create model
		DefaultComboBoxModel aModel = new DefaultComboBoxModel();
		DefaultComboBoxModel aModel1 = new DefaultComboBoxModel();
		DefaultComboBoxModel aModel2 = new DefaultComboBoxModel();

		for(RobotConfiguration r : configs) {
			aModel.addElement(r.getDcopID());
			aModel1.addElement(r.getDcopID());
			aModel2.addElement(r.getDcopID());
		}
			
		//populate
		gui.getRobot1ID().setModel(aModel);
		gui.getRobot2ID().setModel(aModel1);
		gui.getRemoveRobotBox().setModel(aModel2);
		
		//disable remove robot button if necessary			
		if(aModel.getSize()==0) {
			gui.getRemoveRobotButton().setEnabled(false);
		}
		
		//populate JTable
		gui.getRobotTable().setModel(createModel());
		
		//populate remove link box
			
		//build model
		DefaultComboBoxModel aModel11 = new DefaultComboBoxModel();
		boolean[][] cTable = new boolean[currentDCOPID][currentDCOPID];
		for(RobotConfiguration rc : configs) {	
			int id = rc.getDcopID();
			for(int other : rc.getRobotConnections()) {
					
				//ignore if same
				if(id==other)
					continue;
				
				//not already contained
				if(!cTable[id][other]) {
					aModel11.addElement(id + "--" + other);
					cTable[id][other] = true;
					cTable[other][id] = true;
				}
			}			
		}
			
		//populate box
		gui.getRemoveLinkBox().setModel(aModel11);

		//set remove link button
		if(aModel11.getSize()==0) {
			gui.getRemoveLinkButton().setEnabled(false);
		}
		
		//populate algo box
		if(gui.getAlgoBoxModel()==null) {
			gui.setAlgoBoxModel(gui.createAlgoBoxModel());
			gui.getAlgoBox().setModel(gui.getAlgoBoxModel());
		}
		
		//populate movement driver box
		if(gui.getMovementDriverBoxModel()==null) {
			gui.setMovementDriverBoxModel(gui.createMovementDriverBoxModel());
			gui.getMovementDriverBox().setModel(gui.getMovementDriverBoxModel());
		}
	}
	
	public void reloadConfigPressed(boolean defaultSetting) {
		
		//load
		CONFIG_CH c=null;
		if(defaultSetting)
			c = new CONFIG_CH_DEFAULT();
		else
			c = Serializer.load();
		
		//update gui
		if(c!=null) {			
			
			//update configs
			configs = c.getConfigs();
			
			//find max index and set
			int maxIndex=-1;
			for(RobotConfiguration rc : configs) {
				if(rc.getDcopID() > maxIndex)
					maxIndex = rc.getDcopID();
			}
			this.setCurrentDCOPID(maxIndex+1);
			
			//upload DCOP settings
			gui.getAlgoBox().setSelectedItem(c.getDcopAlgorithm());
			gui.getNumRoundsField().setText(""+c.getNumRounds());
			gui.getMaxSignalStrField().setText(""+c.getMaxSignalStrength());
			gui.getAvgSignalStrengthField().setText(""+c.getAvgSignalStrength());
			gui.getIParamField().setText(""+c.getiParam());
			gui.getMovementField().setText(""+c.getMovementWaveLength());			
		}
		
		//update view
		updateView();
		
		//update view for driver settings
		if(c!=null) {
			if(c.getMovementDriverLoc()==null) {
				gui.getMovementDriverBox().setSelectedIndex(0);
			}
			else {
				gui.getMovementDriverBox().setSelectedIndex(1);
				gui.getPlayerDriverField().setText(c.getMovementDriverLoc());				
			}
		}
	}
	
	public void saveConfigPressed() {
		String errors = this.validateDCOPSettings();
	
		if(errors.toString().length()!=0) {
			JOptionPane.showMessageDialog(null, errors.toString(), "DCOP Settings Error", JOptionPane.ERROR_MESSAGE);
			return;
		}
		else {
			
			//extract params
			String algo = (String) gui.getAlgoBox().getSelectedItem();
			String numRoundsStr = gui.getNumRoundsField().getText();
			String maxSignalStr = gui.getMaxSignalStrField().getText();
			String avgSignalStr = gui.getAvgSignalStrengthField().getText();
			String iParamStr = gui.getIParamField().getText();
			String movementStr = gui.getMovementField().getText();
			int numRounds = Integer.parseInt(numRoundsStr);
			double maxSignal = Double.parseDouble(maxSignalStr);
			double avgSignal = Double.parseDouble(avgSignalStr);
			double iParam = Double.parseDouble(iParamStr);
			double movement = Double.parseDouble(movementStr);
			int numAgents = currentDCOPID;
			String movementDriverStr = (String) gui.getMovementDriverBox().getSelectedItem();
			String movementDriverLoc = null;
			if(movementDriverStr.indexOf("Player") > -1) {
				movementDriverLoc = gui.getPlayerDriverField().getText();
			}

			//create CONFIG_H
			CONFIG_CH c = new CONFIG_CH();
			c.setNumAgents(numAgents);
			c.setDcopAlgorithm(algo);
			c.setNumRounds(numRounds);
			c.setMaxSignalStrength(maxSignal);
			c.setAvgSignalStrength(avgSignal);
			c.setiParam(iParam);
			c.setMovementWaveLength(movement);
			c.setConfigs(configs);
			c.setMovementDriverLoc(movementDriverLoc);
			
			//write file
			c.writeHFile();
			c.writeCFile();
			
			//copy movement files to directory
			if(c.getMovementDriverLoc()==null) {
				SysUtil.transferFile("movement_drivers/dummy/create_lib.h", "create_lib.h");
				SysUtil.transferFile("movement_drivers/dummy/create_lib.c", "create_lib.c");
			}
			else {
				SysUtil.transferFile("movement_drivers/player/create_lib.h", "create_lib.h");
				SysUtil.transferFile("movement_drivers/player/create_lib.c", "create_lib.c");				
			}
			
			//serialize configuration
			Serializer.save(c);
			
			//call make
			SysUtil.callSysCmd("make clean");
			SysUtil.callSysCmd("make");
			
			//see if dcop executable exists
			File f = new File("dcop");
			if(f.exists()) {			
				//user notif of success
				JOptionPane.showMessageDialog(null, "Configuration Successful", "Success", JOptionPane.INFORMATION_MESSAGE);
			}
			else {
				//build failed
				JOptionPane.showMessageDialog(null, "Configuration Failed. Some Compile errors occured. Try \"make\" manually.", "Failure", JOptionPane.ERROR_MESSAGE);
			}
		}
	}
		
	public void removeLinkPressed() {
		String linkStr = (String) gui.getRemoveLinkBox().getSelectedItem();
		
		//get robots
		String[] tok = linkStr.split("--");
		int rid1 = Integer.parseInt(tok[0]);
		int rid2 = Integer.parseInt(tok[1]);
		RobotConfiguration rc1 = null;
		RobotConfiguration rc2 = null;
		for(RobotConfiguration rc : configs) {
			if(rc.getDcopID()==rid1)
				rc1 = rc;
			if(rc.getDcopID()==rid2)
				rc2 = rc;
		}
		
		//update model
		rc1.removeConnection(rc2.getDcopID());
		rc2.removeConnection(rc1.getDcopID());
		
		//update view
		updateView();
		
	}
	
	public void removeRobotPressed() {
		int rid = (Integer) gui.getRemoveRobotBox().getSelectedItem();
		
		//find and destroy
		RobotConfiguration toRemove = null;
		for(RobotConfiguration rc : configs) {
			if(rc.getDcopID()==rid)
				toRemove = rc;
		}
		configs.remove(toRemove);
			
		//kill all links to robot
		for(RobotConfiguration rc : configs) {
			rc.removeConnection(rid);
		}
			
		//update view
		updateView();
	}
	
	public void makeChainPressed() {
		
		//extract chain nodes string
		String nodesStr = gui.getChainNodesField().getText();
		
		//validate
		StringBuffer2 errors = new StringBuffer2();
		String[] toks = nodesStr.split(",");
		java.util.List<Integer> chain = new ArrayList<Integer>();
		errorcheck:for(String tok : toks) {
			try {
				int i = Integer.parseInt(tok);
				
				boolean found=false;
				for(RobotConfiguration r : configs) {
					if(r.getDcopID()==i)
						found = true;
				}
				
				if(!found) {
					errors.appendN("Entry must contain only valid comma delimited robot ids.");
					break errorcheck;
				}
				else {
					chain.add(i);
				}				
			}
			catch(Exception e) {
				errors.appendN("Error parsing chain nodes. Must contain only valid comma delimited robot ids.");
				break errorcheck;
			}
		}
		if(chain.size() < 2) {
			errors.appendN("Entry must contain atleast 2 robot ids.");
			
		}
		if(containsLoops(chain)) {
			errors.appendN("Chain cannot contain loops.");
		}
		if(errors.toString().length()!=0) {
			JOptionPane.showMessageDialog(null, errors.toString(), "Invalid Entry", JOptionPane.ERROR_MESSAGE);
			return;
		}
		
		//make chain
		for(int x=1; x < chain.size(); x++) {
			int id = chain.get(x);
			int prevID = chain.get(x-1);			
			RobotConfiguration current = null;
			RobotConfiguration rcPrev = null;
			for(RobotConfiguration rc : configs) {
				if(rc.getDcopID()==id)
					current = rc;
				if(rc.getDcopID()==prevID)
					rcPrev = rc;
			}
			current.addConnection(rcPrev.getDcopID());
			rcPrev.addConnection(current.getDcopID());			
		}
		
		//update view
		updateView();
	}
	
	public void makeFullPressed() {
		
		//make graph
		for(RobotConfiguration r1 : configs) {
			loop:for(RobotConfiguration r2 : configs) {
				if(r1.getDcopID()==r2.getDcopID())
					continue loop;
				
				//add connection
				r1.addConnection(r2.getDcopID());
				r2.addConnection(r1.getDcopID());
			}
		}
		
		//update view
		updateView();
		
	}
	
	public void addLinkPressed() {
		
		//get fields
		String r1 = "" + gui.getRobot1ID().getSelectedItem();
		String r2 = "" + gui.getRobot2ID().getSelectedItem();
		int r1Int  = (Integer) gui.getRobot1ID().getSelectedItem();
		int r2Int = (Integer) gui.getRobot2ID().getSelectedItem();
		
		//validate
		StringBuffer2 errors = new StringBuffer2();
		if(r1.equalsIgnoreCase(r2)) {
			errors.appendN("Robot shouldn't be linked to itself!");			
		}
		if(errors.toString().length()!=0) {
			JOptionPane.showMessageDialog(null, errors.toString(), "Invalid Entry", JOptionPane.ERROR_MESSAGE);
			return;
		}
		
		//add necessary connections
		RobotConfiguration r1c = null;
		RobotConfiguration r2c = null;
		for(RobotConfiguration rc : configs) {
			if(rc.getDcopID()==r1Int)
				r1c = rc;
			if(rc.getDcopID()==r2Int)
				r2c = rc;
		}
		r1c.addConnection(r2c.getDcopID());
		r2c.addConnection(r1c.getDcopID());
		
		//update view
		updateView();		
	}
	
	public boolean ipOkay(String ip) {
		boolean rtn = !ip.trim().equals("");
		
		//ips only contain numbers and .
		for(char c : ip.toCharArray()) {
			if(c >= '0' && c <= '9')
				continue;
			if(c == '.')
				continue;

			rtn  = false;
		}
		
		return rtn;
	}
	
	public boolean hwOkay(String hw) {
		boolean rtn = !hw.trim().equals("");
		
		//hw addr only contains numbers, alphabet and :
		for(char c : hw.toCharArray()) {
			if (c >= 'a' && c <= 'z')
				continue;
			if(c >= 'A' && c <= 'Z')
				continue;
			if(c >= '0' && c <= '9')
				continue;
			if(c==':')
				continue;
			
			rtn = false;
		}
		
		return rtn;
	}
	
	public void addRobotPressed() {
		
		//get fields
		String ip = gui.getIpAddrField().getText().trim();
		String hwAddr = gui.getHwAddrField().getText().trim();
		
		//check validity and process
		StringBuffer2 errors = new StringBuffer2();
		if(!ipOkay(ip)) {
			errors.appendN("IP Address is invalid.");
		}
		if(!hwOkay(hwAddr)) {
			errors.appendN("MAC Address is invalid.");
		}
		if(errors.toString().length()!=0) {
			JOptionPane.showMessageDialog(null, errors.toString(), "Invalid Entry", JOptionPane.ERROR_MESSAGE);
			return;
		}
		
		//create configuration object
		RobotConfiguration rc = new RobotConfiguration(currentDCOPID, ip, hwAddr);
		currentDCOPID++;
		
		//add to list
		configs.add(rc);
		
		//update view
		updateView();
	}
	
	public boolean containsLoops(java.util.List<Integer> chain) {
		for(int x=0; x < chain.size(); x++) {
			for(int y=0; y < chain.size(); y++) {
				if(x==y)
					continue;
				int val1 = chain.get(x);
				int val2 = chain.get(y);
				
				if(val1==val2)
					return true;
			}
		}
		return false;
	}
	
	/**
	 * @return the gui
	 */
	public GUI getGui() {
		return gui;
	}

	/**
	 * @param gui the gui to set
	 */
	public void setGui(GUI gui) {
		this.gui = gui;
		reloadConfigPressed(false);
	}

	/**
	 * @return the configs
	 */
	public java.util.List<RobotConfiguration> getConfigs() {
		return configs;
	}

	/**
	 * @param configs the configs to set
	 */
	public void setConfigs(java.util.List<RobotConfiguration> configs) {
		this.configs = configs;
	}

	/**
	 * @return the currentDCOPID
	 */
	public int getCurrentDCOPID() {
		return currentDCOPID;
	}

	/**
	 * @param currentDCOPID the currentDCOPID to set
	 */
	public void setCurrentDCOPID(int currentDCOPID) {
		this.currentDCOPID = currentDCOPID;
	}	
}
