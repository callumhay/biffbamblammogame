package bbbleveleditor;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

@SuppressWarnings("serial")
public class EditDimensionsDialog extends JDialog implements ActionListener {
	
	static private final double LEVEL_PIECE_WIDTH = 2.5;
	
	private int newWidth;
	private int newHeight;
	private double newPaddleXPos;
	
	/*
	private JToggleButton upButton;
	private JToggleButton leftButton;
	private JToggleButton centerButton;
	private JToggleButton rightButton;
	private JToggleButton downButton;
	*/
	
	private JSpinner widthSpinner;
	private JSpinner heightSpinner;
	
	private JSpinner paddleStartingXPosSpinner;
	private JCheckBox defaultPaddleStartingXPosChkBox;
	
	private JButton okButton;
	private JButton cancelButton;
	private boolean exitWithOK;
	
	public EditDimensionsDialog(JFrame window, int currWidth, int currHeight, double currPaddleXPos) {
		super(window, "Level Dimensions", true);
		this.newWidth = currWidth;
		this.newHeight = currHeight;
		this.newPaddleXPos = currPaddleXPos;
		this.exitWithOK = false;
		
		/*
		// Setup the orientation buttons - these determine the direction that
		// the dimensions will expand/contract in.
		this.upButton = new JToggleButton(new ImageIcon(BBBLevelEditorMain.class.getResource("resources/up_arrow.png")));
		this.upButton.addActionCommand("up");
		this.leftButton = new JToggleButton(new ImageIcon(BBBLevelEditorMain.class.getResource("resources/left_arrow.png")));
		this.upButton.addActionCommand("left");
		this.centerButton = new JToggleButton("o");
		this.upButton.addActionCommand("center");
		this.rightButton = new JToggleButton(new ImageIcon(BBBLevelEditorMain.class.getResource("resources/right_arrow.png")));
		this.upButton.addActionCommand("right");
		this.downButton = new JToggleButton(new ImageIcon(BBBLevelEditorMain.class.getResource("resources/down_arrow.png"))); 
		this.downButton.addActionCommand("down");
		
		JPanel emptyPanel = new JPanel();
		JPanel orientButtonPanel = new JPanel(new GridLayout(3,3));
		orientButtonPanel.add(emptyPanel);
		orientButtonPanel.add(this.upButton);
		orientButtonPanel.add(emptyPanel);
		orientButtonPanel.add(this.leftButton);
		orientButtonPanel.add(this.centerButton);
		orientButtonPanel.add(this.rightButton);
		orientButtonPanel.add(emptyPanel);
		orientButtonPanel.add(this.downButton);
		orientButtonPanel.add(emptyPanel);		
		*/
		
		JLabel widthLabel = new JLabel("Width:");
		JLabel heightLabel = new JLabel("Height:");
		this.widthSpinner = new JSpinner(new SpinnerNumberModel(this.newWidth, 3, 999, 1));
		this.widthSpinner.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent evt) {
				widthSpinnerChanged(evt);
			}
		});
		this.heightSpinner = new JSpinner(new SpinnerNumberModel(this.newHeight, 3, 999, 1));
		this.heightSpinner.addChangeListener(new ChangeListener() {
			public void stateChanged(ChangeEvent evt) {
				heightSpinnerChanged(evt);
			}
		});
		
		
		JLabel paddleXPosLabel = new JLabel("Paddle Starting X Position:");
		this.paddleStartingXPosSpinner = new JSpinner(new SpinnerNumberModel(0.0, 0.0, LEVEL_PIECE_WIDTH * ((Integer)this.widthSpinner.getValue()).intValue(), 0.5));
		
		JSpinner.DefaultEditor editor = ((JSpinner.DefaultEditor)this.paddleStartingXPosSpinner.getEditor());
		Dimension editorDim = editor.getPreferredSize();
		editor.setPreferredSize(new Dimension(50, editorDim.height));
		
		this.defaultPaddleStartingXPosChkBox = new JCheckBox("Default");
		this.defaultPaddleStartingXPosChkBox.addActionListener(new java.awt.event.ActionListener() {
        	public void actionPerformed(java.awt.event.ActionEvent evt) {
        		defaultPaddleStartingXPosChkBoxActionPerformed(evt);
            }
		});
		
		JPanel paddleXPosPanel = new JPanel();
		paddleXPosPanel.setLayout(new FlowLayout());
		paddleXPosPanel.add(paddleXPosLabel);
		paddleXPosPanel.add(this.paddleStartingXPosSpinner);
		paddleXPosPanel.add(this.defaultPaddleStartingXPosChkBox);
		
		if (this.newPaddleXPos < 0) {
			this.defaultPaddleStartingXPosChkBox.setSelected(true);
			this.paddleStartingXPosSpinner.setEnabled(false);
		}
		else {
			this.defaultPaddleStartingXPosChkBox.setSelected(false);
			this.paddleStartingXPosSpinner.setValue(new Double(this.newPaddleXPos));
			this.paddleStartingXPosSpinner.setEnabled(true);
		}
		
		JPanel dimPanel = new JPanel();
		dimPanel.setLayout(new FlowLayout());
		dimPanel.add(widthLabel);
		dimPanel.add(this.widthSpinner);
		dimPanel.add(heightLabel);
		dimPanel.add(this.heightSpinner);
		
		JPanel centerPanel = new JPanel();
		centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.Y_AXIS));
		centerPanel.add(dimPanel);
		centerPanel.add(paddleXPosPanel);
		
		this.setLayout(new BorderLayout());
		this.add(centerPanel, BorderLayout.CENTER);
		
		// Setup the ok and cancel buttons
		this.okButton = new JButton("OK");
		this.okButton.setActionCommand("OK");
		this.okButton.addActionListener(this);
		this.cancelButton = new JButton("Cancel");
		this.cancelButton.setActionCommand("Cancel");
		this.cancelButton.addActionListener(this);
		
		JPanel okCancelButtonPanel = new JPanel(new FlowLayout());
		okCancelButtonPanel.add(this.okButton);
		okCancelButtonPanel.add(this.cancelButton);
		
		this.add(okCancelButtonPanel, BorderLayout.SOUTH);
		
		this.setMinimumSize(new Dimension(200, 0));
		this.setResizable(false);
		
		this.setLocationRelativeTo(window);
		int parentWidth = window.getWidth();
		int parentHeight = window.getHeight();
		
		this.setLocation(new Point((parentWidth - this.getWidth()) / 2, (parentHeight - this.getHeight()) / 2));		
		
		this.pack();
	}

	public int getNewWidth() {
		return this.newWidth;
	}
	public int getNewHeight() {
		return this.newHeight;
	}
	public double getNewPaddleXPos() {
		return this.newPaddleXPos;
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("OK")) {
			this.newWidth  = ((Integer)this.widthSpinner.getValue()).intValue();
			this.newHeight = ((Integer)this.heightSpinner.getValue()).intValue();
			
			if (this.defaultPaddleStartingXPosChkBox.isSelected()) {
				this.newPaddleXPos = -1;
			}
			else {
				Double doubleVal = (Double)this.paddleStartingXPosSpinner.getValue();
				this.newPaddleXPos = doubleVal.doubleValue();
			}
			
			this.exitWithOK = true;
			this.setVisible(false);
		}
		else if (e.getActionCommand().equals("Cancel")) {
			this.exitWithOK = false;
			this.setVisible(false);
		}
	}
	
	private void widthSpinnerChanged(ChangeEvent evt) {
		SpinnerNumberModel xPosSpinnerModel = (SpinnerNumberModel)this.paddleStartingXPosSpinner.getModel();
		Integer widthIntVal = (Integer)(this.widthSpinner.getValue());
		xPosSpinnerModel.setMaximum(new Double(LEVEL_PIECE_WIDTH * widthIntVal.intValue()));
	}
	private void heightSpinnerChanged(ChangeEvent evt) {
	}
	
	private void defaultPaddleStartingXPosChkBoxActionPerformed(ActionEvent evt) {
		if (this.defaultPaddleStartingXPosChkBox.isSelected()) {
			this.paddleStartingXPosSpinner.setEnabled(false);
			this.newPaddleXPos = -1;
		}
		else {
			this.paddleStartingXPosSpinner.setEnabled(true);
			this.newPaddleXPos = ((Double)this.paddleStartingXPosSpinner.getValue()).doubleValue();
		}
	}
	
	public boolean getExitedWithOK() {
		return this.exitWithOK;
	}
}
