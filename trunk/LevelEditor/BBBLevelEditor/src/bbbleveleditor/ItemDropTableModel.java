package bbbleveleditor;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map.Entry;

import javax.swing.table.AbstractTableModel;

public class ItemDropTableModel extends AbstractTableModel {
	private static final long serialVersionUID = -5334233111503776735L;
	
	// Columns: 		<Item_Name> <Item_Description> <Likelihood_Of_Drop>      <Only_If_Unlocked>
	// Types of data:	String		String			   ComboBox (String/Index)   Boolean
	public static final String[] COLUMN_NAMES = { 	"Item Name",
													"Description", 
													"Likelihood",
													"Only If Unlocked"};
	
	public static final String[] COLUMN_TOOLTIPS = { null, null, 
		"The relative likelihood of the item dropping while playing the level",
		"The item will only drop if it has been unlocked in a previous level"
	};		
	
	private String[] itemNames;
	private String[] itemDescriptions;
	private Integer[] dropLikelihoods;
	private Boolean[] onlyIfUnlockedChecks;
	
	public ItemDropTableModel(ItemDrop.ItemType itemTypes) {
		
		if (itemTypes == ItemDrop.ItemType.POWER_UP) {
			this.buildTableModel(ItemDrop.powerUpItems);
		}
		else if (itemTypes == ItemDrop.ItemType.POWER_NEUTRAL) {
			this.buildTableModel(ItemDrop.powerNeutralItems);
		}
		else {
			this.buildTableModel(ItemDrop.powerDownItems);
		}
	}
	
	private void buildTableModel(ArrayList<ItemDrop> items) {
		
		this.itemNames            = new String[items.size()];
		this.itemDescriptions     = new String[items.size()];
		this.dropLikelihoods      = new Integer[items.size()];
		this.onlyIfUnlockedChecks = new Boolean[items.size()];
		
		for (int i = 0; i < items.size(); i++) {
			ItemDrop currItemDrop 		= items.get(i);
			
			this.itemNames[i] 			 = currItemDrop.getName();
			this.itemDescriptions[i] 	 = currItemDrop.getDescription();
			this.dropLikelihoods[i] 	 = ItemDrop.NO_LIKELIHOOD;
			this.onlyIfUnlockedChecks[i] = new Boolean(false);
		}
	}
	
	public String[] GetItemNames() {
		return this.itemNames;
	}
	public Integer[] GetDropLikelihoods() {
		return this.dropLikelihoods;
	}
	
	public String getItemNameAtRow(int row) {
		return this.itemNames[row];
	}
	public int getItemLikelihoodAtRow(int row) {
		return this.dropLikelihoods[row];
	}
	public boolean getItemOnlyIfUnlockedAtRow(int row) {
		return this.onlyIfUnlockedChecks[row];
	}
	
	public void setFromItemDropSettingsHashTable(HashMap<String, ItemDropSettings> allSettings) {
		Iterator<Entry<String, ItemDropSettings>> iter = allSettings.entrySet().iterator();
		while (iter.hasNext()) {
			Entry<String, ItemDropSettings> entry = iter.next();
			this.setRowItemDropSettings(entry.getKey(), entry.getValue());
		}
	}
	
	public void setRowItemDropSettings(String itemName, ItemDropSettings settings) {
		int rowIdx = this.getRowIdxOfItem(itemName);
		if (rowIdx != -1) {
			this.setItemLikelihoodAtRow(rowIdx, settings.GetProbabilityValue());
			this.setItemOnlyIfUnlockedAtRow(rowIdx, settings.GetOnlyDropIfUnlocked());
		}
	}
	
	public void setItemLikelihoodAtRow(int row, int likelihood) {
		this.dropLikelihoods[row] = likelihood;
		this.fireTableCellUpdated(row, 2);
	}
	public void setItemOnlyIfUnlockedAtRow(int row, boolean onlyIfUnlocked) {
		this.onlyIfUnlockedChecks[row] = onlyIfUnlocked;
		this.fireTableCellUpdated(row, 3);
	}
	
	public int getRowIdxOfItem(String itemName) {
		for (int i = 0; i < itemNames.length; i++) {
			if (this.itemNames[i].equals(itemName)) {
				return i;
			}
		}
		return -1;
	}
	
	public void setItemLikelihood(String itemName, int likelihood) {
		int rowIdx = this.getRowIdxOfItem(itemName);
		if (rowIdx != -1) {
			this.setItemLikelihoodAtRow(rowIdx, likelihood);
		}
	}
	public void setItemOnlyIfUnlocked(String itemName, boolean onlyIfUnlocked) {
		int rowIdx = this.getRowIdxOfItem(itemName);
		if (rowIdx != -1) {
			this.setItemOnlyIfUnlockedAtRow(rowIdx, onlyIfUnlocked);
		}
	}
	
	@Override
	public String getColumnName(int col) {
		return ItemDropTableModel.COLUMN_NAMES[col];
	}
	
	@Override
	public int getColumnCount() {
		return ItemDropTableModel.COLUMN_NAMES.length;
	}

	@Override
	public int getRowCount() {
		return this.itemNames.length;
	}

	@Override
	public Object getValueAt(int row, int col) {
		switch (col) {
		case 0:
			return this.itemNames[row];
		case 1:
			return this.itemDescriptions[row];
		case 2:
			return ItemDrop.LIKEYIHOOD_STRINGS[this.dropLikelihoods[row]];
		case 3:
			return this.onlyIfUnlockedChecks[row];
		default:
			assert(false);
			break;
		}
		
		return null;
	}

	@Override
	public Class<? extends Object> getColumnClass(int c) {
		return getValueAt(0, c).getClass();
	}
	
	@Override
	public boolean isCellEditable(int rowIndex, int columnIndex) {
		// Only the 'can drop' and 'likelihood' columns can be edited
		return (columnIndex == 2 || columnIndex == 3);
	}
	
	@Override
	public void setValueAt(Object value, int row, int col) {
		switch (col) {
		case 0:
			this.itemNames[row] = (String)value;
			break;
		case 1:
			this.itemDescriptions[row] = (String)value;
			break;
		case 2:
			String likelihoodStr = (String)value;
			for (int i = 0; i < ItemDrop.LIKEYIHOOD_STRINGS.length; i++) {
				if (ItemDrop.LIKEYIHOOD_STRINGS[i].equals(likelihoodStr)) {
					this.dropLikelihoods[row] = i;
				}
			}	
			break;
		case 3:
			this.onlyIfUnlockedChecks[row] = (Boolean)value;
			break;
		default:
			assert(false);
			break;
		}
		
		this.fireTableCellUpdated(row, col);
	}
	
}
