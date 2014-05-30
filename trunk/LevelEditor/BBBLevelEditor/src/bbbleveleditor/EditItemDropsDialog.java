package bbbleveleditor;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
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
		HashMap<String, ItemDropSettings> itemDropTypes = itemDropPiece.getItemDropTypes();
		
		ItemDropTableModel powerUpsData      = (ItemDropTableModel)this.powerUpsTable.getModel();
		ItemDropTableModel powerNeutralsData = (ItemDropTableModel)this.powerNeutralsTable.getModel();
		ItemDropTableModel powerDownsData    = (ItemDropTableModel)this.powerDownsTable.getModel();
		
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
		
		
		// Find the maximum value in the hash
		float maxValue = 0.0f;
		Iterator<Entry<String, ItemDropSettings>> iter = itemDropTypes.entrySet().iterator();
		while (iter.hasNext()) {
			maxValue = Math.max(maxValue, iter.next().getValue().GetProbabilityValue());
		}
		
		iter = itemDropTypes.entrySet().iterator();
		while (iter.hasNext()) {
			Entry<String, ItemDropSettings> currEntry = iter.next();

			float percentValue = ((float)currEntry.getValue().GetProbabilityValue()) / maxValue;
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
			
			powerUpsData.setItemOnlyIfUnlocked(currEntry.getKey(), currEntry.getValue().GetOnlyDropIfUnlocked());
			powerNeutralsData.setItemOnlyIfUnlocked(currEntry.getKey(), currEntry.getValue().GetOnlyDropIfUnlocked());
			powerDownsData.setItemOnlyIfUnlocked(currEntry.getKey(), currEntry.getValue().GetOnlyDropIfUnlocked());
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
	
	public void setItemDropSettings(HashMap<String, ItemDropSettings> settings) {
		this.powerUpsTable.setItemDropSettings(settings);
		this.powerNeutralsTable.setItemDropSettings(settings);
		this.powerDownsTable.setItemDropSettings(settings);
	}
	
	public HashMap<String, ItemDropSettings> getItemDropSettings() {
		HashMap<String, ItemDropSettings> settings = new HashMap<String, ItemDropSettings>();
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
			
			HashMap<String, ItemDropSettings> itemDropTypes = new HashMap<String, ItemDropSettings>();
			
			// Add each power-up individually...
			for (int i = 0; i < powerUpsData.getRowCount(); i++) {
				itemDropTypes.put(powerUpsData.getItemNameAtRow(i), 
					new ItemDropSettings(powerUpsData.getItemLikelihoodAtRow(i), powerUpsData.getItemOnlyIfUnlockedAtRow(i)));
			}

			// Add each power-neutral individually...
			for (int i = 0; i < powerNeutralsData.getRowCount(); i++) {
				itemDropTypes.put(powerNeutralsData.getItemNameAtRow(i),
					new ItemDropSettings(powerNeutralsData.getItemLikelihoodAtRow(i), powerNeutralsData.getItemOnlyIfUnlockedAtRow(i)));
			}

			// Add each power-down individually...
			for (int i = 0; i < powerDownsData.getRowCount(); i++) {
				itemDropTypes.put(powerDownsData.getItemNameAtRow(i),
						new ItemDropSettings(powerDownsData.getItemLikelihoodAtRow(i), powerDownsData.getItemOnlyIfUnlockedAtRow(i)));
			}

			this.itemDropPiece.setItemDropTypes(itemDropTypes);
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
