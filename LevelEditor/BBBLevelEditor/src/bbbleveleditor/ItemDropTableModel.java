package bbbleveleditor;

import java.util.ArrayList;

import javax.swing.table.AbstractTableModel;

public class ItemDropTableModel extends AbstractTableModel {
	private static final long serialVersionUID = -5334233111503776735L;
	
	// Columns: 		<Item_Name> <Item_Description>  <Can_Drop_In_Level> <Likelihood_Of_Drop>
	// Types of data:	String		String				Boolean				ComboBox (String/Index)
	public static final String[] COLUMN_NAMES = { 	"Item Name",
													"Description", 
													"Likelihood" };
	
	public static final String[] COLUMN_TOOLTIPS = { null, null, 
		"The relative likelihood of the item dropping while playing the level"
	};		
	
	private String[] itemNames;
	private String[] itemDescriptions;
	private Integer[] dropLikelihoods;
	
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
		this.itemNames = new String[items.size()];
		this.itemDescriptions = new String[items.size()];
		this.dropLikelihoods = new Integer[items.size()];
		
		for (int i = 0; i < items.size(); i++) {
			ItemDrop currItemDrop 		= items.get(i);
			
			this.itemNames[i] 			= currItemDrop.getName();
			this.itemDescriptions[i] 	= currItemDrop.getDescription();
			this.dropLikelihoods[i] 	= ItemDrop.MEDIUM_LIKELIHOOD;
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
	public void setItemLikelihoodAtRow(int row, Integer likelihood) {
		this.dropLikelihoods[row] = likelihood;
		this.fireTableCellUpdated(row, 2);
	}
	public void setItemLikelihood(String itemName, Integer likelihood) {
		for (int i = 0; i < itemNames.length; i++) {
			if (this.itemNames[i].equals(itemName)) {
				this.dropLikelihoods[i] = likelihood;
				this.fireTableCellUpdated(i, 2);
				break;
			}
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
			return itemNames[row];
		case 1:
			return itemDescriptions[row];
		case 2:
			return ItemDrop.LIKEYIHOOD_STRINGS[this.dropLikelihoods[row]];
		default:
			assert(false);
			break;
		}
		
		return null;
	}

	@SuppressWarnings("unchecked")
	@Override
	public Class getColumnClass(int c) {
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
			itemNames[row] = (String)value;
			break;
		case 1:
			itemDescriptions[row] = (String)value;
			break;
		case 2:
			String likelihoodStr = (String)value;
			for (int i = 0; i < ItemDrop.LIKEYIHOOD_STRINGS.length; i++) {
				if (ItemDrop.LIKEYIHOOD_STRINGS[i].equals(likelihoodStr)) {
					dropLikelihoods[row] = i;
				}
			}	
			break;
		default:
			assert(false);
			break;
		}
		
		this.fireTableCellUpdated(row, col);
	}
	
}
