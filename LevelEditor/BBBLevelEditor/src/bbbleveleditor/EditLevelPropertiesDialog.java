package bbbleveleditor;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.GridLayout;
import java.awt.Point;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.SpinnerNumberModel;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class EditLevelPropertiesDialog extends JDialog {

	private static final long serialVersionUID = 1L;
	
	private BBBLevelEditDocumentWindow docWindow;
	private JTextField levelNameTxtField;
	
	private JSpinner firstStarPtAmtSpinner;
	private JSpinner secondStarPtAmtSpinner;
	private JSpinner thirdStarPtAmtSpinner;
	private JSpinner fourthStarPtAmtSpinner;
	private JSpinner fifthStarPtAmtSpinner;
	
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
		
		int[] starPointAmts = docWindow.getStarPointAmounts();
		
		this.firstStarPtAmtSpinner  = new JSpinner(new SpinnerNumberModel(starPointAmts[0], 0, 9999999, 1));
		this.firstStarPtAmtSpinner.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent e) {
				starPointSpinnerChanged(e);
			}
		});
		
		this.secondStarPtAmtSpinner = new JSpinner(new SpinnerNumberModel(starPointAmts[1], 0, 9999999, 1));
		this.secondStarPtAmtSpinner.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent e) {
				starPointSpinnerChanged(e);
			}
		});
		
		this.thirdStarPtAmtSpinner  = new JSpinner(new SpinnerNumberModel(starPointAmts[2], 0, 9999999, 1));
		this.thirdStarPtAmtSpinner.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent e) {
				starPointSpinnerChanged(e);
			}
		});
		
		this.fourthStarPtAmtSpinner = new JSpinner(new SpinnerNumberModel(starPointAmts[3], 0, 9999999, 1));
		this.fourthStarPtAmtSpinner.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent e) {
				starPointSpinnerChanged(e);
			}
		});
		
		this.fifthStarPtAmtSpinner  = new JSpinner(new SpinnerNumberModel(starPointAmts[4], 0, 9999999, 1));
		this.fifthStarPtAmtSpinner.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent e) {
				starPointSpinnerChanged(e);
			}
		});
		
		// Setup the star points panel - for defining the point milestones for stars
		JPanel starPanel = new JPanel();
		TitledBorder starBorder = BorderFactory.createTitledBorder("Star Point Milestones"); 
		starPanel.setBorder(starBorder);
		starPanel.setLayout(new GridLayout(5, 2));
		starPanel.add(new JLabel("First Star Point Amount:"));
		starPanel.add(this.firstStarPtAmtSpinner);
		starPanel.add(new JLabel("Second Star Point Amount:"));
		starPanel.add(this.secondStarPtAmtSpinner);
		starPanel.add(new JLabel("Third Star Point Amount:"));
		starPanel.add(this.thirdStarPtAmtSpinner);
		starPanel.add(new JLabel("Fourth Star Point Amount:"));
		starPanel.add(this.fourthStarPtAmtSpinner);
		starPanel.add(new JLabel("Fifth Star Point Amount:"));
		starPanel.add(this.fifthStarPtAmtSpinner);
		
		JPanel bodyPanel = new JPanel();
		bodyPanel.setLayout(new BoxLayout(bodyPanel, BoxLayout.Y_AXIS));
		bodyPanel.add(namePanel);
		bodyPanel.add(starPanel);
		
		this.setLayout(new BorderLayout());
		this.add(bodyPanel, BorderLayout.CENTER);
		
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
	
	
	
	private void starPointSpinnerChanged(ChangeEvent evt) {
		// Make sure the points values are all updated
		SpinnerNumberModel firstStarNumModel = (SpinnerNumberModel)this.firstStarPtAmtSpinner.getModel();
		SpinnerNumberModel secondStarNumModel = (SpinnerNumberModel)this.secondStarPtAmtSpinner.getModel();
		SpinnerNumberModel thirdStarNumModel = (SpinnerNumberModel)this.thirdStarPtAmtSpinner.getModel();
		SpinnerNumberModel fourthStarNumModel = (SpinnerNumberModel)this.fourthStarPtAmtSpinner.getModel();
		SpinnerNumberModel fifthStarNumModel = (SpinnerNumberModel)this.fifthStarPtAmtSpinner.getModel();
		
		int firstStarIntValue  = firstStarNumModel.getNumber().intValue();
		int secondStarIntValue = secondStarNumModel.getNumber().intValue();
		int thirdStarIntValue  = thirdStarNumModel.getNumber().intValue();
		int fourthStarIntValue = fourthStarNumModel.getNumber().intValue();
		int fifthStarIntValue  = fifthStarNumModel.getNumber().intValue();
		
		int nextValue = firstStarIntValue;
		if (secondStarIntValue < nextValue) {
			this.secondStarPtAmtSpinner.setValue(new Integer(nextValue));
		}
		else {
			nextValue = secondStarIntValue;
		}
		if (thirdStarIntValue < nextValue) {
			this.thirdStarPtAmtSpinner.setValue(new Integer(nextValue));
		}
		else {
			nextValue = thirdStarIntValue;
		}
		if (fourthStarIntValue < nextValue) {
			this.fourthStarPtAmtSpinner.setValue(new Integer(nextValue));
		}
		else {
			nextValue = fourthStarIntValue;
		}
		if (fifthStarIntValue < nextValue) {
			this.fifthStarPtAmtSpinner.setValue(new Integer(nextValue));
		}
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


		SpinnerNumberModel firstStarNumModel = (SpinnerNumberModel)this.firstStarPtAmtSpinner.getModel();
		SpinnerNumberModel secondStarNumModel = (SpinnerNumberModel)this.secondStarPtAmtSpinner.getModel();
		SpinnerNumberModel thirdStarNumModel = (SpinnerNumberModel)this.thirdStarPtAmtSpinner.getModel();
		SpinnerNumberModel fourthStarNumModel = (SpinnerNumberModel)this.fourthStarPtAmtSpinner.getModel();
		SpinnerNumberModel fifthStarNumModel = (SpinnerNumberModel)this.fifthStarPtAmtSpinner.getModel();
		
		int[] starPointAmts = new int[5];
		starPointAmts[0] = firstStarNumModel.getNumber().intValue();
		starPointAmts[1] = secondStarNumModel.getNumber().intValue();
		starPointAmts[2] = thirdStarNumModel.getNumber().intValue();
		starPointAmts[3] = fourthStarNumModel.getNumber().intValue();
		starPointAmts[4] = fifthStarNumModel.getNumber().intValue();
		this.docWindow.setStarPointAmounts(starPointAmts);
		// TODO: Set the point amounts...
		
		this.setVisible(false);
	}
	
	private void cancelButtonActionPerformed(java.awt.event.ActionEvent evt) {
		this.setVisible(false);
	}
	
}
