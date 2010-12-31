package bbbleveleditor;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Point;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;

public class LevelPieceTriggerIDDialog extends JDialog {

	private static final long serialVersionUID = 57605608260878100L;
	
	private JButton okButton;
	private JButton cancelButton;
	private JSpinner triggerIDSpinner;
	private JCheckBox noTriggerIDChkBox;
	
	private LevelPieceImageLabel levelPieceLbl;
	
	public LevelPieceTriggerIDDialog(JFrame parentWindow, LevelPieceImageLabel levelPieceLbl) {
		super(parentWindow, "Trigger Identifier", true);
        this.setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);

		this.levelPieceLbl = levelPieceLbl;
		
		this.triggerIDSpinner = new JSpinner();
		this.triggerIDSpinner.setModel(new SpinnerNumberModel(0, 0, 10000, 1)); 
		
		this.noTriggerIDChkBox = new JCheckBox("No Trigger ID");
		this.noTriggerIDChkBox.setEnabled(true);
		if (levelPieceLbl.getTriggerID() == LevelPiece.NO_TRIGGER_ID) {
			this.noTriggerIDChkBox.setSelected(true);
			this.triggerIDSpinner.setEnabled(false);
		}
		else {
			this.noTriggerIDChkBox.setSelected(false);
			this.triggerIDSpinner.setEnabled(true);
			this.triggerIDSpinner.setValue(new Integer(levelPieceLbl.getTriggerID()));
		}
		
		this.noTriggerIDChkBox.addActionListener(new java.awt.event.ActionListener() {
        	public void actionPerformed(java.awt.event.ActionEvent evt) {
        		noTriggerIDChkBoxActionPerformed(evt);
            }
        });
		
		JPanel triggerIDPanel = new JPanel();
		triggerIDPanel.setLayout(new FlowLayout());
		triggerIDPanel.add(new JLabel("Trigger ID:"));
		triggerIDPanel.add(this.triggerIDSpinner);
		triggerIDPanel.add(this.noTriggerIDChkBox);
		
		this.okButton     = new JButton("OK");
		this.okButton.addActionListener(new java.awt.event.ActionListener() {
        	public void actionPerformed(java.awt.event.ActionEvent evt) {
        		okBtnActionPerformed(evt);
            }
        });
		
		this.cancelButton = new JButton("Cancel");
		this.cancelButton.addActionListener(new java.awt.event.ActionListener() {
        	public void actionPerformed(java.awt.event.ActionEvent evt) {
        		cancelBtnActionPerformed(evt);
            }
        });
		
		JPanel buttonPanel = new JPanel();
	    buttonPanel.setLayout(new FlowLayout());
	    buttonPanel.add(okButton);
	    buttonPanel.add(cancelButton);
		
	    this.getContentPane().setLayout(new BorderLayout());
	    this.getContentPane().add(triggerIDPanel, BorderLayout.CENTER);
	    this.getContentPane().add(buttonPanel, BorderLayout.SOUTH);
	    
		this.pack();
		
		this.setLocationRelativeTo(parentWindow);
		int parentWidth = parentWindow.getWidth();
		int parentHeight = parentWindow.getHeight();
		this.setLocation(new Point((parentWidth - this.getWidth()) / 2, (parentHeight - this.getHeight()) / 2));		
	}
	
	private void okBtnActionPerformed(java.awt.event.ActionEvent evt) {
		// Set the trigger id for the piece
		if (this.noTriggerIDChkBox.isSelected()) {
			this.levelPieceLbl.setTriggerID(LevelPiece.NO_TRIGGER_ID);
		}
		else {
			Integer value = (Integer)this.triggerIDSpinner.getValue();
			this.levelPieceLbl.setTriggerID(value.intValue());
		}
    	this.setVisible(false);
	}
	private void cancelBtnActionPerformed(java.awt.event.ActionEvent evt) {
		// Do nothing...
    	this.setVisible(false);
	}
	private void noTriggerIDChkBoxActionPerformed(java.awt.event.ActionEvent evt) {
		if (this.noTriggerIDChkBox.isSelected()) {
			this.triggerIDSpinner.setEnabled(false);
		}
		else {
			this.triggerIDSpinner.setEnabled(true);
		}
	}

}
