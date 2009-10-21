package bbbleveleditor;

import java.awt.Component;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.ListCellRenderer;

public class LevelPieceListRenderer extends JLabel implements ListCellRenderer {
	private static final long serialVersionUID = 1L;
    
	public LevelPieceListRenderer() {
        setOpaque(true);
        setHorizontalAlignment(CENTER);
        setVerticalAlignment(CENTER);
    }
    
	@Override
	public Component getListCellRendererComponent(JList list, Object value, int index, boolean isSelected, boolean hasFocus) {
        
		// Set the selection background colour
        if (isSelected) {
            this.setBackground(list.getSelectionBackground());
            this.setForeground(list.getSelectionForeground());
        } 
        else {
            this.setBackground(list.getBackground());
            this.setForeground(list.getForeground());
        }
        
        // Set the level piece image and name
        LevelPiece levelPiece = (LevelPiece)value;
        if (levelPiece == null) {
        	this.setText("ERROR");
        }
        else {
        	this.setText(levelPiece.getName());
        	this.setIcon(new ImageIcon(levelPiece.getImage()));
        }

		return this;
	}

}
