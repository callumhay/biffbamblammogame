package bbbleveleditor;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.HashMap;

import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.table.TableColumn;

public class EditItemDropsDialog extends JDialog implements ActionListener {
	private static final long serialVersionUID = 3548973828806465681L;
	
	private BBBLevelEditMainWindow levelEditWindow;
	private JButton applyButton;
	private JButton okButton;
	private JButton cancelButton;
	private boolean exitWithOK;
	
	private ItemDropTable powerUpsTable;
	private ItemDropTable powerNeutralsTable;
	private ItemDropTable powerDownsTable;
	
	public EditItemDropsDialog(BBBLevelEditMainWindow window) {
		super(window, "Level Drop Items", true);
		
		this.levelEditWindow = window;
		
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
		
		JPanel tablePanel = new JPanel();
		tablePanel.setLayout(new BoxLayout(tablePanel, BoxLayout.Y_AXIS));
		tablePanel.add(powerUpTableScroll);
		tablePanel.add(powerNeutralTableScroll);
		tablePanel.add(powerDownTableScroll);
		
		this.setLayout(new BorderLayout());
		this.add(tablePanel, BorderLayout.CENTER);
		this.add(buttonPanel, BorderLayout.SOUTH);
		
		this.pack();
		
		this.setLocationRelativeTo(window);
		int parentWidth = window.getWidth();
		int parentHeight = window.getHeight();
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
	
	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("OK")) {
			this.exitWithOK = true;
			this.setVisible(false);
		}
		else if (e.getActionCommand().equals("Cancel")) {
			this.exitWithOK = false;
			this.setVisible(false);
		}
		else if (e.getActionCommand().equals("Apply")) {
			this.levelEditWindow.setItemDropsForLevelEditDocument(this.getItemDropSettings());
		}
	}
	
	public boolean getExitedWithOK() {
		return this.exitWithOK;
	}

}
