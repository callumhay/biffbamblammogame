package bbbleveleditor;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;

public class BBBLevelEditorToolBar extends JToolBar implements ActionListener {
	private static final long serialVersionUID = 1L;

	private BBBLevelEditMainWindow levelEditWindow;
	
	//private JToggleButton selectionBtn;
	private JToggleButton paintBrushBtn;
	private JToggleButton eraserBtn;
	
	public BBBLevelEditorToolBar(BBBLevelEditMainWindow window) {
		this.levelEditWindow = window;
		
		ButtonGroup toolbarButtonGrp = new ButtonGroup();
		
		//ImageIcon selectionIcon  = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/selection_icon_16x16.png"));
		//this.selectionBtn = new JButton(selectionIcon);
		//this.selectionBtn.setToolTipText("Select and drag/drop level pieces");
		//this.selectionBtn.setActionCommand("select");
		//this.selectionBtn.addActionListener(this);
			
		ImageIcon paintBrushIcon = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/paintbrush_icon_16x16.png"));
		this.paintBrushBtn = new JToggleButton(paintBrushIcon);
		this.paintBrushBtn.setToolTipText("Paint the selected level piece");
		this.paintBrushBtn.setActionCommand("paint");
		this.paintBrushBtn.addActionListener(this);
		this.paintBrushBtn.setSelected(true);
		toolbarButtonGrp.add(this.paintBrushBtn);
		
		window.setToolMode(BBBLevelEditMainWindow.ToolMode.PAINT);
		
		ImageIcon eraserIcon = new ImageIcon(BBBLevelEditorMain.class.getResource("resources/eraser_icon_16x16.png"));
		this.eraserBtn = new JToggleButton(eraserIcon);
		this.eraserBtn.setToolTipText("Erase level pieces back to an empty piece");
		this.eraserBtn.setActionCommand("erase");
		this.eraserBtn.addActionListener(this);
		this.eraserBtn.setSelected(false);
		toolbarButtonGrp.add(this.eraserBtn);
		
		//this.add(this.selectionBtn);
		this.add(this.paintBrushBtn);
		this.add(this.eraserBtn);
	}

	public void actionPerformed(ActionEvent e) {
		//if (e.getActionCommand().equals("select")) {
		//}
		if (e.getActionCommand().equals("paint")) {
			this.levelEditWindow.setToolMode(BBBLevelEditMainWindow.ToolMode.PAINT);
		}
		else if (e.getActionCommand().equals("erase")) {
			this.levelEditWindow.setToolMode(BBBLevelEditMainWindow.ToolMode.ERASE);
		}
	}
	
	
	
}
