package bbbleveleditor;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Point;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class EditLevelPropertiesDialog extends JDialog {

	private static final long serialVersionUID = 1L;
	
	private BBBLevelEditDocumentWindow docWindow;
	private JTextField levelNameTxtField;
	private JButton okButton;
	private JButton cancelButton;
	
	public EditLevelPropertiesDialog(BBBLevelEditMainWindow mainWindow, BBBLevelEditDocumentWindow docWindow) {
		super(mainWindow, "Level Properties", true);
		this.docWindow = docWindow;
		
		this.levelNameTxtField = new JTextField(docWindow.getLevelName(), 50);
		this.levelNameTxtField.addActionListener(new java.awt.event.ActionListener() {
        	public void actionPerformed(java.awt.event.ActionEvent evt) {
        		nameTextFieldChangedActionPerformed(evt);
            }
        });
		
		JPanel namePanel = new JPanel();
		namePanel.setLayout(new FlowLayout());
		namePanel.add(new JLabel("Level Name:"));
		namePanel.add(this.levelNameTxtField);	
		
		this.setLayout(new BorderLayout());
		this.add(namePanel, BorderLayout.CENTER);
		
		this.okButton = new JButton("OK");
		this.okButton.setActionCommand("OK");
		this.okButton.addActionListener(new java.awt.event.ActionListener() {
        	public void actionPerformed(java.awt.event.ActionEvent evt) {
        		okButtonActionPerformed(evt);
            }
        });
		this.cancelButton = new JButton("Cancel");
		this.cancelButton.setActionCommand("Cancel");
		this.cancelButton.addActionListener(new java.awt.event.ActionListener() {
        	public void actionPerformed(java.awt.event.ActionEvent evt) {
        		cancelButtonActionPerformed(evt);
            }
        });
		
		JPanel okCancelButtonPanel = new JPanel(new FlowLayout());
		okCancelButtonPanel.add(this.okButton);
		okCancelButtonPanel.add(this.cancelButton);
		this.add(okCancelButtonPanel, BorderLayout.SOUTH);
		
		this.pack();
		
		this.setResizable(true);
		this.setLocationRelativeTo(mainWindow);
		int parentWidth = mainWindow.getWidth();
		int parentHeight = mainWindow.getHeight();
		this.setLocation(new Point((parentWidth - this.getWidth()) / 2, (parentHeight - this.getHeight()) / 2));
	}
	
	private void nameTextFieldChangedActionPerformed(java.awt.event.ActionEvent evt) {
	}
	
	private void okButtonActionPerformed(java.awt.event.ActionEvent evt) {
		if (this.levelNameTxtField.getText().isEmpty()) {
			JOptionPane.showMessageDialog(this,
				    "The level name must not be empty.",
				    "Level Property Error",
				    JOptionPane.ERROR_MESSAGE);
			return;
		}
		
		this.docWindow.setLevelName(this.levelNameTxtField.getText());
		this.setVisible(false);
	}
	
	private void cancelButtonActionPerformed(java.awt.event.ActionEvent evt) {
		this.setVisible(false);
	}
	
}
