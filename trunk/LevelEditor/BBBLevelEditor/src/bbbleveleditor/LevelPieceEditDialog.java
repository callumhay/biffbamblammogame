/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * LevelPieceEditDialog.java
 *
 * Created on Jul 31, 2010, 4:39:13 PM
 */

package bbbleveleditor;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.*;
import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.text.JTextComponent;

public class LevelPieceEditDialog extends JDialog {

	private static final long serialVersionUID = 1L;
	
	// Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton addSiblingBtn;
    private javax.swing.JButton cancelBtn;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JButton okBtn;
    private javax.swing.JTextField pieceIDTxtField;
    private javax.swing.JTextField pieceTypeTxtField;
    private javax.swing.JButton removeSiblingBtn;
    private javax.swing.JList siblingList;
    private DefaultListModel siblingListModel;
    
    private JCheckBox teslaStartsOn;
    private JCheckBox teslaChangable;
    //private JComboBox triggerIDComboBox;
    // End of variables declaration//GEN-END:variables
	
	private Set<Character> allPieceIDs;
	private LevelPieceImageLabel levelPieceLbl;
	private JFrame parentWindow;
	
	private JRadioButton randomCannonAngleRadio;
	private JRadioButton specifyCannonAngleRadio;
	private JSpinner cannonAngleValue1;
	private JSpinner cannonAngleValue2;
	
	private JSpinner switchTriggerIDSpinner;
	
    /** Creates new form LevelPieceEditDialog */
	public LevelPieceEditDialog(JFrame parentWindow, LevelPieceImageLabel levelPieceLbl, Set<Character> allPieceIDs) {
    	super(parentWindow, "Block Properties", true);
    	assert(levelPieceLbl != null);
    	this.parentWindow = parentWindow;
    	this.allPieceIDs = allPieceIDs;
    	this.levelPieceLbl = levelPieceLbl;
    	
        initComponents();
        
        String pieceIDStr = "";
        if (levelPieceLbl.getBlockID() == LevelPieceImageLabel.INVALID_ID) {
        	pieceIDStr = "N/A";
        }
        else {
        	pieceIDStr = "" +  levelPieceLbl.getBlockID();
        }
        
        this.pieceIDTxtField.setText(pieceIDStr);
        this.pieceTypeTxtField.setText("" + levelPieceLbl.getLevelPiece().getName());
        
        // Populate the sibling list with all of the sibling IDs
        Set<Character> siblingIDs = this.levelPieceLbl.getSiblingIDs();
        for (Iterator<Character> iter = siblingIDs.iterator(); iter.hasNext();) {
        	this.siblingListModel.addElement(iter.next());
        }
        
        if (allPieceIDs == null) {
        	this.siblingList.setEnabled(false);
        	this.addSiblingBtn.setEnabled(false);
        	this.removeSiblingBtn.setEnabled(false);
        }
        
		this.setLocationRelativeTo(parentWindow);
		int parentWidth = parentWindow.getWidth();
		int parentHeight = parentWindow.getHeight();
		this.setLocation(new Point((parentWidth - this.getWidth()) / 2, (parentHeight - this.getHeight()) / 2));		
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        pieceIDTxtField = new javax.swing.JTextField();
        pieceTypeTxtField = new javax.swing.JTextField();
        jLabel3 = new javax.swing.JLabel();
        jScrollPane1 = new javax.swing.JScrollPane();
        siblingList = new javax.swing.JList();
        removeSiblingBtn = new javax.swing.JButton();
        addSiblingBtn = new javax.swing.JButton();
        cancelBtn = new javax.swing.JButton();
        okBtn = new javax.swing.JButton();
        
        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);

        jLabel1.setText("Level Piece Type:");

        jLabel2.setText("Level Piece ID:");

        pieceIDTxtField.setText("INVALID_ID");
        pieceIDTxtField.setInheritsPopupMenu(true);
        pieceIDTxtField.setInputVerifier(new InputVerifier() {
            public boolean verify(JComponent input) {
                final JTextComponent source = (JTextComponent) input;
                String text = source.getText();
                if (text.length() != 1) {
                    return false;
                }
                if (!LevelPieceImageLabel.IsValidBlockID(text.charAt(0))) {
                    return false;
                }
                return true;
            }
        });

        pieceTypeTxtField.setEditable(false);
        pieceTypeTxtField.setText("INVALID_TYPE");

        jLabel3.setText("Level Piece Siblings:");

        this.siblingListModel = new DefaultListModel();
        siblingList.setModel(this.siblingListModel);
        jScrollPane1.setViewportView(siblingList);

        removeSiblingBtn.setToolTipText("Remove Sibling");
        removeSiblingBtn.setActionCommand("removeSibling");
        removeSiblingBtn.setMaximumSize(new java.awt.Dimension(25, 25));
        removeSiblingBtn.setMinimumSize(new java.awt.Dimension(25, 25));
        removeSiblingBtn.setPreferredSize(new java.awt.Dimension(25, 25));
        removeSiblingBtn.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                removeSiblingBtnActionPerformed(evt);
            }
        });
        removeSiblingBtn.setIcon(new ImageIcon(BBBLevelEditorMain.class.getResource("resources/minus_icon_16x16.png")));

        addSiblingBtn.setToolTipText("Add Sibling");
        addSiblingBtn.setActionCommand("addSibling");
        addSiblingBtn.setMaximumSize(new java.awt.Dimension(25, 25));
        addSiblingBtn.setMinimumSize(new java.awt.Dimension(25, 25));
        addSiblingBtn.setPreferredSize(new java.awt.Dimension(25, 25));
        addSiblingBtn.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                addSiblingBtnActionPerformed(evt);
            }
        });
        addSiblingBtn.setIcon(new ImageIcon(BBBLevelEditorMain.class.getResource("resources/plus_icon_16x16.png")));

        cancelBtn.setText("Cancel");
        cancelBtn.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cancelBtnActionPerformed(evt);
            }
        });

        okBtn.setText("OK");
        okBtn.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                okBtnActionPerformed(evt);
            }
        });
        
        
        this.getContentPane().setLayout(new BorderLayout());
        
        //javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        //getContentPane().setLayout(layout);
        
        JPanel otherPanel = new JPanel();
        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(otherPanel);
        otherPanel.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 215, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jLabel2)
                            .addComponent(jLabel1))
                        .addGap(23, 23, 23)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(pieceTypeTxtField, javax.swing.GroupLayout.DEFAULT_SIZE, 108, Short.MAX_VALUE)
                            .addComponent(pieceIDTxtField, javax.swing.GroupLayout.DEFAULT_SIZE, 108, Short.MAX_VALUE)))
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addComponent(removeSiblingBtn, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(addSiblingBtn, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addComponent(jLabel3)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addComponent(okBtn, javax.swing.GroupLayout.PREFERRED_SIZE, 63, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(cancelBtn)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel1)
                    .addComponent(pieceTypeTxtField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel2)
                    .addComponent(pieceIDTxtField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(11, 11, 11)
                .addComponent(jLabel3)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 91, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(addSiblingBtn, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(removeSiblingBtn, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE))
                    
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 22, Short.MAX_VALUE)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(cancelBtn)
                    .addComponent(okBtn))
                .addContainerGap())
        );

        this.getContentPane().add(otherPanel, BorderLayout.CENTER);
        
        JPanel buttonPanel = new JPanel();
        buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.PAGE_AXIS));
        
        if (this.levelPieceLbl.getIsTesla()) {
	        this.teslaStartsOn = new JCheckBox("Tesla starts on");
	        this.teslaStartsOn.setSelected(this.levelPieceLbl.getIsTeslaOnBool());
	        this.teslaStartsOn.setEnabled(true);
	        this.teslaStartsOn.addActionListener(new java.awt.event.ActionListener() {
	        	public void actionPerformed(java.awt.event.ActionEvent evt) {
	            	teslaOnCheckBoxClicked(evt);
	            }
	        });
        
	        this.teslaChangable = new JCheckBox("Tesla is changable");
	        this.teslaChangable.setSelected(this.levelPieceLbl.getIsTeslaChangableBool());
	        this.teslaChangable.setEnabled(true);
	        this.teslaChangable.addActionListener(new java.awt.event.ActionListener() {
	        	public void actionPerformed(java.awt.event.ActionEvent evt) {
	            	teslaChangableCheckBoxClicked(evt);
	            }
	        });
       
	        buttonPanel.add(this.teslaStartsOn);
	        buttonPanel.add(this.teslaChangable);
	        
        }
        else if (this.levelPieceLbl.getIsCannonBlock()) {
        	this.randomCannonAngleRadio  = new JRadioButton("Random Angle");
        	this.randomCannonAngleRadio.setEnabled(true);
        	this.randomCannonAngleRadio.addActionListener( new ActionListener() {
        		public void actionPerformed(ActionEvent evt) {
        			cannonAngleRadioClicked(true);
        		}
        	});
        	
        	
        	this.specifyCannonAngleRadio = new JRadioButton("Specify Angle");
        	this.specifyCannonAngleRadio.setEnabled(true);
        	this.specifyCannonAngleRadio.addActionListener( new ActionListener() {
        		public void actionPerformed(ActionEvent evt) {
        			cannonAngleRadioClicked(false);
        		}
        	});
        	this.specifyCannonAngleRadio.setSelected(false);
        	
        	ButtonGroup group = new ButtonGroup();
        	group.add(this.randomCannonAngleRadio);
        	group.add(this.specifyCannonAngleRadio);
        	
        	SpinnerNumberModel spinnerModel1 = new SpinnerNumberModel(0, 0, 356, 1);
        	this.cannonAngleValue1 = new JSpinner(spinnerModel1);
        	this.cannonAngleValue1.setEnabled(false);
        	this.cannonAngleValue1.addChangeListener(new ChangeListener() {

				public void stateChanged(ChangeEvent arg0) {
					cannonAngleValueChanged(true, (Integer)cannonAngleValue1.getValue());
				}
        	});
        	SpinnerNumberModel spinnerModel2 = new SpinnerNumberModel(0, 0, 356, 1);
        	this.cannonAngleValue2 = new JSpinner(spinnerModel2);
        	this.cannonAngleValue2.setEnabled(false);
        	this.cannonAngleValue2.addChangeListener(new ChangeListener() {

				public void stateChanged(ChangeEvent arg0) {
					cannonAngleValueChanged(false, (Integer)cannonAngleValue2.getValue());
				}
        	});
        	
        	
        	if (this.levelPieceLbl.getCannonBlockDegAngle1() == 0 && this.levelPieceLbl.getCannonBlockDegAngle2() == 359) {
        		this.randomCannonAngleRadio.setSelected(true);
        		this.specifyCannonAngleRadio.setSelected(false);
        		this.cannonAngleValue1.setEnabled(false);
        		this.cannonAngleValue2.setEnabled(false);
        	}
        	else {
        		this.cannonAngleValue1.setValue(new Integer(this.levelPieceLbl.getCannonBlockDegAngle1()));
        		this.cannonAngleValue2.setValue(new Integer(this.levelPieceLbl.getCannonBlockDegAngle2()));
        		this.randomCannonAngleRadio.setSelected(false);
        		this.specifyCannonAngleRadio.setSelected(true);
        		this.cannonAngleValue1.setEnabled(true);
        		this.cannonAngleValue2.setEnabled(true);
        	}
        		
        	JPanel tempPanel = new JPanel(new FlowLayout());
        	tempPanel.add(this.randomCannonAngleRadio);
        	tempPanel.add(this.specifyCannonAngleRadio);
        	tempPanel.add(this.cannonAngleValue1);
        	tempPanel.add(new JLabel("to"));
        	tempPanel.add(this.cannonAngleValue2);

	        buttonPanel.add(tempPanel);
        }
        else if (this.levelPieceLbl.getIsSwitchBlock()) {
        	this.switchTriggerIDSpinner = new JSpinner(new SpinnerNumberModel(0, 0, 10000, 1));
        	this.switchTriggerIDSpinner.setValue(new Integer(this.levelPieceLbl.getSwitchTriggerID()));
        	this.switchTriggerIDSpinner.addChangeListener(new ChangeListener() {
				public void stateChanged(ChangeEvent arg0) {
					switchTriggerableIDChanged((Integer)switchTriggerIDSpinner.getValue());
				}
        	});
        	
        	JPanel tempPanel = new JPanel(new FlowLayout());
        	tempPanel.add(new JLabel("ID to Trigger on Switch:"));
        	tempPanel.add(this.switchTriggerIDSpinner);
        	
        	buttonPanel.add(tempPanel);
        }
        
        this.getContentPane().add(buttonPanel, BorderLayout.SOUTH);
        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void removeSiblingBtnActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_removeSiblingBtnActionPerformed
    	
    	int[] selectedIndices = this.siblingList.getSelectedIndices();
    	if (selectedIndices.length == this.siblingListModel.getSize()) {
    		JOptionPane.showMessageDialog(this, "Cannot remove all siblings from the list!", "Illegal operation", JOptionPane.ERROR_MESSAGE);
    		return;
    	}
    	for (int i = 0; i < selectedIndices.length; i++) {
    		this.siblingListModel.remove(selectedIndices[i]);
    	}
    	
    }//GEN-LAST:event_removeSiblingBtnActionPerformed

    private void addSiblingBtnActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_addSiblingBtnActionPerformed
    	if (this.allPieceIDs == null) {
    		return;
    	}
    	
        // Open up a dialog with a combo box filled with possible siblings
    	assert(this.allPieceIDs.size() >= 1);
    	Set<Character> allowableSiblings = new TreeSet<Character>();
    	Iterator<Character> iter = this.allPieceIDs.iterator();
    	while (iter.hasNext()) {
    		Character currIterChar = iter.next();
    		if (currIterChar != this.levelPieceLbl.getBlockID() &&
    			!this.siblingListModel.contains(currIterChar)) {
    			allowableSiblings.add(currIterChar);
    		}
    	}
    	
    	if (allowableSiblings.size() == 0) {
    		JOptionPane.showMessageDialog(this.parentWindow, "No other siblings are available!", "Error", JOptionPane.WARNING_MESSAGE);
    		return;
    	}
    	
    	Character c = (Character)JOptionPane.showInputDialog(
                this.parentWindow,
                "Select a sibling ID to add:",
                "Add a Sibling",
                JOptionPane.PLAIN_MESSAGE,
                null,
                allowableSiblings.toArray(),
                allowableSiblings.iterator().next());
    	
    	if (c == null) {
    		return;
    	}
    	
    	this.siblingListModel.addElement(c);
    }//GEN-LAST:event_addSiblingBtnActionPerformed

    private void okBtnActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_okBtnActionPerformed
    	// Update the ID siblings and other attributes...
    	this.updateSiblingsFromList();
    	
    	if (this.cannonAngleValue1 != null && this.cannonAngleValue2 != null) {
	    	int cannonValue1 = (Integer)this.cannonAngleValue1.getValue();
	    	int cannonValue2 = (Integer)this.cannonAngleValue2.getValue();
	    	if (cannonValue1 > cannonValue2) {
	    		cannonValue1 = cannonValue2;
	    	}
	    	
	    	this.levelPieceLbl.setCannonBlockDegAngle1(cannonValue1);
	    	this.levelPieceLbl.setCannonBlockDegAngle2(cannonValue2);
    	}
    	
    	this.setVisible(false);
    }//GEN-LAST:event_okBtnActionPerformed

    private void cancelBtnActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cancelBtnActionPerformed
        // Don't update anything, just exit...
    	this.setVisible(false);
    }//GEN-LAST:event_cancelBtnActionPerformed

    private void teslaOnCheckBoxClicked(java.awt.event.ActionEvent evt) {
    	this.levelPieceLbl.setTeslaStartsOn(this.teslaStartsOn.isSelected());
    }
    
    private void teslaChangableCheckBoxClicked(java.awt.event.ActionEvent evt) {
    	this.levelPieceLbl.setTeslaChangable(this.teslaChangable.isSelected());
    }
    
    private void cannonAngleRadioClicked(boolean random) {
    	if (random) {
    		this.levelPieceLbl.setCannonBlockDegAngle1(-1);
    		this.cannonAngleValue1.setEnabled(false);
    		this.cannonAngleValue2.setEnabled(false);
    	}
    	else {
    		this.cannonAngleValue1.setEnabled(true);
    		this.cannonAngleValue2.setEnabled(true);
    		this.cannonAngleValueChanged(true,  (Integer)this.cannonAngleValue1.getValue());
    		this.cannonAngleValueChanged(false, (Integer)this.cannonAngleValue2.getValue());
    	}
    	
    }
    
	private void cannonAngleValueChanged(boolean startValue, int value) {
		if (startValue) {
			this.levelPieceLbl.setCannonBlockDegAngle1(value);
		}
		else {
			this.levelPieceLbl.setCannonBlockDegAngle2(value);
		}
	}
    
	private void switchTriggerableIDChanged(int value) {
		this.levelPieceLbl.setSwitchTriggerID(value);
	}
	
    private void updateSiblingsFromList() {
    	Set<Character> siblings = new TreeSet<Character>();
    	for (int i = 0; i < this.siblingListModel.getSize(); i++) {
    		siblings.add((Character)this.siblingListModel.get(i));
    	}
    	this.levelPieceLbl.setSiblingIDs(siblings);
    }
    
    
    
    
}
