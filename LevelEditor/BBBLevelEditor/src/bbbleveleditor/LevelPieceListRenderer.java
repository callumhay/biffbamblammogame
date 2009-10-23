package bbbleveleditor;

import java.awt.Component;
import java.awt.Dimension;

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
        LevelPiece levelPiece = null;
        try {
        	levelPiece = (LevelPiece)value;
        }
        catch (Exception e) {
        	levelPiece = null;
        	assert false;
        }
        
        if (levelPiece == null) {
        	this.setText("ERROR");
        }
        else {
        	ImageIcon img = levelPiece.getImageIcon();
        	int imgWidth = img.getIconWidth();
        	int imgHeight = img.getIconHeight();
        	this.setIcon(img);
        	this.setPreferredSize(new Dimension(imgWidth+10, imgHeight+10));
        	this.setToolTipText(levelPiece.getName());
        }

		return this;
	}

}
