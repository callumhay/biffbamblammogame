package bbbleveleditor;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

public class EditItemDropsDialog extends JDialog implements ActionListener {
	
	private BBBLevelEditMainWindow levelEditWindow;
	private JButton applyButton;
	private JButton okButton;
	private JButton cancelButton;
	private boolean exitWithOK;
	
	public EditItemDropsDialog(BBBLevelEditMainWindow window) {
		super(window, "Level Drop Items");
		
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
			//this.levelEditWindow.setItemDropsForLevelEditDocument(...);
		}
	}
	
	public boolean getExitedWithOK() {
		return this.exitWithOK;
	}	
}
