package bbbleveleditor;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map.Entry;

import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.table.TableColumn;

public class EditItemDropsDialog extends JDialog implements ActionListener {
	private static final long serialVersionUID = 3548973828806465681L;
	
	private LevelPieceImageLabel itemDropPiece;
	
	private BBBLevelEditMainWindow levelEditWindow;
	private JButton applyButton;
	private JButton okButton;
	private JButton cancelButton;
	private boolean exitWithOK;
	
	private ItemDropTable powerUpsTable;
	private ItemDropTable powerNeutralsTable;
	private ItemDropTable powerDownsTable;
	
	private JComboBox powerUpsLikelyhoodComboBox;
	private JComboBox powerNeutralsLikelyhoodComboBox;
	private JComboBox powerDownsLikelyhoodComboBox;
	
	public EditItemDropsDialog(BBBLevelEditMainWindow window) {
		super(window, "Level Drop Items", true);
		
		this.levelEditWindow = window;
		this.setupGUI();
	}
	
	public EditItemDropsDialog(BBBLevelEditMainWindow window, LevelPieceImageLabel itemDropPiece) {
		super(window, "Item Drop Block Item Types", true);
		
		this.levelEditWindow = window;
		this.itemDropPiece = itemDropPiece;
		this.setupGUI();
		
		// Read the current state of the item drop piece and use it to establish what item types
		// are currently being dropped by it...
		Iterator<String> iter = itemDropPiece.getItemDropTypes().iterator();
		
		ItemDropTableModel powerUpsData      = (ItemDropTableModel)this.powerUpsTable.getModel();
		ItemDropTableModel powerNeutralsData = (ItemDropTableModel)this.powerNeutralsTable.getModel();
		ItemDropTableModel powerDownsData    = (ItemDropTableModel)this.powerDownsTable.getModel();
		
		HashMap<String, Integer> itemCounts = new HashMap<String, Integer>();
		while (iter.hasNext()) {
			String currItemTypeName = iter.next();
			if (itemCounts.containsKey(currItemTypeName)) {
				itemCounts.put(currItemTypeName, new Integer(itemCounts.get(currItemTypeName).intValue() + 1));
			}
			else {
				itemCounts.put(currItemTypeName, new Integer(1));
			}
		}
		
		// Find the maximum value in the hash
		float maxValue = 0.0f;
		Iterator<Integer> countValueIter = itemCounts.values().iterator();
		while (countValueIter.hasNext()) {
			maxValue = Math.max(maxValue, countValueIter.next());
		}
		
		// Set everything to NONE to begin with
		for (int i = 0; i < powerUpsData.getRowCount(); i++) {
			powerUpsData.setItemLikelihoodAtRow(i, ItemDrop.NO_LIKELIHOOD);
		}
		for (int i = 0; i < powerNeutralsData.getRowCount(); i++) {
			powerNeutralsData.setItemLikelihoodAtRow(i, ItemDrop.NO_LIKELIHOOD);
		}
		for (int i = 0; i < powerDownsData.getRowCount(); i++) {
			powerDownsData.setItemLikelihoodAtRow(i, ItemDrop.NO_LIKELIHOOD);
		}
		
		Iterator<Entry<String, Integer>> hashEntryIter = itemCounts.entrySet().iterator();
		while (hashEntryIter.hasNext()) {
			Entry<String, Integer> currEntry = hashEntryIter.next();

			// Handle some special cases first...
			if (currEntry.getKey().equals("all")) {
				for (int i = 0; i < powerUpsData.getRowCount(); i++) {
					powerUpsData.setItemLikelihoodAtRow(i, ItemDrop.MEDIUM_LIKELIHOOD);
				}
				for (int i = 0; i < powerNeutralsData.getRowCount(); i++) {
					powerNeutralsData.setItemLikelihoodAtRow(i, ItemDrop.MEDIUM_LIKELIHOOD);
				}
				for (int i = 0; i < powerDownsData.getRowCount(); i++) {
					powerDownsData.setItemLikelihoodAtRow(i, ItemDrop.MEDIUM_LIKELIHOOD);
				}
			}
			else if (currEntry.getKey().equals("powerups")) {
				for (int i = 0; i < powerUpsData.getRowCount(); i++) {
					powerUpsData.setItemLikelihoodAtRow(i, ItemDrop.MEDIUM_LIKELIHOOD);
				}
			}
			else if (currEntry.getKey().equals("powerneutrals")) {
				for (int i = 0; i < powerNeutralsData.getRowCount(); i++) {
					powerNeutralsData.setItemLikelihoodAtRow(i, ItemDrop.MEDIUM_LIKELIHOOD);
				}
			}
			else if (currEntry.getKey().equals("powerdowns")) {
				for (int i = 0; i < powerDownsData.getRowCount(); i++) {
					powerDownsData.setItemLikelihoodAtRow(i, ItemDrop.MEDIUM_LIKELIHOOD);
				}
			}
			else {
				float percentValue = ((float)currEntry.getValue()) / maxValue;
				int likelihood = ItemDrop.NO_LIKELIHOOD;
				if (percentValue == 0.0f) {
					likelihood = ItemDrop.NO_LIKELIHOOD;
				}
				else if (percentValue <= 0.333333334f) {
					likelihood = ItemDrop.LOW_LIKELIHOOD;
				}
				else if (percentValue <= 0.666666667f) {
					likelihood = ItemDrop.MEDIUM_LIKELIHOOD;
				}
				else if (percentValue <= 1.0f) {
					likelihood = ItemDrop.HIGH_LIKELIHOOD;
				}
								
				
				powerUpsData.setItemLikelihood(currEntry.getKey(),      likelihood);
				powerNeutralsData.setItemLikelihood(currEntry.getKey(), likelihood);
				powerDownsData.setItemLikelihood(currEntry.getKey(),    likelihood);
			}
		}
	}
	
	private void setupGUI() {
		// Setup the apply, ok and cancel buttons
		this.applyButton = new JButton("Apply");
		this.applyButton.setActionCommand("Apply");
		this.applyButton.addActionListener(this);
		this.okButton = new JButton("OK");
		this.okButton.setActionCommand("OK");
		this.okButton.addActionListener(this);
		this.cancelButton = new JButton("Cancel");
		this.cancelButton.setActionCommand("Cancel");
		this.cancelButton.addActionListener(this);
	
		this.powerUpsLikelyhoodComboBox = new JComboBox(ItemDrop.LIKEYIHOOD_STRINGS);
		this.powerUpsLikelyhoodComboBox.setEditable(false);
		this.powerUpsLikelyhoodComboBox.setActionCommand("AllPowerUpsComboBox");
		this.powerUpsLikelyhoodComboBox.addActionListener(this);
		
		this.powerNeutralsLikelyhoodComboBox = new JComboBox(ItemDrop.LIKEYIHOOD_STRINGS);
		this.powerNeutralsLikelyhoodComboBox.setEditable(false);
		this.powerNeutralsLikelyhoodComboBox.setActionCommand("AllPowerNeutralsComboBox");
		this.powerNeutralsLikelyhoodComboBox.addActionListener(this);
		
		this.powerDownsLikelyhoodComboBox = new JComboBox(ItemDrop.LIKEYIHOOD_STRINGS);
		this.powerDownsLikelyhoodComboBox.setEditable(false);		
		this.powerDownsLikelyhoodComboBox.setActionCommand("AllPowerDownsComboBox");
		this.powerDownsLikelyhoodComboBox.addActionListener(this);
		
		
		JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
		buttonPanel.add(this.okButton);
		buttonPanel.add(this.cancelButton);
		buttonPanel.add(this.applyButton);
		
		JComboBox likelihoodCombo = new JComboBox();
		likelihoodCombo.addItem(ItemDrop.LIKEYIHOOD_STRINGS[ItemDrop.NO_LIKELIHOOD]);
		likelihoodCombo.addItem(ItemDrop.LIKEYIHOOD_STRINGS[ItemDrop.LOW_LIKELIHOOD]);
		likelihoodCombo.addItem(ItemDrop.LIKEYIHOOD_STRINGS[ItemDrop.MEDIUM_LIKELIHOOD]);
		likelihoodCombo.addItem(ItemDrop.LIKEYIHOOD_STRINGS[ItemDrop.HIGH_LIKELIHOOD]);
		
		// Setup the tables with each set of items
		this.powerUpsTable   	= new ItemDropTable(ItemDrop.ItemType.POWER_UP);
		this.powerUpsTable.setPreferredScrollableViewportSize(new Dimension(800, 100));
		TableColumn likelihoodCol = this.powerUpsTable.getColumnModel().getColumn(2);
		likelihoodCol.setCellEditor(new DefaultCellEditor(likelihoodCombo));
		likelihoodCol.setMinWidth(100);
		likelihoodCol.setMaxWidth(100);
		likelihoodCol.setPreferredWidth(100);
		TableColumn nameCol = this.powerUpsTable.getColumnModel().getColumn(0);
		nameCol.setMinWidth(120);
		nameCol.setMaxWidth(120);
		nameCol.setPreferredWidth(120);
		
		this.powerNeutralsTable = new ItemDropTable(ItemDrop.ItemType.POWER_NEUTRAL);
		this.powerNeutralsTable.setPreferredScrollableViewportSize(new Dimension(800, 100));
		likelihoodCol = this.powerNeutralsTable.getColumnModel().getColumn(2);
		likelihoodCol.setCellEditor(new DefaultCellEditor(likelihoodCombo));
		likelihoodCol.setMinWidth(100);
		likelihoodCol.setMaxWidth(100);
		likelihoodCol.setPreferredWidth(100);
		nameCol = this.powerNeutralsTable.getColumnModel().getColumn(0);
		nameCol.setMinWidth(120);
		nameCol.setMaxWidth(120);
		nameCol.setPreferredWidth(120);
		
		this.powerDownsTable 	= new ItemDropTable(ItemDrop.ItemType.POWER_DOWN);
		this.powerDownsTable.setPreferredScrollableViewportSize(new Dimension(800, 100));
		likelihoodCol = this.powerDownsTable.getColumnModel().getColumn(2);
		likelihoodCol.setCellEditor(new DefaultCellEditor(likelihoodCombo));
		likelihoodCol.setMinWidth(100);
		likelihoodCol.setMaxWidth(100);
		likelihoodCol.setPreferredWidth(100);
		nameCol = this.powerDownsTable.getColumnModel().getColumn(0);
		nameCol.setMinWidth(120);
		nameCol.setMaxWidth(120);
		nameCol.setPreferredWidth(120);
		
		JScrollPane powerUpTableScroll 		= new JScrollPane(this.powerUpsTable);
		JScrollPane powerNeutralTableScroll	= new JScrollPane(this.powerNeutralsTable);
		JScrollPane powerDownTableScroll	= new JScrollPane(this.powerDownsTable);
		
		this.powerUpsTable.setFillsViewportHeight(true);
		this.powerNeutralsTable.setFillsViewportHeight(true);
		this.powerDownsTable.setFillsViewportHeight(true);
		
		Border blackLine = BorderFactory.createLineBorder(Color.black);
		powerUpTableScroll.setBorder(BorderFactory.createTitledBorder(blackLine, "Power-Up Items"));
		powerNeutralTableScroll.setBorder(BorderFactory.createTitledBorder(blackLine, "Power-Neutral Items"));
		powerDownTableScroll.setBorder(BorderFactory.createTitledBorder(blackLine, "Power-Down Items"));
		
		JPanel powerUpComboBoxPanel = new JPanel();
		powerUpComboBoxPanel.setLayout(new FlowLayout());
		powerUpComboBoxPanel.add(new JLabel("Set all power-up likelihoods:"));
		powerUpComboBoxPanel.add(this.powerUpsLikelyhoodComboBox);
		
		JPanel powerNeutralComboBoxPanel = new JPanel();
		powerNeutralComboBoxPanel.setLayout(new FlowLayout());
		powerNeutralComboBoxPanel.add(new JLabel("Set all power-neutral likelihoods:"));
		powerNeutralComboBoxPanel.add(this.powerNeutralsLikelyhoodComboBox);
		
		JPanel powerDownComboBoxPanel = new JPanel();
		powerDownComboBoxPanel.setLayout(new FlowLayout());
		powerDownComboBoxPanel.add(new JLabel("Set all power-down likelihoods:"));
		powerDownComboBoxPanel.add(this.powerDownsLikelyhoodComboBox);	
		
		JPanel tablePanel = new JPanel();
		tablePanel.setLayout(new BoxLayout(tablePanel, BoxLayout.Y_AXIS));
		tablePanel.add(powerUpTableScroll);
		tablePanel.add(powerUpComboBoxPanel);
		tablePanel.add(powerNeutralTableScroll);
		tablePanel.add(powerNeutralComboBoxPanel);
		tablePanel.add(powerDownTableScroll);
		tablePanel.add(powerDownComboBoxPanel);
		
		this.setLayout(new BorderLayout());
		this.add(tablePanel, BorderLayout.CENTER);
		this.add(buttonPanel, BorderLayout.SOUTH);
		
		this.pack();
		
		this.setLocationRelativeTo(this.levelEditWindow);
		int parentWidth = this.levelEditWindow.getWidth();
		int parentHeight = this.levelEditWindow.getHeight();
		this.setLocation(new Point((parentWidth - this.getWidth()) / 2, (parentHeight - this.getHeight()) / 2));
	}
	
	public void setItemDropSettings(HashMap<String, Integer> settings) {
		this.powerUpsTable.setItemDropSettings(settings);
		this.powerNeutralsTable.setItemDropSettings(settings);
		this.powerDownsTable.setItemDropSettings(settings);
	}
	
	public HashMap<String, Integer> getItemDropSettings() {
		HashMap<String, Integer> settings = new HashMap<String, Integer>();
		this.powerUpsTable.getItemDropSettings(settings);
		this.powerNeutralsTable.getItemDropSettings(settings);
		this.powerDownsTable.getItemDropSettings(settings);
		
		return settings;
	}
	
	public void DoApply() {
		if (this.itemDropPiece != null) {
		
			ItemDropTableModel powerUpsData      = (ItemDropTableModel)this.powerUpsTable.getModel();
			ItemDropTableModel powerNeutralsData = (ItemDropTableModel)this.powerNeutralsTable.getModel();
			ItemDropTableModel powerDownsData    = (ItemDropTableModel)this.powerDownsTable.getModel();
			
			ArrayList<String> itemDropTypeStrs = new ArrayList<String>();
			
			// Start with power-ups check to see if every power up is included with the same likelihood...
			boolean allSame = true;
			boolean allAreAllSame = true;
			int commonLikelihood = powerUpsData.getItemLikelihoodAtRow(0);
			if (commonLikelihood != ItemDrop.NO_LIKELIHOOD) {
				for (int i = 0; i < powerUpsData.getRowCount(); i++) {
					if (powerUpsData.getItemLikelihoodAtRow(i) != commonLikelihood) {
						allSame = false;
						break;
					}
				}
			}
			else {
				allSame = false;
			}
			
			if (allSame) {
				itemDropTypeStrs.add("powerups");
			}
			else {
				allAreAllSame = false;
				// Add each power-up individually...
				for (int i = 0; i < powerUpsData.getRowCount(); i++) {
					for (int j = 0; j < powerUpsData.getItemLikelihoodAtRow(i); j++) {
						itemDropTypeStrs.add(powerUpsData.getItemNameAtRow(i));
					}
				}
			}
			
			// Now do the same for power-neutrals
			allSame = true;
			commonLikelihood = powerNeutralsData.getItemLikelihoodAtRow(0);
			if (commonLikelihood != ItemDrop.NO_LIKELIHOOD) {
				for (int i = 0; i < powerNeutralsData.getRowCount(); i++) {
					if (powerNeutralsData.getItemLikelihoodAtRow(i) != commonLikelihood) {
						allSame = false;
						break;
					}
				}
			}
			else {
				allSame = false;
			}
	
			if (allSame) {
				itemDropTypeStrs.add("powerneutrals");
			}
			else {
				allAreAllSame = false;
				// Add each power-neutral individually...
				for (int i = 0; i < powerNeutralsData.getRowCount(); i++) {
					for (int j = 0; j < powerNeutralsData.getItemLikelihoodAtRow(i); j++) {
						itemDropTypeStrs.add(powerNeutralsData.getItemNameAtRow(i));
					}
				}
			}
			
			// And finally, power-downs...
			allSame = true;
			commonLikelihood = powerDownsData.getItemLikelihoodAtRow(0);
			if (commonLikelihood != ItemDrop.NO_LIKELIHOOD) {
				for (int i = 0; i < powerDownsData.getRowCount(); i++) {
					if (powerDownsData.getItemLikelihoodAtRow(i) != commonLikelihood) {
						allSame = false;
						break;
					}
				}
			}
			else {
				allSame = false;
			}
			if (allSame) {
				itemDropTypeStrs.add("powerdowns");
			}
			else {
				allAreAllSame = false;
				// Add each power-down individually...
				for (int i = 0; i < powerDownsData.getRowCount(); i++) {
					for (int j = 0; j < powerDownsData.getItemLikelihoodAtRow(i); j++) {
						itemDropTypeStrs.add(powerDownsData.getItemNameAtRow(i));
					}
				}
			}
			
			if (allAreAllSame) {
				itemDropTypeStrs.clear();
				itemDropTypeStrs.add("all");
			}
			else {
				// Go through the string and eliminate duplicates...
				Iterator<String> iter = itemDropTypeStrs.iterator();
				String currStr = "";
				String lastStr = "";
				while (iter.hasNext()) {
					currStr = iter.next();
					if (currStr.equals(lastStr)) {
						iter.remove();
					}
					lastStr = currStr;
				}
			}
			
			this.itemDropPiece.setItemDropTypes(itemDropTypeStrs);
		}
		else if (this.levelEditWindow != null) {
			this.levelEditWindow.setItemDropsForLevelEditDocument(this.getItemDropSettings());
		}
	}
	
	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("OK")) {
			this.DoApply();
			this.exitWithOK = true;
			this.setVisible(false);
		}
		else if (e.getActionCommand().equals("Cancel")) {
			this.exitWithOK = false;
			this.setVisible(false);
		}
		else if (e.getActionCommand().equals("Apply")) {
			this.DoApply();
		}
		else if (e.getActionCommand().equals("AllPowerUpsComboBox")) {
			ItemDropTableModel powerUpsData      = (ItemDropTableModel)this.powerUpsTable.getModel();
			for (int i = 0; i < powerUpsData.getRowCount(); i++) {
				powerUpsData.setItemLikelihoodAtRow(i, this.powerUpsLikelyhoodComboBox.getSelectedIndex());
			}
		}
		else if (e.getActionCommand().equals("AllPowerNeutralsComboBox")) {
			ItemDropTableModel powerNeutralsData = (ItemDropTableModel)this.powerNeutralsTable.getModel();
			for (int i = 0; i < powerNeutralsData.getRowCount(); i++) {
				powerNeutralsData.setItemLikelihoodAtRow(i, this.powerNeutralsLikelyhoodComboBox.getSelectedIndex());
			}
		}
		else if (e.getActionCommand().equals("AllPowerDownsComboBox")) {
			ItemDropTableModel powerDownsData    = (ItemDropTableModel)this.powerDownsTable.getModel();
			for (int i = 0; i < powerDownsData.getRowCount(); i++) {
				powerDownsData.setItemLikelihoodAtRow(i, this.powerDownsLikelyhoodComboBox.getSelectedIndex());
			}
		}		
	}
	
	public boolean getExitedWithOK() {
		return this.exitWithOK;
	}

}
