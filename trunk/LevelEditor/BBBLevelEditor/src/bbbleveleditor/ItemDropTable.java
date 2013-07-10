package bbbleveleditor;

import java.awt.event.MouseEvent;
import javax.swing.*;
import javax.swing.table.JTableHeader;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;

import java.util.*;

public class ItemDropTable extends JTable {
	private static final long serialVersionUID = -4882973958052801537L;

	public ItemDropTable(ItemDrop.ItemType itemTypes) {
		super(new ItemDropTableModel(itemTypes));
		this.setAutoResizeMode(JTable.AUTO_RESIZE_ALL_COLUMNS);
		
		TableColumn column = null;
		for (int i = 0; i < this.getColumnCount(); i++) {
		    column = this.getColumnModel().getColumn(i);
		    column.setMaxWidth(Integer.MAX_VALUE);
		}
	}
	
	public void getItemDropSettings(HashMap<String, Integer> settings) {
		ItemDropTableModel model = (ItemDropTableModel)this.getModel();
		int numSettings = model.getRowCount();
		
		for (int i = 0; i < numSettings; i++) {
			settings.put(model.getItemNameAtRow(i), model.getItemLikelihoodAtRow(i));
		}
	}
	
	public void setItemDropSettings(HashMap<String, Integer> settings) {
		
		ItemDropTableModel model = (ItemDropTableModel)this.getModel();
		Iterator<String> iter = settings.keySet().iterator();
		while (iter.hasNext()) {
			String itemDropName = iter.next();
			model.setItemLikelihood(itemDropName, settings.get(itemDropName));
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
