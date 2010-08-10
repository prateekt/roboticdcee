package config;
import java.util.*;

public class CONFIG_CH_DEFAULT extends CONFIG_CH {
	
	public CONFIG_CH_DEFAULT() {
		setNumAgents(2);
		setNumRounds(10);
		setDcopAlgorithm("MPGM1");
		setAvgSignalStrength(27);
		setMaxSignalStrength(100);
		setiParam(38);
		setMovementWaveLength(0.06);
		setMovementDriverLoc(null);
		
		//Default Agents
		List<RobotConfiguration> configs = new ArrayList<RobotConfiguration>();
		RobotConfiguration rc1 = new RobotConfiguration(0, "127.0.0.1", "57:57:57:57:6A:42");
		RobotConfiguration rc2 = new RobotConfiguration(1, "127.0.0.1", "57:57:57:57:6A:44");
		rc1.addConnection(1);
		rc2.addConnection(0);
		configs.add(rc1);
		configs.add(rc2);
		
		setConfigs(configs);		
	}
}
