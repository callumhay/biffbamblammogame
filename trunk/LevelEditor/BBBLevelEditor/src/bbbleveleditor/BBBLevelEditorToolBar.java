package bbbleveleditor;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JToolBar;

public class BBBLevelEditorToolBar extends JToolBar {
	public BBBLevelEditorToolBar() {
		ImageIcon selectionIcon  = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/selection_icon_16x16.png"));
		JButton selectionBtn  = new JButton(selectionIcon);
		ImageIcon paintBrushIcon = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/paintbrush_icon_16x16.png"));
		JButton paintBrushBtn = new JButton(paintBrushIcon);
		ImageIcon eraserIcon     = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/eraser_icon_16x16.png"));
		JButton eraserBtn     = new JButton(eraserIcon);
		
		this.add(selectionBtn);
		this.add(paintBrushBtn);
		this.add(eraserBtn);	
	}
}
