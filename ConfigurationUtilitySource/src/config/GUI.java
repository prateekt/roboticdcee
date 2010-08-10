package config;
import java.awt.BorderLayout;

import javax.swing.BorderFactory;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JPanel;
import javax.swing.JFrame;
import java.awt.Dimension;
import javax.swing.JTextField;
import java.awt.Rectangle;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;
import java.awt.Label;
import java.awt.GridLayout;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.border.TitledBorder;
import javax.swing.JComboBox;
import javax.swing.JScrollPane;
import javax.swing.JTable;

public class GUI extends JFrame {

	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;
	private JTextField ipAddrField = null;
	private JPanel row1Panel = null;
	private JTextField hwAddrField = null;
	private Label label1 = null;
	private Label label2 = null;
	private JButton addRobotButton = null;
	private JPanel row2Panel = null;
	private Label label3 = null;
	private Label label4 = null;
	private JButton addNetworkConnButton = null;
	private JComboBox robot1ID = null;
	private JComboBox robot2ID = null;
	private JPanel row3Panel = null;
	private Label label5 = null;
	private JButton fullGraphButton = null;
	private JButton chainGraphButton = null;
	private JTextField chainNodesField = null;
	private JScrollPane graphPane = null;
	private JTable robotTable = null;
	private JPanel row4Panel = null;
	private JComboBox removeRobotBox = null;
	private JComboBox removeLinkBox = null;
	private JButton removeRobotButton = null;
	private JButton removeLinkButton = null;
	private JPanel row5Panel = null;
	private JButton saveConfigButton = null;
	private JButton reloadConfigButton = null;
	private ActionListener l;
	private JPanel row5 = null;
	private JComboBox algoBox = null;
	private Label label6 = null;
	private Label label7 = null;
	private JTextField numRoundsField = null;
	private Label label8 = null;
	private JTextField maxSignalStrField = null;
	private Label label9 = null;
	private JTextField avgSignalStrengthField = null;
	private Label label10 = null;
	private JTextField iParamField = null;
	private Label label11 = null;
	private JTextField movementField = null;
	private JButton defaultConfigButton = null;
	private JComboBox movementDriverBox = null;
	private JTextField playerDriverField = null;
	private DefaultComboBoxModel algoBoxModel = null;
	private DefaultComboBoxModel movementDriverBoxModel = null;
	
	/**
	 * @return the algoBoxModel
	 */
	public DefaultComboBoxModel getAlgoBoxModel() {
		return algoBoxModel;
	}

	/**
	 * @param algoBoxModel the algoBoxModel to set
	 */
	public void setAlgoBoxModel(DefaultComboBoxModel algoBoxModel) {
		this.algoBoxModel = algoBoxModel;
	}

	/**
	 * @param movementDriverBoxModel the movementDriverBoxModel to set
	 */
	public void setMovementDriverBoxModel(
			DefaultComboBoxModel movementDriverBoxModel) {
		this.movementDriverBoxModel = movementDriverBoxModel;
	}

	/**
	 * @return the movementDriverBoxModel
	 */
	public DefaultComboBoxModel getMovementDriverBoxModel() {
		return movementDriverBoxModel;
	}

	/**
	 * This is the default constructor
	 */
	public GUI(ActionListener l) {
		super();
		this.l = l;
		initialize();
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	}

	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(514, 725);
		this.setContentPane(getJContentPane());
		this.setTitle("DCEE Configuration Utility");
	}
	
	public DefaultComboBoxModel createAlgoBoxModel() {
		DefaultComboBoxModel rtn = new DefaultComboBoxModel();
		rtn.addElement("MPGM1");
		rtn.addElement("MGMMean1");
		rtn.addElement("BeRebid1");
		rtn.addElement("SEI1");
		rtn.addElement("MPGM2");
		rtn.addElement("MGMMean2");
		rtn.addElement("BeRebid2");
		rtn.addElement("SEI2");
		return rtn;		
	}
	
	public DefaultComboBoxModel createMovementDriverBoxModel() {
		DefaultComboBoxModel rtn = new DefaultComboBoxModel();
		rtn.addElement("Use Dummy Movement Driver");
		rtn.addElement("PlayerStage (*.cfg path):");
		return rtn;
	}

	/**
	 * This method initializes jContentPane	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			label1 = new Label();
			label1.setText("IP Address:");
			jContentPane = new JPanel();
			jContentPane.setLayout(null);
			jContentPane.add(getRow1Panel(), null);
			jContentPane.add(getRow2Panel(), null);
			jContentPane.add(getRow3Panel(), null);
			jContentPane.add(getGraphPane(), null);
			jContentPane.add(getRow4Panel(), null);
			jContentPane.add(getRow5Panel(), null);
			jContentPane.add(getRow5(), null);
		}
		return jContentPane;
	}

	/**
	 * This method initializes ipAddrField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	public JTextField getIpAddrField() {
		if (ipAddrField == null) {
			ipAddrField = new JTextField();
		}
		return ipAddrField;
	}

	/**
	 * This method initializes row1Panel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getRow1Panel() {
		if (row1Panel == null) {
			GridLayout gridLayout = new GridLayout();
			gridLayout.setRows(1);
			label2 = new Label();
			label2.setText("MAC Address:");
			row1Panel = new JPanel();
			row1Panel.setLayout(gridLayout);
			row1Panel.setBounds(new Rectangle(15, 14, 490, 50));
			row1Panel.add(label1, null);
			row1Panel.add(getIpAddrField(), null);
			row1Panel.add(label2, null);
			row1Panel.add(getHwAddrField(), null);
			row1Panel.add(getAddRobotButton(), null);
			TitledBorder title1;
			title1 = BorderFactory.createTitledBorder("Modify Robots in Configuration");
			row1Panel.setBorder(title1);
		}
		return row1Panel;
	}

	/**
	 * This method initializes hwAddrField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	public JTextField getHwAddrField() {
		if (hwAddrField == null) {
			hwAddrField = new JTextField();
		}
		return hwAddrField;
	}

	/**
	 * This method initializes addRobotButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	public JButton getAddRobotButton() {
		if (addRobotButton == null) {
			addRobotButton = new JButton("Add Robot");
			addRobotButton.setText("Add Robot");
			addRobotButton.addActionListener(l);
		}
		return addRobotButton;
	}

	/**
	 * This method initializes row2Panel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getRow2Panel() {
		if (row2Panel == null) {
			label4 = new Label();
			label4.setText("Robot 2 ID:");
			label3 = new Label();
			label3.setText("Robot 1 ID:");
			GridLayout gridLayout1 = new GridLayout();
			gridLayout1.setRows(1);
			row2Panel = new JPanel();
			row2Panel.setBorder(BorderFactory.createTitledBorder("Modify Network Topology"));
			row2Panel.setLayout(gridLayout1);
			row2Panel.setBounds(new Rectangle(15, 70, 490, 50));
			row2Panel.add(label3, null);
			row2Panel.add(getRobot1ID(), null);
			row2Panel.add(label4, null);
			row2Panel.add(getRobot2ID(), null);
			row2Panel.add(getAddNetworkConnButton(), null);
		}
		return row2Panel;
	}

	/**
	 * This method initializes addNetworkConnButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	public JButton getAddNetworkConnButton() {
		if (addNetworkConnButton == null) {
			addNetworkConnButton = new JButton("Add Link");
			addNetworkConnButton.addActionListener(l);
		}
		return addNetworkConnButton;
	}

	/**
	 * This method initializes robot1ID	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	public JComboBox getRobot1ID() {
		if (robot1ID == null) {
			robot1ID = new JComboBox();
		}
		return robot1ID;
	}

	/**
	 * This method initializes robot2ID	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	public JComboBox getRobot2ID() {
		if (robot2ID == null) {
			robot2ID = new JComboBox();
		}
		return robot2ID;
	}

	/**
	 * This method initializes row3Panel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getRow3Panel() {
		if (row3Panel == null) {
			label5 = new Label();
			label5.setText("Chain Nodes:");
			GridLayout gridLayout3 = new GridLayout();
			gridLayout3.setRows(1);
			row3Panel = new JPanel();
			row3Panel.setBorder(BorderFactory.createTitledBorder("Autogenerate Graph Topology"));
			row3Panel.setLayout(gridLayout3);
			row3Panel.setBounds(new Rectangle(15, 129, 490, 50));
			row3Panel.add(label5, null);
			row3Panel.add(getChainNodesField(), null);
			row3Panel.add(getChainGraphButton(), null);
			row3Panel.add(getFullGraphButton(), null);
		}
		return row3Panel;
	}

	/**
	 * This method initializes fullGraphButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	public JButton getFullGraphButton() {
		if (fullGraphButton == null) {
			fullGraphButton = new JButton("Full Graph");
			fullGraphButton.setText("Full Graph");
			fullGraphButton.addActionListener(l);
		}
		return fullGraphButton;
	}

	/**
	 * This method initializes chainGraphButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	public JButton getChainGraphButton() {
		if (chainGraphButton == null) {
			chainGraphButton = new JButton("Chain Graph");
			chainGraphButton.setText("Make Chain");
			chainGraphButton.addActionListener(l);
		}
		return chainGraphButton;
	}

	/**
	 * This method initializes chainNodesField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	public JTextField getChainNodesField() {
		if (chainNodesField == null) {
			chainNodesField = new JTextField();
		}
		return chainNodesField;
	}

	/**
	 * This method initializes graphPane	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getGraphPane() {
		if (graphPane == null) {
			graphPane = new JScrollPane();
			graphPane.setBounds(new Rectangle(17, 191, 482, 176));
			graphPane.setViewportView(getRobotTable());
		}
		return graphPane;
	}

	/**
	 * This method initializes robotTable	
	 * 	
	 * @return javax.swing.JTable	
	 */
	public JTable getRobotTable() {
		if (robotTable == null) {
			robotTable = new JTable();
			robotTable.setEnabled(false);
		}
		return robotTable;
	}

	/**
	 * This method initializes row4Panel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getRow4Panel() {
		if (row4Panel == null) {
			GridLayout gridLayout4 = new GridLayout();
			gridLayout4.setRows(1);
			row4Panel = new JPanel();
			row4Panel.setBorder(BorderFactory.createTitledBorder("Remove Network Elements"));
			row4Panel.setLayout(gridLayout4);
			row4Panel.setBounds(new Rectangle(15, 377, 490, 50));
			row4Panel.add(getRemoveRobotBox(), null);
			row4Panel.add(getRemoveRobotButton(), null);
			row4Panel.add(getRemoveLinkBox(), null);
			row4Panel.add(getRemoveLinkButton(), null);
		}
		return row4Panel;
	}

	/**
	 * This method initializes removeRobotBox	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	public JComboBox getRemoveRobotBox() {
		if (removeRobotBox == null) {
			removeRobotBox = new JComboBox();
		}
		return removeRobotBox;
	}

	/**
	 * This method initializes removeLinkBox	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	public JComboBox getRemoveLinkBox() {
		if (removeLinkBox == null) {
			removeLinkBox = new JComboBox();
		}
		return removeLinkBox;
	}

	/**
	 * This method initializes removeRobotButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	public JButton getRemoveRobotButton() {
		if (removeRobotButton == null) {
			removeRobotButton = new JButton("Add Link");
			removeRobotButton.setText("Remove Robot");
			removeRobotButton.addActionListener(l);
		}
		return removeRobotButton;
	}

	/**
	 * This method initializes removeLinkButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	public JButton getRemoveLinkButton() {
		if (removeLinkButton == null) {
			removeLinkButton = new JButton("Add Link");
			removeLinkButton.setText("Remove Link");
			removeLinkButton.addActionListener(l);
		}
		return removeLinkButton;
	}

	/**
	 * This method initializes row5Panel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getRow5Panel() {
		if (row5Panel == null) {
			GridLayout gridLayout5 = new GridLayout(1,3);
//			gridLayout5.setRows(1);
			row5Panel = new JPanel();
			row5Panel.setBorder(BorderFactory.createTitledBorder("Save/Load Configuration"));
			row5Panel.setLayout(gridLayout5);
			row5Panel.setBounds(new Rectangle(13, 632, 490, 50));
			row5Panel.add(getSaveConfigButton(), null);
			row5Panel.add(getReloadConfigButton(), null);
			row5Panel.add(getDefaultConfigButton(), null);
		}
		return row5Panel;
	}

	/**
	 * This method initializes saveConfigButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	public JButton getSaveConfigButton() {
		if (saveConfigButton == null) {
			saveConfigButton = new JButton("Add Link");
			saveConfigButton.setText("Save Configuration");
			saveConfigButton.addActionListener(l);
		}
		return saveConfigButton;
	}

	/**
	 * This method initializes reloadConfigButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	public JButton getReloadConfigButton() {
		if (reloadConfigButton == null) {
			reloadConfigButton = new JButton("Add Link");
			reloadConfigButton.setText("Reload Configuration");
			reloadConfigButton.addActionListener(l);
		}
		return reloadConfigButton;
	}

	/**
	 * This method initializes row5	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getRow5() {
		if (row5 == null) {
			label11 = new Label();
			label11.setText("Movement Amount:");
			label10 = new Label();
			label10.setText("I Parameter:");
			label9 = new Label();
			label9.setText("Average Signal Strength:");
			label8 = new Label();
			label8.setText("Maximum Signal Strength:");
			label7 = new Label();
			label7.setText("Number of Rounds:");
			label6 = new Label();
			label6.setText("DCEE Algorithm:");
			GridLayout gridLayout6 = new GridLayout(7,2);
			row5 = new JPanel();
			row5.setBorder(BorderFactory.createTitledBorder("DCEE Settings"));
			row5.setLayout(gridLayout6);
			row5.setBounds(new Rectangle(15, 436, 490, 180));
			row5.add(label6, null);
			row5.add(getAlgoBox(), null);
			row5.add(label7, null);
			row5.add(getNumRoundsField(), null);
			row5.add(label8, null);
			row5.add(getMaxSignalStrField(), null);
			row5.add(label9, null);
			row5.add(getAvgSignalStrengthField(), null);
			row5.add(label10, null);
			row5.add(getIParamField(), null);
			row5.add(label11, null);
			row5.add(getMovementField(), null);
			row5.add(getMovementDriverBox(), null);
			row5.add(getPlayerDriverField(), null);
		}
		return row5;
	}

	/**
	 * This method initializes algoBox	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	public JComboBox getAlgoBox() {
		if (algoBox == null) {
			algoBox = new JComboBox();
		}
		return algoBox;
	}

	/**
	 * This method initializes numRoundsField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	public JTextField getNumRoundsField() {
		if (numRoundsField == null) {
			numRoundsField = new JTextField();
		}
		return numRoundsField;
	}

	/**
	 * This method initializes maxSignalStrField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	public JTextField getMaxSignalStrField() {
		if (maxSignalStrField == null) {
			maxSignalStrField = new JTextField();
		}
		return maxSignalStrField;
	}

	/**
	 * This method initializes avgSignalStrengthField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	public JTextField getAvgSignalStrengthField() {
		if (avgSignalStrengthField == null) {
			avgSignalStrengthField = new JTextField();
		}
		return avgSignalStrengthField;
	}

	/**
	 * This method initializes iParamField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	public JTextField getIParamField() {
		if (iParamField == null) {
			iParamField = new JTextField();
		}
		return iParamField;
	}

	/**
	 * This method initializes movementField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	public JTextField getMovementField() {
		if (movementField == null) {
			movementField = new JTextField();
		}
		return movementField;
	}

	/**
	 * This method initializes defaultConfigButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	public JButton getDefaultConfigButton() {
		if (defaultConfigButton == null) {
			defaultConfigButton = new JButton("Default Configuration");
			defaultConfigButton.setText("Default Configuration");
			defaultConfigButton.addActionListener(l);
		}
		return defaultConfigButton;
	}

	/**
	 * This method initializes movementDriverBox	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	public JComboBox getMovementDriverBox() {
		if (movementDriverBox == null) {
			movementDriverBox = new JComboBox();
		}
		return movementDriverBox;
	}

	/**
	 * This method initializes playerDriverField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	public JTextField getPlayerDriverField() {
		if (playerDriverField == null) {
			playerDriverField = new JTextField();
		}
		return playerDriverField;
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
