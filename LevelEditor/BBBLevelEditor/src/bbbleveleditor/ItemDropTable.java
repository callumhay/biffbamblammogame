package bbbleveleditor;

import java.awt.event.MouseEvent;

import javax.swing.*;
import javax.swing.table.JTableHeader;
import javax.swing.table.TableColumnModel;

import java.util.*;
import java.util.Map.Entry;

public class ItemDropTable extends JTable {
	private static final long serialVersionUID = -4882973958052801537L;

	public ItemDropTable(ItemDrop.ItemType itemTypes) {
		super(new ItemDropTableModel(itemTypes));
		this.setAutoResizeMode(JTable.AUTO_RESIZE_ALL_COLUMNS);
		
		this.getColumnModel().getColumn(0).setPreferredWidth(150);
		this.getColumnModel().getColumn(1).setPreferredWidth(400);
		this.getColumnModel().getColumn(2).setPreferredWidth(80);
		this.getColumnModel().getColumn(3).setPreferredWidth(50);
	}
	
	public void getItemDropSettings(HashMap<String, ItemDropSettings> settings) {
		ItemDropTableModel model = (ItemDropTableModel)this.getModel();
		int numSettings = model.getRowCount();
		
		for (int i = 0; i < numSettings; i++) {
			settings.put(model.getItemNameAtRow(i), new ItemDropSettings(model.getItemLikelihoodAtRow(i), model.getItemOnlyIfUnlockedAtRow(i)));
		}
	}
	
	public void setItemDropSettings(HashMap<String, ItemDropSettings> settings) {
		
		ItemDropTableModel model = (ItemDropTableModel)this.getModel();
		Iterator<Entry<String, ItemDropSettings>> iter = settings.entrySet().iterator();
		while (iter.hasNext()) {
			Entry<String, ItemDropSettings> entry = iter.next();
			model.setRowItemDropSettings(entry.getKey(), entry.getValue());
		}
	}
	
	public class ItemDropTableHeader extends JTableHeader {
		private static final long serialVersionUID = 8198803046852968362L;

		public ItemDropTableHeader(TableColumnModel tableColumnModel) {
			super(tableColumnModel);
			this.setReorderingAllowed(false);
			this.setResizingAllowed(true);
		}
		
		public String getToolTipText(MouseEvent e) {
	        java.awt.Point p = e.getPoint();
	        int index = columnModel.getColumnIndexAtX(p.x);
	        int realIndex = columnModel.getColumn(index).getModelIndex();
	        return ItemDropTableModel.COLUMN_TOOLTIPS[realIndex];
	    }
	}	
	
	protected JTableHeader createDefaultTableHeader() {
		return new ItemDropTableHeader(columnModel);
	}
}
